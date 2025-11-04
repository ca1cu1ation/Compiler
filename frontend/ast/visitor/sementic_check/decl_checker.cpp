#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::visit(Initializer& node)
    {
        // 示例实现：单个初始化器的语义检查
        // 1) 访问初始化值表达式
        // 2) 将子表达式的属性拷贝到当前节点
        ASSERT(node.init_val && "Null initializer value");
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
        ASSERT(node.lval && "Null lval in VarDeclarator");

        // 访问左值表达式
        bool res = apply(*this, *node.lval);

        // 同步属性
        node.attr = node.lval->attr;

        // 处理初始化器（如果有）
        if (node.init)
        {
            res &= apply(*this, *node.init);
            // 检查初始化器类型是否匹配
            ASSERT(node.attr.op == node.init->attr.op && "Type mismatch in initializer");
        }

        return res;
    }

    bool ASTChecker::visit(ParamDeclarator& node)
    {
        // 实现函数形参的语义检查
        ASSERT(node.type && "Null type in ParamDeclarator");

        // 检查形参重定义
        if (symTable.getSymbol(node.entry))
        {
            errors.push_back("Redefinition of parameter");
            return false;
        }

        // 将形参加入符号表        
        VarAttr varAttr;
        varAttr.type = node.type;
        varAttr.isConstDecl = false; // 假设形参不是 const 声明
        symTable.addSymbol(node.entry, varAttr);

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
                errors.push_back("Invalid left value in variable declaration");
                res = false;
                continue;
            }

            // 获取 entry
            Entry* entry = leftValExpr->entry;

            // 检查重定义
            if (symTable.getSymbol(entry))
            {
                errors.push_back("Redefinition of variable");
                res = false;
                continue;
            }

            // 处理初始化器
            if (decl->init)
            {
                res &= apply(*this, *decl->init);
                ASSERT(decl->lval->attr.op == decl->init->attr.op && "Type mismatch in initializer");
            }

            // 将符号加入符号表
            VarAttr varAttr;
            varAttr.type = node.type;
            varAttr.isConstDecl = node.isConstDecl;

            symTable.addSymbol(entry, varAttr);
        }

        return res;
    }
}  // namespace FE::AST
