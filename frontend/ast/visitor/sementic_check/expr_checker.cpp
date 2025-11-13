#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::visit(LeftValExpr& node)
    {
        // 检查变量是否存在
        auto varAttr = symTable.getSymbol(node.entry);
        if (!varAttr)
        {
            errors.push_back("Error: Variable " + node.entry->getName() + " not declared at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }
        
        bool res = true;

        if (node.isDeclaration)
        {
            // 数组声明：检查维度是否为编译期常量
            if (node.indices)
            {
                for (auto& index : *node.indices)
                {
                    if (!apply(*this, *index))
                    {
                        res = false;
                        continue;
                    }
                    if (!index->attr.val.isConstexpr || index->attr.val.value.type != intType)
                    {
                        errors.push_back("Error: Array dimension must be a compile-time constant of type int at line " +
                                        std::to_string(node.line_num) + ", column " +
                                        std::to_string(node.col_num) + ".");
                        res = false;
                        continue;
                    }
                    int dimValue = index->attr.val.value.getInt();
                    if (dimValue <= 0)
                    {
                        errors.push_back("Error: Array dimension must be greater than zero at line " +
                                        std::to_string(node.line_num) + ", column " +
                                        std::to_string(node.col_num) + ".");
                        res = false;
                        continue;
                    }
                    varAttr->arrayDims.push_back(dimValue);
                }
            }
        }
        else
        {   
            // 数组调用：检查下标是否为整数类型
            if (node.indices)
            {
                if (node.indices->size() > varAttr->arrayDims.size())
                {
                    errors.push_back("Error: Array subscript count mismatch for variable " + node.entry->getName() + " "
                                    + std::to_string(node.indices->size()) + " vs " + std::to_string(varAttr->arrayDims.size()) +
                                    " at line " + std::to_string(node.line_num) + ", column " +
                                    std::to_string(node.col_num) + ".");
                    res = false;
                }

                for (size_t i = 0; i < node.indices->size(); ++i)
                {
                    auto& index = (*node.indices)[i];
                    if (!apply(*this, *index))
                    {
                        res = false;
                        continue;
                    }
                    if (index->attr.val.value.type != intType)
                    {
                        errors.push_back("Error: Array subscript must be of type int at line " +
                                        std::to_string(node.line_num) + ", column " +
                                        std::to_string(node.col_num) + ".");
                        res = false;
                        continue;
                    }
                    // 检查下标是否越界
                    int subscript = index->attr.val.value.getInt();
                    if (subscript < 0 || subscript >= varAttr->arrayDims[i])
                    {
                        errors.push_back("Error: Array subscript out of bounds at line " +
                                        std::to_string(node.line_num) + ", column " +
                                        std::to_string(node.col_num) + ".");
                        res = false;
                        continue;
                    }
                }
            }
        }

        // 设置表达式属性
        node.attr.val.value = varAttr->initList.begin() != varAttr->initList.end() ? *(varAttr->initList.begin()) : VarValue();
        node.attr.val.value.type = varAttr->type;
        node.attr.val.isConstexpr = varAttr->isConstDecl;
        return res;
    }

    bool ASTChecker::visit(LiteralExpr& node)
    {
        // 示例实现：字面量表达式的语义检查
        // 字面量总是编译期常量，直接设置属性
        node.attr.val.isConstexpr = true;
        node.attr.val.value       = node.literal;
        return true;
    }

    bool ASTChecker::visit(UnaryExpr& node)
    {
        // 访问子表达式
        if (!apply(*this, *node.expr))
            return false;

        // 检查操作数类型并进行隐式类型转换
        if (node.op == Operator::NOT)
        {
            if (node.expr->attr.val.value.type == intType)
            {
                // 隐式转换 int -> bool
                node.expr->attr.val.value.type = boolType;
            }
            else if (node.expr->attr.val.value.type != boolType)
            {
                errors.push_back("Error: Unary NOT operator requires a boolean or integer operand at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }
        }

        // 设置表达式属性
        node.attr.val.value.type = node.expr->attr.val.value.type;
        node.attr.val.isConstexpr = node.expr->attr.val.isConstexpr;
        return true;
    }

    bool ASTChecker::visit(BinaryExpr& node)
    {
        // 访问左右子表达式
        if (!apply(*this, *node.lhs) || !apply(*this, *node.rhs))
            return false;

        // 检查操作数是否为 void 类型
        if (node.lhs->attr.val.value.type == voidType || node.rhs->attr.val.value.type == voidType)
        {
            errors.push_back("Error: Operands of binary expression cannot be of type void at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }
        
        // 检查赋值操作
        if (node.op == Operator::ASSIGN)
        {   
            // 检查左操作数是否为左值
            auto* leftValExpr = dynamic_cast<LeftValExpr*>(node.lhs);
            if (!leftValExpr)
            {
                errors.push_back("Error: Left operand of assignment must be a left value at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }

            // 检查是否对 const 变量进行赋值
            auto varAttr = symTable.getSymbol(leftValExpr->entry);            
            if (varAttr->isConstDecl && varAttr->isInitialized)
            {
                errors.push_back("Error: Cannot assign to const variable " + leftValExpr->entry->getName() + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }
            varAttr->isInitialized = true;

            // 检查类型兼容性
            if (node.lhs->attr.val.value.type != node.rhs->attr.val.value.type)
            {
                // 如果需要，可以在这里添加隐式类型转换逻辑
                errors.push_back("Error: Type mismatch in assignment at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }

            node.attr.val.isConstexpr = false;  // 赋值表达式不是编译期常量
            return true;
        }
        // 检查操作数类型并进行隐式类型转换
        else if (node.op == Operator::ADD || node.op == Operator::SUB ||
            node.op == Operator::MUL || node.op == Operator::DIV || node.op == Operator::MOD)
        {
            if (node.lhs->attr.val.value.type == boolType)
            {
                // 隐式转换 bool -> int
                node.lhs->attr.val.value.type = intType;
            }
            if (node.rhs->attr.val.value.type == boolType)
            {
                // 隐式转换 bool -> int
                node.rhs->attr.val.value.type = intType;
            }

            if (node.lhs->attr.val.value.type != intType ||
                node.rhs->attr.val.value.type != intType)
            {
                errors.push_back("Error: Arithmetic operators require integer operands at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }
            
            // 常量折叠
            if (node.lhs->attr.val.isConstexpr && node.rhs->attr.val.isConstexpr)
            {
                int lhsValue = node.lhs->attr.val.value.intValue;
                int rhsValue = node.rhs->attr.val.value.intValue;
                int result = 0;

                switch (node.op)
                {
                case Operator::ADD:
                    result = lhsValue + rhsValue;
                    break;
                case Operator::SUB:
                    result = lhsValue - rhsValue;
                    break;
                case Operator::MUL:
                    result = lhsValue * rhsValue;
                    break;
                case Operator::MOD:
                    result = lhsValue % rhsValue;
                    break;
                case Operator::DIV:
                    if (rhsValue == 0)
                    {
                        errors.push_back("Error: Division by zero at line " +
                                        std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                        return false;
                    }
                    result = lhsValue / rhsValue;
                    break;
                default:
                    break;
                }

                // 设置结果为编译期常量
                node.attr.val.isConstexpr = true;
                node.attr.val.value.intValue = result;
            }
            else
            {
                // 如果不是常量，设置为非编译期常量
                node.attr.val.isConstexpr = false;
            }

            // 检查除法中的除数是否为 0
            if (node.op == Operator::DIV && node.rhs->attr.val.isConstexpr && node.rhs->attr.val.value.intValue == 0)
            {
                errors.push_back("Error: Division by zero at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }

        }
        else if (node.op == Operator::AND || node.op == Operator::OR)
        {
            if (node.lhs->attr.val.value.type == intType)
            {
                // 隐式转换 int -> bool
                node.lhs->attr.val.value.type = boolType;
            }
            if (node.rhs->attr.val.value.type == intType)
            {
                // 隐式转换 int -> bool
                node.rhs->attr.val.value.type = boolType;
            }

            if (node.lhs->attr.val.value.type != boolType ||
                node.rhs->attr.val.value.type != boolType)
            {
                errors.push_back("Error: Logical operators require boolean operands at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }
        }

        // 设置表达式属性
        node.attr.val.value.type = node.lhs->attr.val.value.type;
        return true;
    }

    bool ASTChecker::visit(CallExpr& node)
    {
        // 检查函数是否存在
        auto it = funcDecls.find(node.func);
        if (it == funcDecls.end())
        {
            errors.push_back("Error: Function " + node.func->getName() + " not declared at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        auto funcDecl = it->second;

        // 检查参数数量和类型
        if (node.args && funcDecl->params)
        {
            if (node.args->size() != funcDecl->params->size())
            {
                errors.push_back("Error: Argument count mismatch for function " + node.func->getName() + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }

            for (size_t i = 0; i < node.args->size(); ++i)
            {
                if (!apply(*this, *(*node.args)[i]))
                    return false;
                if ((*node.args)[i]->attr.val.value.type != (*funcDecl->params)[i]->attr.val.value.type)
                {
                    errors.push_back("Error: Argument type mismatch for function " + node.func->getName() + " at line " + std::to_string((*node.args)[i]->line_num) + ", column " + std::to_string((*node.args)[i]->col_num) + ".");
                    return false;
                }
            }
        }

        // 设置表达式属性
        node.attr.val.value.type = funcDecl->retType;
        node.attr.val.isConstexpr = false;
        return true;
    }

    bool ASTChecker::visit(CommaExpr& node)
    {
        // 依次访问所有子表达式
        for (auto& expr : *node.exprs)
        {
            if (!apply(*this, *expr))
                return false;
        }

        // 结果属性继承最后一个子表达式
        node.attr = node.exprs->back()->attr;
        return true;
    }

}  // namespace FE::AST
