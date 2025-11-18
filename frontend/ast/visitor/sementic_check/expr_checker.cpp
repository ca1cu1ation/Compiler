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
                size_t offset = 0;
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
                    size_t mul = 1;
                    for (size_t j = i + 1; j < varAttr->arrayDims.size(); ++j) {
                        mul *= varAttr->arrayDims[j];
                    }
                    offset += subscript * mul;
                }
                // 计算偏移量并获取数组元素值
                if( offset < varAttr->initList.size()) node.attr.val.value = varAttr->initList[offset];
            }
            else{
                // 非数组调用，直接获取变量值
                node.attr.val.value = varAttr->initList.begin() != varAttr->initList.end() ? *(varAttr->initList.begin()) : VarValue();
            }
        }

            // 设置表达式属性
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

        bool res = true;

        // 检查操作数类型并进行隐式类型转换
        if (node.op == Operator::NOT || node.op == Operator::SUB || node.op == Operator::ADD)
        {
            bool hasError = false;
            ExprValue result = typeInfer(node.expr->attr.val, node.op, node, hasError);
            if (hasError) {
                errors.push_back("Error: Unary operator requires a right operand at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                res = false;
            }
            node.expr->attr.val = result;
        }

        // 设置表达式属性
        node.attr.val = node.expr->attr.val;
        return res;
    }

    bool ASTChecker::visit(BinaryExpr& node)
    {
        // 访问左右子表达式
        if (!apply(*this, *node.lhs) || !apply(*this, *node.rhs))
            return false;

        bool res = true;

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

            // 检查类型兼容性
            bool hasError = false;
            ExprValue result = typeInfer(node.lhs->attr.val, node.rhs->attr.val, node.op, node, hasError);
            if (hasError) {
                res = false;
            }
            node.attr.val = result;

            // 检查是否对 const 变量进行赋值
            auto varAttr = symTable.getSymbol(leftValExpr->entry);            
            if (varAttr->isConstDecl && varAttr->isInitialized)
            {
                errors.push_back("Error: Cannot assign to const variable " + leftValExpr->entry->getName() + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                return false;
            }
            varAttr->isInitialized = true;
            varAttr->initList.push_back(node.attr.val.value);

            node.attr.val.isConstexpr = false;  // 赋值表达式不是编译期常量
            return res;
        }
        // 检查操作数类型并进行隐式类型转换和常量折叠
        else if (node.op == Operator::ADD || node.op == Operator::SUB ||
            node.op == Operator::MUL || node.op == Operator::DIV || node.op == Operator::MOD ||
            node.op == Operator::AND || node.op == Operator::OR || node.op == Operator::GT ||
            node.op == Operator::GE || node.op == Operator::LT || node.op == Operator::LE ||
            node.op == Operator::EQ || node.op == Operator::NEQ ||
            node.op == Operator::BITOR || node.op == Operator::BITAND)
        {
            // 隐式类型转换和常量折叠统一交给 typeInfer
            bool hasError = false;
            ExprValue result = typeInfer(node.lhs->attr.val, node.rhs->attr.val, node.op, node, hasError);
            if (hasError) {
                res = false;
            }
            node.attr.val = result;
            return res;
        }
        else
        {            
            errors.push_back("Error: Invalid binary operator at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }
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
                // 检查参数类型是否匹配
                VarValue converted = convertType((*node.args)[i]->attr.val.value, (*funcDecl->params)[i]->attr.val.value.type);
                if (converted.type != (*funcDecl->params)[i]->attr.val.value.type)
                {
                    errors.push_back("Error: Argument type mismatch for function " + node.func->getName() + " at line " + std::to_string((*node.args)[i]->line_num) + ", column " + std::to_string((*node.args)[i]->col_num) + ".");
                    return false;
                }
                // 进一步检查数组参数的维度匹配                
                if((*funcDecl->params)[i]->dims != nullptr){                    
                    auto leftValExpr = dynamic_cast<LeftValExpr*>((*node.args)[i]);
                    if(!leftValExpr){
                        errors.push_back("Error: Array argument must be a left value at line " + std::to_string((*node.args)[i]->line_num) + ", column " + std::to_string((*node.args)[i]->col_num) + ".");
                        return false;
                    }
                    auto sym = symTable.getSymbol(leftValExpr->entry);
                    size_t offset = leftValExpr->indices != nullptr ? leftValExpr->indices->size():0;                                    
                    if((*funcDecl->params)[i]->dims->size() != sym->arrayDims.size() - offset){
                        errors.push_back("Error: Array argument dimension count mismatch at line " + std::to_string((*node.args)[i]->line_num) + ", column " + std::to_string((*node.args)[i]->col_num) + ".");
                        return false;
                    }
                    for(size_t d = 0; d < (*funcDecl->params)[i]->dims->size(); ++d){
                        if(d==0) continue; // 第一个维度不需要匹配
                        if((*funcDecl->params)[i]->dims->at(d)->attr.val.value.intValue != (sym->arrayDims)[d + offset]){
                            errors.push_back("Error: Array argument dimension size mismatch at line " + std::to_string((*node.args)[i]->line_num) + ", column " + std::to_string((*node.args)[i]->col_num) + ".");
                            return false;
                        }
                    }               
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
