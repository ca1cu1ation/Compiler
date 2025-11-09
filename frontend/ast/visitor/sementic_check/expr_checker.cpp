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
        // 检查是否对 const 变量进行赋值
        if (varAttr->isConstDecl && varAttr->isInitialized)
        {
            errors.push_back("Error: Cannot assign to const variable " + node.entry->getName() + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        // 设置变量为已初始化
        varAttr->isInitialized = true;
        
        // 检查数组下标
        if (node.indices)
        {
            for (auto& index : *node.indices)
            {
                if (!apply(*this, *index))
                    return false;
                if (index->attr.val.value.type != intType)
                {
                    errors.push_back("Error: Array index must be of type int at line " + std::to_string(index->line_num) + ", column " + std::to_string(index->col_num) + ".");
                    return false;
                }
            }
        }

        // 设置表达式属性
        node.attr.val.value.type = varAttr->type;
        node.attr.val.isConstexpr = varAttr->isConstDecl;
        return true;
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

        // 检查操作数类型并进行隐式类型转换
        if (node.op == Operator::ADD || node.op == Operator::SUB ||
            node.op == Operator::MUL || node.op == Operator::DIV)
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
        node.attr.val.isConstexpr = node.lhs->attr.val.isConstexpr && node.rhs->attr.val.isConstexpr;
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
