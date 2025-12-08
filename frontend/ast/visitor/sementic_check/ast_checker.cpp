#include <frontend/ast/visitor/sementic_check/ast_checker.h>
#include <debug.h>

namespace FE::AST
{
    bool ASTChecker::visit(Root& node)
    {
        // 重置符号表
        symTable.reset();
        mainExists = false;

        // 遍历所有顶层语句进行检查
        for (auto stmt : *node.getStmts())
        {
            apply(*this, *stmt);
        }

        // 确保存在 main 函数
        auto mainEntry = FE::Sym::Entry::getEntry("main");
        auto it = funcDecls.find(mainEntry);
        if (it == funcDecls.end() || it->second->retType != intType || !it->second->params->empty())
        {   
            errors.push_back("Error: Main function not found or has incorrect signature.");
            return false;
        }

        mainExists = true;
        return errors.empty();
    }

    VarValue ASTChecker::convertType(const VarValue& src, Type* targetType) {
        Type_t srcType = src.type->getBaseType();
        Type_t tgtType = targetType->getBaseType();

        if (srcType == tgtType) return src;

        VarValue result;
        result.type = targetType;

        if (tgtType == Type_t::INT) {
            if (srcType == Type_t::FLOAT) result.intValue = static_cast<int>(src.floatValue);
            else if (srcType == Type_t::BOOL) result.intValue = src.boolValue ? 1 : 0;
            else if (srcType == Type_t::LL) result.intValue = static_cast<int>(src.llValue);
        } else if (tgtType == Type_t::FLOAT) {
            if (srcType == Type_t::INT) result.floatValue = static_cast<float>(src.intValue);
            else if (srcType == Type_t::BOOL) result.floatValue = src.boolValue ? 1.0f : 0.0f;
            else if (srcType == Type_t::LL) result.floatValue = static_cast<float>(src.llValue);
        } else if (tgtType == Type_t::BOOL) {
            if (srcType == Type_t::INT) result.boolValue = src.intValue != 0;
            else if (srcType == Type_t::FLOAT) result.boolValue = src.floatValue != 0.0f;
            else if (srcType == Type_t::LL) result.boolValue = src.llValue != 0;
        }
        return result;
    }

    bool ASTChecker::checkArrayInitializer(InitializerList& initList, Type* elemType, std::vector<VarValue>& elements, const std::vector<int>& arrayDims, size_t dimIndex, size_t& offset)    
    {   
        bool res = true;
        if (dimIndex >= arrayDims.size())
        {
            // 对于多余的嵌套层数，尝试找到可用于合法初始化的部分
            res &= checkArrayInitializer(initList, elemType, elements, arrayDims, dimIndex - 1, offset);
            return res;
        }

        size_t stride = 1; // 计算当前维度的跨度
        for (size_t i = dimIndex + 1; i < arrayDims.size(); ++i)
            stride *= arrayDims[i];

        for (auto* subInit : *(initList.init_list))
        {
            auto* subInitList = dynamic_cast<InitializerList*>(subInit);
            if (subInitList)
            {
                // 嵌套初始化
                size_t startOffset = offset;
                res &= checkArrayInitializer(*subInitList, elemType, elements, arrayDims, dimIndex + 1, offset);
                if (subInitList->init_list->empty()) {
                    // 空列表，跳过当前上下文对应的维度大小
                    offset += stride; 
                } else {
                    // 非空列表，我们需要检查是否对齐
                    size_t endOffset = startOffset + stride;
                    if (offset < endOffset) {
                        offset = endOffset;
                    }
                }
            }
            else
            {   
                // 展平初始化
                if (offset >= elements.size())
                {
                    errors.push_back("Error: Too many initializers for array at line " +
                                    std::to_string(subInit->line_num) + ", column " +
                                    std::to_string(subInit->col_num) + ".");
                    return false;
                }
                VarValue converted = convertType(subInit->attr.val.value, elemType);                
                elements[offset] = converted;
                offset++;
            }
        }
       
        return res;
    }

