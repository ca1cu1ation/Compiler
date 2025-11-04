#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::visit(ExprStmt& node)
    {
        // 示例实现：表达式语句的语义检查
        // 空表达式直接通过，否则访问内部表达式
        if (!node.expr) return true;
        return apply(*this, *node.expr);
    }

    bool ASTChecker::visit(FuncDeclStmt& node)
    {
        // 检查函数是否已在当前作用域中声明
        if (symTable.getSymbol(node.entry))
        {
            errors.push_back("Error: Function " + node.entry->getName() + " already declared.");
            return false;
        }

        // 将函数添加到符号表
        VarAttr varAttr;
        varAttr.type = node.retType;
        varAttr.isConstDecl = false;
        symTable.addSymbol(node.entry, varAttr);

        // 检查形参
        if (node.params)
        {
            for (auto& param : *node.params)
            {
                if (!apply(*this, *param))
                    return false;
            }
        }

        // 检查函数体
        if (node.body && !apply(*this, *node.body))
            return false;

        return true;
    }

    bool ASTChecker::visit(VarDeclStmt& node)
    {
        // 检查变量声明语句
        bool accept = apply(*this, *node.decl);
        if (!accept){
            errors.push_back("Error: Invalid variable declaration.");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(BlockStmt& node)
    {
        // 进入新作用域
        symTable.enterScope();

        // 逐条检查语句
        for (auto& stmt : *node.stmts)
        {
            if (!apply(*this, *stmt))
            {
                symTable.exitScope();
                return false;
            }
        }

        // 退出作用域
        symTable.exitScope();
        return true;
    }

    bool ASTChecker::visit(ReturnStmt& node)
    {
        // 检查返回值类型是否匹配
        if (node.retExpr && !apply(*this, *node.retExpr))
            return false;

        // 设置返回标记
        funcHasReturn = true;
        return true;
    }

    bool ASTChecker::visit(WhileStmt& node)
    {
        // 检查条件表达式
        if (!apply(*this, *node.cond))
            return false;

        // 管理循环深度
        loopDepth++;
        bool result = apply(*this, *node.body);
        loopDepth--;

        return result;
    }

    bool ASTChecker::visit(IfStmt& node)
    {
        // 检查条件表达式
        if (!apply(*this, *node.cond))
            return false;

        // 检查 then 分支
        if (!apply(*this, *node.thenStmt))
            return false;

        // 检查 else 分支（如果存在）
        if (node.elseStmt && !apply(*this, *node.elseStmt))
            return false;

        return true;
    }

    bool ASTChecker::visit(BreakStmt& node)
    {
        (void)node;
        // 检查是否在循环内
        if (loopDepth == 0)
        {
            errors.push_back("Error: Break statement not within a loop.");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(ContinueStmt& node)
    {
        (void)node;
        // 检查是否在循环内
        if (loopDepth == 0)
        {
            errors.push_back("Error: Continue statement not within a loop.");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(ForStmt& node)
    {
        // 检查初始化表达式
        if (node.init && !apply(*this, *node.init))
            return false;

        // 检查条件表达式
        if (node.cond && !apply(*this, *node.cond))
            return false;

        // 检查步进表达式
        if (node.step && !apply(*this, *node.step))
            return false;

        // 管理循环深度
        loopDepth++;
        bool result = apply(*this, *node.body);
        loopDepth--;

        return result;
    }
}  // namespace FE::AST
