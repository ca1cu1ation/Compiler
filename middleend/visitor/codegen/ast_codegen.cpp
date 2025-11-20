#include <middleend/visitor/codegen/ast_codegen.h>
#include <debug.h>

namespace ME
{
    void ASTCodeGen::libFuncRegister(Module* m)
    {
        auto& decls = m->funcDecls;

        // int getint();
        decls.emplace_back(new FuncDeclInst(DataType::I32, "getint"));

        // int getch();
        decls.emplace_back(new FuncDeclInst(DataType::I32, "getch"));

        // int getarray(int a[]);
        decls.emplace_back(new FuncDeclInst(DataType::I32, "getarray", {DataType::PTR}));

        // float getfloat();
        decls.emplace_back(new FuncDeclInst(DataType::F32, "getfloat"));

        // int getfarray(float a[]);
        decls.emplace_back(new FuncDeclInst(DataType::I32, "getfarray", {DataType::PTR}));

        // void putint(int a);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "putint", {DataType::I32}));

        // void putch(int a);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "putch", {DataType::I32}));

        // void putarray(int n, int a[]);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "putarray", {DataType::I32, DataType::PTR}));

        // void putfloat(float a);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "putfloat", {DataType::F32}));

        // void putfarray(int n, float a[]);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "putfarray", {DataType::I32, DataType::PTR}));

        // void starttime(int lineno);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "_sysy_starttime", {DataType::I32}));

        // void stoptime(int lineno);
        decls.emplace_back(new FuncDeclInst(DataType::VOID, "_sysy_stoptime", {DataType::I32}));

        // llvm memset
        decls.emplace_back(new FuncDeclInst(
            DataType::VOID, "llvm.memset.p0.i32", {DataType::PTR, DataType::I8, DataType::I32, DataType::I1}));
    }

    void ASTCodeGen::handleGlobalVarDecl(FE::AST::VarDeclStmt* decls, Module* m)
    {
        (void)decls;
        // 使用语义检查器提供的 glbSymbols 注册全局变量
        for (auto& [entry, attr] : glbSymbols)
        {
            DataType dt = convert(attr.type);
            std::string name = entry->getName();
            if (attr.arrayDims.empty())
            {
                // 标量或简单指针
                if (!attr.initList.empty())
                {
                    // 对于标量初始化，将第一个初值转换为立即数 Operand 并传入 GlbVarDeclInst
                    ME::Operand* initOp = nullptr;
                    const auto& vv     = attr.initList[0];
                    if (vv.type == FE::AST::intType || vv.type == FE::AST::llType || vv.type == FE::AST::boolType)
                    {
                        // 将 bool/ll/int 都当作 i32 立即数打印（前端已保证类型匹配或转换）
                        initOp = getImmeI32Operand(vv.getInt());
                    }
                    else if (vv.type == FE::AST::floatType)
                    {
                        initOp = getImmeF32Operand(vv.getFloat());
                    }
                    // 传入构造函数（如果 initOp 为 nullptr，将退化为 zeroinitializer）
                    m->globalVars.emplace_back(new GlbVarDeclInst(dt, name, initOp));
                }
                else
                {
                    m->globalVars.emplace_back(new GlbVarDeclInst(dt, name, nullptr));
                }
            }
            else
            {
                // 数组类型，使用 initList 构造函数
                m->globalVars.emplace_back(new GlbVarDeclInst(dt, name, attr));
            }
        }
    }

    void ASTCodeGen::visit(FE::AST::Root& node, Module* m)
    {
        // 先生成全局变量声明（从语义检查器提供的全局符号表）
        handleGlobalVarDecl(nullptr, m);

        // 生成所有函数声明（来自语义检查器提供的 funcDecls）
        for (auto& [entry, fdecl] : funcDecls)
        {
            DataType rt = convert(fdecl->retType);
            std::vector<DataType> argTypes;
            if (fdecl->params)
            {
                for (auto p : *fdecl->params)
                {
                    DataType at = convert(p->type);
                    if (p->dims) at = DataType::PTR; // 数组作为指针传参
                    argTypes.push_back(at);
                }
            }
            m->funcDecls.emplace_back(new FuncDeclInst(rt, entry->getName(), argTypes));
        }

        // 生成函数定义（如果有函数体）
        for (auto stmt : *node.getStmts())
        {
            if (!stmt) continue;
            if (!stmt->isVarDeclStmt())
            {
                apply(*this, *stmt, m);
            }
        }
    }

    LoadInst* ASTCodeGen::createLoadInst(DataType t, Operand* ptr, size_t resReg)
    {
        return new LoadInst(t, ptr, getRegOperand(resReg));
    }