    void ASTChecker::libFuncRegister()
    {
        // 示例实现：注册 SysY 标准库函数到 funcDecls 中
        // 这样在语义检查时可以识别并检查对库函数的调用
        // 包括：getint, getch, getarray, getfloat, getfarray,
        //      putint, putch, putarray, putfloat, putfarray,
        //      _sysy_starttime, _sysy_stoptime
        using SymEnt = FE::Sym::Entry;

        // int getint(), getch(), getarray(int a[]);
        static SymEnt* getint   = SymEnt::getEntry("getint");
        static SymEnt* getch    = SymEnt::getEntry("getch");
        static SymEnt* getarray = SymEnt::getEntry("getarray");

        // float getfloat();
        static SymEnt* getfloat = SymEnt::getEntry("getfloat");

        // int getfarray(float a[]);
        static SymEnt* getfarray = SymEnt::getEntry("getfarray");

        // void putint(int a), putch(int a), putarray(int n, int a[]);
        static SymEnt* putint   = SymEnt::getEntry("putint");
        static SymEnt* putch    = SymEnt::getEntry("putch");
        static SymEnt* putarray = SymEnt::getEntry("putarray");

        // void putfloat(float a);
        static SymEnt* putfloat = SymEnt::getEntry("putfloat");

        // void putfarray(int n, float a[]);
        static SymEnt* putfarray = SymEnt::getEntry("putfarray");

        // void starttime(), stoptime();
        static SymEnt* _sysy_starttime = SymEnt::getEntry("_sysy_starttime");
        static SymEnt* _sysy_stoptime  = SymEnt::getEntry("_sysy_stoptime");

        // int getint()
        funcDecls[getint] = new FuncDeclStmt(intType, getint, nullptr);

        // int getch()
        funcDecls[getch] = new FuncDeclStmt(intType, getch, nullptr);

        // int getarray(int a[])
        auto getarray_params = new std::vector<ParamDeclarator*>();
        auto getarray_param  = new ParamDeclarator(TypeFactory::getPtrType(intType), SymEnt::getEntry("a"));
        getarray_param->attr.val.value.type = TypeFactory::getPtrType(intType);
        getarray_params->push_back(getarray_param);
        funcDecls[getarray] = new FuncDeclStmt(intType, getarray, getarray_params);

        // float getfloat()
        funcDecls[getfloat] = new FuncDeclStmt(floatType, getfloat, nullptr);

        // int getfarray(float a[])
        auto getfarray_params = new std::vector<ParamDeclarator*>();
        auto getfarray_param  = new ParamDeclarator(TypeFactory::getPtrType(floatType), SymEnt::getEntry("a"));
        getfarray_param->attr.val.value.type = TypeFactory::getPtrType(floatType);
        getfarray_params->push_back(getfarray_param);
        funcDecls[getfarray] = new FuncDeclStmt(intType, getfarray, getfarray_params);

        // void putint(int a)
        auto putint_params                = new std::vector<ParamDeclarator*>();
        auto putint_param                 = new ParamDeclarator(intType, SymEnt::getEntry("a"));
        putint_param->attr.val.value.type = intType;
        putint_params->push_back(putint_param);
        funcDecls[putint] = new FuncDeclStmt(voidType, putint, putint_params);

        // void putch(int a)
        auto putch_params                = new std::vector<ParamDeclarator*>();
        auto putch_param                 = new ParamDeclarator(intType, SymEnt::getEntry("a"));
        putch_param->attr.val.value.type = intType;
        putch_params->push_back(putch_param);
        funcDecls[putch] = new FuncDeclStmt(voidType, putch, putch_params);

        // void putarray(int n, int a[])
        auto putarray_params                 = new std::vector<ParamDeclarator*>();
        auto putarray_param1                 = new ParamDeclarator(intType, SymEnt::getEntry("n"));
        putarray_param1->attr.val.value.type = intType;
        auto putarray_param2 = new ParamDeclarator(TypeFactory::getPtrType(intType), SymEnt::getEntry("a"));
        putarray_param2->attr.val.value.type = TypeFactory::getPtrType(intType);
        putarray_params->push_back(putarray_param1);
        putarray_params->push_back(putarray_param2);
        funcDecls[putarray] = new FuncDeclStmt(voidType, putarray, putarray_params);

        // void putfloat(float a)
        auto putfloat_params                = new std::vector<ParamDeclarator*>();
        auto putfloat_param                 = new ParamDeclarator(floatType, SymEnt::getEntry("a"));
        putfloat_param->attr.val.value.type = floatType;
        putfloat_params->push_back(putfloat_param);
        funcDecls[putfloat] = new FuncDeclStmt(voidType, putfloat, putfloat_params);

        // void putfarray(int n, float a[])
        auto putfarray_params                 = new std::vector<ParamDeclarator*>();
        auto putfarray_param1                 = new ParamDeclarator(intType, SymEnt::getEntry("n"));
        putfarray_param1->attr.val.value.type = intType;
        auto putfarray_param2 = new ParamDeclarator(TypeFactory::getPtrType(floatType), SymEnt::getEntry("a"));
        putfarray_param2->attr.val.value.type = TypeFactory::getPtrType(floatType);
        putfarray_params->push_back(putfarray_param1);
        putfarray_params->push_back(putfarray_param2);
        funcDecls[putfarray] = new FuncDeclStmt(voidType, putfarray, putfarray_params);

        // void _sysy_starttime(int lineno)
        auto starttime_params                = new std::vector<ParamDeclarator*>();
        auto starttime_param                 = new ParamDeclarator(intType, SymEnt::getEntry("lineno"));
        starttime_param->attr.val.value.type = intType;
        starttime_params->push_back(starttime_param);
        funcDecls[_sysy_starttime] = new FuncDeclStmt(voidType, _sysy_starttime, starttime_params);

        // void _sysy_stoptime(int lineno)
        auto stoptime_params                = new std::vector<ParamDeclarator*>();
        auto stoptime_param                 = new ParamDeclarator(intType, SymEnt::getEntry("lineno"));
        stoptime_param->attr.val.value.type = intType;
        stoptime_params->push_back(stoptime_param);
        funcDecls[_sysy_stoptime] = new FuncDeclStmt(voidType, _sysy_stoptime, stoptime_params);
    }
}  // namespace FE::AST
