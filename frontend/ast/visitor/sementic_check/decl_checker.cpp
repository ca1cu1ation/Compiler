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
                    size_t totalSize = 1;
                    if(sym->arrayDims.empty())
                    {
                        errors.push_back("Error: Initializer list provided for non-array variable " + entry->getName() + " at line " +
                                        std::to_string(node.init->line_num) + ", column " +
                                        std::to_string(node.init->col_num) + ".");
                        return false;
                    }
                    for (int dim : sym->arrayDims)
                    {
                        totalSize *= dim;
                    }
                    Type* elemType = sym->type;
                    VarValue defaultValue;
                    defaultValue.type = elemType;
                    // 可根据类型设置默认值
                    if (elemType->getBaseType() == Type_t::INT) defaultValue.intValue = 0;
                    else if (elemType->getBaseType() == Type_t::FLOAT) defaultValue.floatValue = 0.0f;
                    else if (elemType->getBaseType() == Type_t::BOOL) defaultValue.boolValue = false;
                    std::vector<VarValue> elements(totalSize, defaultValue);
                    if (!checkArrayInitializer(*initList, elemType, elements, sym->arrayDims, 0, 0))
                    {
                        errors.push_back("Error: Invalid initializer structure for array at line " +
                                        std::to_string(node.init->line_num) + ", column " +
                                        std::to_string(node.init->col_num) + ".");
                        res = false;
                    }
                    sym->initList = elements;
                }
            else
            {
                // 非数组初始化器
                // 检查类型匹配
                VarValue converted = convertType(node.init->attr.val.value, node.attr.val.value.type);
                if (node.attr.val.value.type != converted.type)
                {
                    errors.push_back("Error: Type mismatch in initializer " + node.attr.val.value.type->toString() + " vs " + node.init->attr.val.value.type->toString() + " at line " + std::to_string(node.init->line_num) + ", column " + std::to_string(node.init->col_num) + ".");
                    res = false;
                }

                // 将初始化器值写入符号表
                sym->initList.push_back(converted);
                res = true;
            }
        }

        return res;
    }

    bool ASTChecker::visit(ParamDeclarator& node)
    {   
        bool res = true;
        // 实现函数形参的语义检查
        if(!node.type){
            errors.push_back("Error: Null type in ParamDeclarator at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            res = false;
        }

        // 尝试将形参加入符号表                 
        VarAttr varAttr;
        varAttr.type = node.type;
        varAttr.isConstDecl = false; // 假设形参不是 const 声明
        // 处理数组维度
        if(node.dims != nullptr){
            for (size_t i = 0; i < node.dims->size(); ++i)
            {                        
                // 访问维度表达式以确保其语义正确
                if (!apply(*this, *(*node.dims)[i]))
                {
                    errors.push_back("Error: Invalid dimension expression in ParamDeclarator at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                    res = false;
                }
                // 形参数组维度应为整数常量
                if ((*node.dims)[i]->attr.val.value.type != intType || !(*node.dims)[i]->attr.val.isConstexpr)
                {
                    errors.push_back("Error: Array dimension must be a constant integer in ParamDeclarator at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                    res = false;
                }
                if (i !=0 && (*node.dims)[i]->attr.val.value.getInt() <= 0){
                    errors.push_back("Error: Array dimension must be positive in ParamDeclarator at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
                    res = false;
                }
                varAttr.arrayDims.push_back((*node.dims)[i]->attr.val.value.getInt());
            }
        }

        try
        {
            symTable.addSymbol(node.entry, varAttr);
        }
        catch (const std::runtime_error& e)
        {
            errors.push_back("Error: " + std::string(e.what()) + " at line " + std::to_string(node.line_num) + ", column " + std::to_string(node.col_num) + ".");
            return false;
        }

        return res;
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
}  // namespace FE::AST