    StoreInst* ASTCodeGen::createStoreInst(DataType t, size_t valReg, Operand* ptr)
    {
        return new StoreInst(t, getRegOperand(valReg), ptr);
    }
    StoreInst* ASTCodeGen::createStoreInst(DataType t, Operand* val, Operand* ptr)
    {
        return new StoreInst(t, val, ptr);
    }

    ArithmeticInst* ASTCodeGen::createArithmeticI32Inst(Operator op, size_t lhsReg, size_t rhsReg, size_t resReg)
    {
        return new ArithmeticInst(
            op, DataType::I32, getRegOperand(lhsReg), getRegOperand(rhsReg), getRegOperand(resReg));
    }
    ArithmeticInst* ASTCodeGen::createArithmeticI32Inst_ImmeLeft(Operator op, int lhsVal, size_t rhsReg, size_t resReg)
    {
        return new ArithmeticInst(
            op, DataType::I32, getImmeI32Operand(lhsVal), getRegOperand(rhsReg), getRegOperand(resReg));
    }
    ArithmeticInst* ASTCodeGen::createArithmeticI32Inst_ImmeAll(Operator op, int lhsVal, int rhsVal, size_t resReg)
    {
        return new ArithmeticInst(
            op, DataType::I32, getImmeI32Operand(lhsVal), getImmeI32Operand(rhsVal), getRegOperand(resReg));
    }
    ArithmeticInst* ASTCodeGen::createArithmeticF32Inst(Operator op, size_t lhsReg, size_t rhsReg, size_t resReg)
    {
        return new ArithmeticInst(
            op, DataType::F32, getRegOperand(lhsReg), getRegOperand(rhsReg), getRegOperand(resReg));
    }
    ArithmeticInst* ASTCodeGen::createArithmeticF32Inst_ImmeLeft(
        Operator op, float lhsVal, size_t rhsReg, size_t resReg)
    {
        return new ArithmeticInst(
            op, DataType::F32, getImmeF32Operand(lhsVal), getRegOperand(rhsReg), getRegOperand(resReg));
    }
    ArithmeticInst* ASTCodeGen::createArithmeticF32Inst_ImmeAll(Operator op, float lhsVal, float rhsVal, size_t resReg)
    {
        return new ArithmeticInst(
            op, DataType::F32, getImmeF32Operand(lhsVal), getImmeF32Operand(rhsVal), getRegOperand(resReg));
    }

    IcmpInst* ASTCodeGen::createIcmpInst(ICmpOp cond, size_t lhsReg, size_t rhsReg, size_t resReg)
    {
        return new IcmpInst(DataType::I32, cond, getRegOperand(lhsReg), getRegOperand(rhsReg), getRegOperand(resReg));
    }
    IcmpInst* ASTCodeGen::createIcmpInst_ImmeRight(ICmpOp cond, size_t lhsReg, int rhsVal, size_t resReg)
    {
        return new IcmpInst(
            DataType::I32, cond, getRegOperand(lhsReg), getImmeI32Operand(rhsVal), getRegOperand(resReg));
    }
    FcmpInst* ASTCodeGen::createFcmpInst(FCmpOp cond, size_t lhsReg, size_t rhsReg, size_t resReg)
    {
        return new FcmpInst(DataType::F32, cond, getRegOperand(lhsReg), getRegOperand(rhsReg), getRegOperand(resReg));
    }
    FcmpInst* ASTCodeGen::createFcmpInst_ImmeRight(FCmpOp cond, size_t lhsReg, float rhsVal, size_t resReg)
    {
        return new FcmpInst(
            DataType::F32, cond, getRegOperand(lhsReg), getImmeF32Operand(rhsVal), getRegOperand(resReg));
    }

    FP2SIInst* ASTCodeGen::createFP2SIInst(size_t srcReg, size_t destReg)
    {
        return new FP2SIInst(getRegOperand(srcReg), getRegOperand(destReg));
    }
    SI2FPInst* ASTCodeGen::createSI2FPInst(size_t srcReg, size_t destReg)
    {
        return new SI2FPInst(getRegOperand(srcReg), getRegOperand(destReg));
    }
    ZextInst* ASTCodeGen::createZextInst(size_t srcReg, size_t destReg, size_t srcBits, size_t destBits)
    {
        ASSERT(srcBits == 1 && destBits == 32 && "Currently only support i1 to i32 zext");
        return new ZextInst(DataType::I1, DataType::I32, getRegOperand(srcReg), getRegOperand(destReg));
    }

