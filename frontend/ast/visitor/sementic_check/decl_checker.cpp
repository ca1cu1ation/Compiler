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
        bool res = true;

        // 检查 decl->lval 是否为 LeftValExpr 类型
        auto* leftValExpr = dynamic_cast<LeftValExpr*>(node.lval);
        if (!leftValExpr)
        {
            errors.push_back("Error: Invalid left value in variable declaration at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            res = false;
        }
        leftValExpr->isDeclaration = true;

        // 获取 entry
        Entry* entry = leftValExpr->entry;
        // 尝试将符号加入符号表
        VarAttr varAttr;
        varAttr.type = node.attr.val.value.type;
        varAttr.isConstDecl = node.attr.val.isConstexpr;
        varAttr.isInitialized = (node.init != nullptr);  // 如果有初始化器，则标记为已初始化
        
        try
        {
            symTable.addSymbol(entry, varAttr);
        }
        catch (const std::runtime_error& e)
        {
            errors.push_back("Error: " + std::string(e.what()) + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            res = false;
        }

        // 根据作用域添加到全局符号表
        if (symTable.isGlobalScope())
        {
            glbSymbols[entry] = varAttr;
        }

        // 访问 lval 表达式以设置其属性
        res &= apply(*this, *node.lval);
        
        // 获取符号表项修改属性
        auto sym = symTable.getSymbol(entry);

        // 处理初始化器（如果有）
        if (node.init)
        {
            res &= apply(*this, *node.init);
            // 如果是数组初始化器
            auto* initList = dynamic_cast<InitializerList*>(node.init);
            if (initList)
                {
                    size_t totalCount = 0;
                    std::vector<VarValue> elements;
                    if (!checkArrayInitializer(*initList, elements, sym->arrayDims, 0, totalCount))
                    {
                        errors.push_back("Error: Invalid initializer structure for array at line " +
                                        std::to_string(node.init->line_num) + ", column " +
                                        std::to_string(node.init->col_num) + ".");
                        res = false;
                    }
                }
            else
            {
                // 非数组初始化器
                // 检查类型匹配
                if (node.attr.val.value.type != node.init->attr.val.value.type)
                {
                    errors.push_back("Error: Type mismatch in initializer " + node.attr.val.value.type->toString() + " vs " + node.init->attr.val.value.type->toString() + " at line " + std::to_string(node.init->line_num) + ", column " + std::to_string(node.init->col_num) + ".");
                    res = false;
                }

                // 将初始化器值写入符号表
                sym->initList.push_back(node.init->attr.val.value);
                res = true;
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

            // 设置变量类型
            decl->attr.val.value.type = node.type;
            decl->attr.val.isConstexpr = node.isConstDecl;

            // 变量声明器语义检查
            res &= apply(*this, *decl);
        }

        return res;
    }

    bool ASTChecker::checkArrayInitializer(InitializerList& initList, std::vector<VarValue>& elements, const std::vector<int>& arrayDims, size_t dimIndex, size_t& totalCount)    
    {
        if (dimIndex >= arrayDims.size())
        {
            // 超过数组维度，报错
            errors.push_back("Error: Too many initializers for array at dimension " + std::to_string(dimIndex) + ".");
            return false;
        }

        size_t totalSize = 1;
        for (int dim : arrayDims)
        {
            totalSize *= dim;
        }

        bool res = true;

        for (auto* subInit : *(initList.init_list))
        {
            auto* subInitList = dynamic_cast<InitializerList*>(subInit);
            if (subInitList)
            {
                // 嵌套初始化
                res &= checkArrayInitializer(*subInitList, elements, arrayDims, dimIndex + 1, totalCount);
            }
            else
            {   
                // 展平初始化
                totalCount++;
                if (totalCount > totalSize)
                {
                    errors.push_back("Error: Too many initializers for array at line " +
                                    std::to_string(subInit->line_num) + ", column " +
                                    std::to_string(subInit->col_num) + ".");
                    return false;
                }                
                elements.push_back(subInit->attr.val.value);
            }
        }
       
        // 默认初始化未填充的元素
        size_t expectedSize = arrayDims[dimIndex];
        while (elements.size() < expectedSize)
        {
            elements.push_back(VarValue()); // 默认初始化为 0
            totalCount++;
        }

        return res;
    }
}  // namespace FE::AST
