#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::visit(Initializer& node)
    {
        // 示例实现：单个初始化器的语义检查
        // 1) 访问初始化值表达式
        // 2) 将子表达式的属性拷贝到当前节点
        if (!node.init_val)
        {
            errors.push_back("Error: Null initializer value at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }
        bool res  = apply(*this, *node.init_val);
        node.attr = node.init_val->attr;
        return res;
    }

    bool ASTChecker::visit(InitializerList& node)
    {
        // 示例实现：初始化器列表的语义检查
        // 遍历列表中的每个初始化器并逐个访问
        if (!node.init_list) return true;
        bool res = true;
        for (auto* init : *(node.init_list))
        {
            if (!init) continue;
            res &= apply(*this, *init);
        }
        return res;
    }

    bool ASTChecker::visit(VarDeclarator& node)
    {
        // 实现变量声明器的语义检查
        if (!node.lval)
        {
            errors.push_back("Error: Null lval in VarDeclarator at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        bool res = true;
        // 同步属性
        node.attr = node.lval->attr;

        // 处理初始化器（如果有）
        if (node.init)
        {
            res &= apply(*this, *node.init);
            if (node.attr.op != node.init->attr.op)
            {
                errors.push_back("Error: Type mismatch in initializer at line " + std::to_string(node.init->line_num) + ", column " + std::to_string(node.init->col_num) + ".");
                res = false;
            }
        }

        return res;
    }

    bool ASTChecker::visit(ParamDeclarator& node)
    {
        // 实现函数形参的语义检查
        if(!node.type){
            errors.push_back("Error: Null type in ParamDeclarator at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        // 尝试将形参加入符号表                 
        VarAttr varAttr;
        varAttr.type = node.type;
        varAttr.isConstDecl = false; // 假设形参不是 const 声明
        try
        {
            symTable.addSymbol(node.entry, varAttr);
        }
        catch (const std::runtime_error& e)
        {
            errors.push_back("Error: " + std::string(e.what()) + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        return true;
    }

    bool ASTChecker::visit(VarDeclaration& node)
    {
        // 实现变量声明的语义检查
        bool res = true;

        for (auto* decl : *(node.decls))
        {
            if (!decl) continue;

            // 检查 decl->lval 是否为 LeftValExpr 类型
            auto* leftValExpr = dynamic_cast<LeftValExpr*>(decl->lval);
            if (!leftValExpr)
            {
                errors.push_back("Error: Invalid left value in variable declaration at line " + std::to_string(decl->line_num) + ", column " + std::to_string(decl->col_num) + ".");
                res = false;
                continue;
            }

            // 获取 entry
            Entry* entry = leftValExpr->entry;

            // 变量声明器语义检查
            res &= apply(*this, *decl);

            // 尝试将符号加入符号表
            VarAttr varAttr;
            varAttr.type = node.type;
            varAttr.isConstDecl = node.isConstDecl;
            varAttr.isInitialized = (decl->init != nullptr);  // 如果有初始化器，则标记为已初始化
            try
            {
                symTable.addSymbol(entry, varAttr);
            }
            catch (const std::runtime_error& e)
            {
                errors.push_back("Error: " + std::string(e.what()) + " at line " + std::to_string(decl->line_num) + ", column " + std::to_string(decl->col_num) + ".");
                res = false;
                continue;
            }

            // 根据作用域添加到全局符号表
            if (symTable.isGlobalScope())
            {
                glbSymbols[entry] = varAttr;
            }
        }

        return res;
    }
}  // namespace FE::AST