    GEPInst* ASTCodeGen::createGEP_I32Inst(
        DataType t, Operand* ptr, std::vector<int> dims, std::vector<Operand*> is, size_t resReg)
    {
        return new GEPInst(t, DataType::I32, ptr, getRegOperand(resReg), dims, is);
    }

    CallInst* ASTCodeGen::createCallInst(DataType t, std::string funcName, CallInst::argList args, size_t resReg)
    {
        return new CallInst(t, funcName, args, getRegOperand(resReg));
    }
    CallInst* ASTCodeGen::createCallInst(DataType t, std::string funcName, CallInst::argList args)
    {
        return new CallInst(t, funcName, args);
    }
    CallInst* ASTCodeGen::createCallInst(DataType t, std::string funcName, size_t resReg)
    {
        return new CallInst(t, funcName, getRegOperand(resReg));
    }
    CallInst* ASTCodeGen::createCallInst(DataType t, std::string funcName) { return new CallInst(t, funcName); }

    RetInst* ASTCodeGen::createRetInst() { return new RetInst(DataType::VOID); }
    RetInst* ASTCodeGen::createRetInst(DataType t, size_t retReg) { return new RetInst(t, getRegOperand(retReg)); }
    RetInst* ASTCodeGen::createRetInst(int val) { return new RetInst(DataType::I32, getImmeI32Operand(val)); }
    RetInst* ASTCodeGen::createRetInst(float val) { return new RetInst(DataType::F32, getImmeF32Operand(val)); }

    BrCondInst* ASTCodeGen::createBranchInst(size_t condReg, size_t trueTar, size_t falseTar)
    {
        return new BrCondInst(getRegOperand(condReg), getLabelOperand(trueTar), getLabelOperand(falseTar));
    }
    BrUncondInst* ASTCodeGen::createBranchInst(size_t tar) { return new BrUncondInst(getLabelOperand(tar)); }

    AllocaInst* ASTCodeGen::createAllocaInst(DataType t, size_t ptrReg)
    {
        return new AllocaInst(t, getRegOperand(ptrReg));
    }
    AllocaInst* ASTCodeGen::createAllocaInst(DataType t, size_t ptrReg, std::vector<int> dims)
    {
        return new AllocaInst(t, getRegOperand(ptrReg), dims);
    }

    std::list<Instruction*> ASTCodeGen::createTypeConvertInst(DataType from, DataType to, size_t srcReg)
    {
        if (from == to) return {};
        ASSERT((from == DataType::I1) || (from == DataType::I32) || (from == DataType::F32));
        ASSERT((to == DataType::I1) || (to == DataType::I32) || (to == DataType::F32));

        std::list<Instruction*> insts;

        switch (from)
        {
            case DataType::I1:
            {
                switch (to)
                {
                    case DataType::I32:
                    {
                        size_t    destReg = getNewRegId();
                        ZextInst* zext    = createZextInst(srcReg, destReg, 1, 32);
                        insts.push_back(zext);
                        break;
                    }
                    case DataType::F32:
                    {
                        size_t    i32Reg = getNewRegId();
                        ZextInst* zext   = createZextInst(srcReg, i32Reg, 1, 32);
                        insts.push_back(zext);
                        size_t f32Reg = getNewRegId();
                        insts.push_back(createSI2FPInst(i32Reg, f32Reg));
                        break;
                    }
                    default: ERROR("Type conversion not supported");
                }
                break;
            }
            case DataType::I32:
            {
                switch (to)
                {
                    case DataType::I1:
                    {
                        size_t    destReg = getNewRegId();
                        IcmpInst* icmp    = createIcmpInst_ImmeRight(ICmpOp::NE, srcReg, 0, destReg);
                        insts.push_back(icmp);
                        break;
                    }
                    case DataType::F32:
                    {
                        size_t destReg = getNewRegId();
                        insts.push_back(createSI2FPInst(srcReg, destReg));
                        break;
                    }
                    default: ERROR("Type conversion not supported");
                }
                break;
            }
            case DataType::F32:
            {
                switch (to)
                {
                    case DataType::I1:
                    {
                        size_t    destReg = getNewRegId();
                        FcmpInst* fcmp    = createFcmpInst_ImmeRight(FCmpOp::ONE, srcReg, 0.0f, destReg);
                        insts.push_back(fcmp);
                        break;
                    }
                    case DataType::I32:
                    {
                        size_t destReg = getNewRegId();
                        insts.push_back(createFP2SIInst(srcReg, destReg));
                        break;
                    }
                    default: ERROR("Type conversion not supported");
                }
                break;
            }
            default: ERROR("Type conversion not supported");
        }

        return insts;
    }
}  // namespace ME

    