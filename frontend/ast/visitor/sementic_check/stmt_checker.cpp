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
        // 进入新作用域
        symTable.enterScope();
        
        bool res = true;
        
        // 检查函数是否已在函数声明表中声明
        if (funcDecls.find(node.entry) != funcDecls.end())
        {
            errors.push_back("Error: Function " + node.entry->getName() + " already declared at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        // 将函数添加到函数声明表
        funcDecls[node.entry] = &node;

        // 设置当前函数的返回类型
        curFuncRetType = voidType;
        funcHasReturn = false;

        // 检查形参
        if (node.params)
        {
            for (auto& param : *node.params)
            {
                if (!apply(*this, *param))
                    res = false;
            }
        }

        // 检查函数体
        if (node.body && !apply(*this, *node.body))
            res = false;

        // 检查是否存在 return 语句
        if (!funcHasReturn && curFuncRetType != voidType && node.entry!=Sym::Entry::getEntry("main"))
        {
            errors.push_back("Error: Function " + node.entry->getName() + " must have a return statement.");
            res = false;
        }

        // 检查返回类型是否匹配
        if(curFuncRetType !=node.retType && node.entry!=Sym::Entry::getEntry("main"))
        {
            errors.push_back("Error: Function " + node.entry->getName() + " return type mismatch.");
            res = false;
        }
        
        // 退出作用域
        symTable.exitScope();
        return res;
    }

    bool ASTChecker::visit(VarDeclStmt& node)
    {
        // 检查变量声明语句
        if (!apply(*this, *node.decl)){
            errors.push_back("Error: Invalid variable declaration at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(BlockStmt& node)
    {
        // 进入新作用域
        symTable.enterScope();

        bool res = true;
        // 逐条检查语句
        for (auto& stmt : *node.stmts)
        {
            if (!apply(*this, *stmt))
            {                
                res = false;
            }
        }

        // 退出作用域
        symTable.exitScope();
        return res;
    }

    bool ASTChecker::visit(ReturnStmt& node)
    {
        // 检查返回值类型是否匹配
        if (node.retExpr && !apply(*this, *node.retExpr))
            return false;

        // 设置返回标记
        funcHasReturn = true;
        curFuncRetType = node.retExpr ? node.retExpr->attr.val.value.type : voidType;
        return true;
    }

    bool ASTChecker::visit(WhileStmt& node)
    {   
        // 进入新作用域
        symTable.enterScope();

        bool res = true;
        // 检查条件表达式
        if (!apply(*this, *node.cond))
            res = false;
        
        // 检查条件表达式的返回值类型
        if (node.cond->attr.val.value.type == voidType)
        {
            errors.push_back("Error: Condition expression cannot have void type at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            res = false;
        }

        // 管理循环深度
        loopDepth++;
        if(apply(*this, *node.body))
            res = false;
        loopDepth--;

        // 退出作用域
        symTable.exitScope();
        return res;
    }

    bool ASTChecker::visit(IfStmt& node)
    {   
        // 进入新作用域
        symTable.enterScope();

        bool res = true;
        // 检查条件表达式
        if (!apply(*this, *node.cond))
            res = false;

        // 检查条件表达式的返回值类型
        if (node.cond->attr.val.value.type == voidType)
        {
            errors.push_back("Error: Condition expression cannot have void type at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            res = false;
        }

        // 检查 then 分支
        if (node.thenStmt && !apply(*this, *node.thenStmt))
            res = false;

        // 检查 else 分支（如果存在）
        if (node.elseStmt && !apply(*this, *node.elseStmt))
            res = false;

        // 退出作用域
        symTable.exitScope();
        return res;
    }

    bool ASTChecker::visit(BreakStmt& node)
    {
        (void)node;
        // 检查是否在循环内
        if (loopDepth == 0)
        {
            errors.push_back("Error: Break statement not within a loop at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
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
            errors.push_back("Error: Continue statement not within a loop at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(ForStmt& node)
    {   
        // 进入新作用域
        symTable.enterScope();

        bool res = true;
        // 检查初始化表达式
        if (node.init && !apply(*this, *node.init))
            res = false;

        // 检查条件表达式
        if (node.cond && !apply(*this, *node.cond))
            res = false;
        
        // 检查条件表达式的返回值类型
        if (node.cond->attr.val.value.type == voidType)
        {
            errors.push_back("Error: Condition expression cannot have void type at line " +
                                std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            res = false;
        }

        // 检查步进表达式
        if (node.step && !apply(*this, *node.step))
            res = false;

        // 管理循环深度
        loopDepth++;
        if(apply(*this, *node.body))
            res = false;
        loopDepth--;

        // 退出作用域
        symTable.exitScope();
        return res;
    }
}  // namespace FE::AST
