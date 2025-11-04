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
            errors.push_back("Error: Variable " + node.entry->getName() + " not declared.");
            return false;
        }

        // 检查数组下标
        if (node.indices)
        {
            for (auto& index : *node.indices)
            {
                if (!apply(*this, *index))
                    return false;
                if (index->attr.val.value.type != intType)
                {
                    errors.push_back("Error: Array index must be of type int.");
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

        // 检查操作数类型
        if (node.op == Operator::NOT && node.expr->attr.val.value.type != boolType)
        {
            errors.push_back("Error: Unary NOT operator requires a boolean operand.");
            return false;
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

        // 检查操作数类型
        if (node.op == Operator::ADD || node.op == Operator::SUB ||
            node.op == Operator::MUL || node.op == Operator::DIV)
        {
            if (node.lhs->attr.val.value.type != intType ||
                node.rhs->attr.val.value.type != intType)
            {
                errors.push_back("Error: Arithmetic operators require integer operands.");
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
        auto funcAttr = symTable.getSymbol(node.func);
        if (!funcAttr)
        {
            errors.push_back("Error: Function " + node.func->getName() + " not declared.");
            return false;
        }

        // 检查参数数量和类型
        if (node.args && funcAttr->initList.empty())
        {
            if (node.args->size() != funcAttr->initList.size())
            {
                errors.push_back("Error: Argument count mismatch for function " + node.func->getName() + ".");
                return false;
            }

            for (size_t i = 0; i < node.args->size(); ++i)
            {
                if (!apply(*this, *(*node.args)[i]))
                    return false;
                if ((*node.args)[i]->attr.val.value.type != funcAttr->initList[i].type)
                {
                    errors.push_back("Error: Argument type mismatch for function " + node.func->getName() + ".");
                    return false;
                }
            }
        }

        // 设置表达式属性
        node.attr.val.value.type = funcAttr->type;
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
