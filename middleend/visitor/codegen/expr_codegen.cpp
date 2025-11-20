#include <middleend/visitor/codegen/ast_codegen.h>

namespace ME
{
    void ASTCodeGen::visit(FE::AST::LeftValExpr& node, Module* m)
    {
        // 查找变量位置（全局或局部），处理数组下标/GEP，必要时发出 load
        FE::Sym::Entry* entry = node.entry;

        // 首先判断是否为全局变量
        auto git = glbSymbols.find(entry);
        Operand* basePtr = nullptr;
        DataType baseType = DataType::UNK;

        if (git != glbSymbols.end())
        {
            // 全局变量使用 GlobalOperand
            basePtr = getGlobalOperand(entry->getName());
            baseType = convert(git->second.type);
        }
        else
        {
            // 局部变量：name2reg 存储了指针寄存器
            size_t reg = name2reg.getReg(entry);
            if (reg == static_cast<size_t>(-1)) ERROR("Unknown local variable");
            basePtr = getRegOperand(reg);
            auto it = reg2attr.find(reg);
            if (it != reg2attr.end()) baseType = convert(it->second.type);
        }

        // 处理数组下标：如果有 indices，则生成 GEP
        if (node.indices && !node.indices->empty())
        {
            std::vector<Operand*> idxOps;
            for (auto idxExpr : *node.indices)
            {
                apply(*this, *idxExpr, m);
                idxOps.push_back(getRegOperand(getMaxReg()));
            }

            size_t resReg = getNewRegId();
            auto gep = createGEP_I32Inst(baseType, basePtr, {}, idxOps, resReg);
            insert(gep);
            lval2ptr[&node] = getRegOperand(resReg);
            // 始终生成 load，以便表达式求值产生值寄存器
            size_t valReg = getNewRegId();
            auto ld = createLoadInst(baseType, getRegOperand(resReg), valReg);
            insert(ld);
        }
        else
        {
            // 非数组元素，直接返回指针或加载
            lval2ptr[&node] = basePtr;
            // 始终生成 load，以便表达式求值产生值寄存器
            size_t valReg = getNewRegId();
            auto ld = createLoadInst(baseType, basePtr, valReg);
            insert(ld);
        }
    }

    void ASTCodeGen::visit(FE::AST::LiteralExpr& node, Module* m)
    {
        (void)m;

        size_t reg = getNewRegId();
        switch (node.literal.type->getBaseType())
        {
            case FE::AST::Type_t::INT:
            case FE::AST::Type_t::LL:  // treat as I32
            {
                int             val  = node.literal.getInt();
                ArithmeticInst* inst = createArithmeticI32Inst_ImmeAll(Operator::ADD, val, 0, reg);  // reg = val + 0
                insert(inst);
                break;
            }
            case FE::AST::Type_t::FLOAT:
            {
                float           val  = node.literal.getFloat();
                ArithmeticInst* inst = createArithmeticF32Inst_ImmeAll(Operator::FADD, val, 0, reg);  // reg = val + 0
                insert(inst);
                break;
            }
            default: ERROR("Unsupported literal type");
        }
    }

    void ASTCodeGen::visit(FE::AST::UnaryExpr& node, Module* m)
    {
        // 一元运算
        handleUnaryCalc(node, node.op, curBlock, m);
    }

    void ASTCodeGen::handleAssign(FE::AST::LeftValExpr& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        // 先计算右值并进行必要的类型转换（在生成左值/地址之前完成），以避免后续插入的指令改变寄存器编号
        apply(*this, rhs, m);
        size_t rhsReg = getMaxReg();

        // 确定目标类型
        DataType toType = DataType::UNK;
        auto git = glbSymbols.find(lhs.entry);
        if (git != glbSymbols.end()) toType = convert(git->second.type);
        else
        {
            size_t preg = name2reg.getReg(lhs.entry);
            if (preg != static_cast<size_t>(-1))
            {
                auto rit = reg2attr.find(preg);
                if (rit != reg2attr.end()) toType = convert(rit->second.type);
            }
        }

        DataType fromType = convert(rhs.attr.val.value.type);
        // 类型转换：在生成左值地址之前插入转换指令
        auto convs = createTypeConvertInst(fromType, toType, rhsReg);
        for (auto& ci : convs) insert(ci);
        if (!convs.empty()) rhsReg = getMaxReg();

        // 获取左值地址（visit LeftValExpr 会将地址放到 lval2ptr）
        apply(*this, lhs, m);
        Operand* ptr = nullptr;
        auto it = lval2ptr.find(&lhs);
        if (it != lval2ptr.end()) ptr = it->second;
        else ERROR("LeftVal address not found");

        // 发出 store
        insert(createStoreInst(toType, rhsReg, ptr));
    }
    void ASTCodeGen::handleLogicalAnd(
        FE::AST::BinaryExpr& node, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        (void)node;
        // 短路与: if lhs then val=rhs else val=0
        size_t rhsLabel, falseLabel, endLabel;

        // create blocks
        createBlock();
        rhsLabel = getMaxLabel() - 1;
        createBlock();
        falseLabel = getMaxLabel() - 1;
        createBlock();
        endLabel = getMaxLabel() - 1;

        // 评估 lhs
        apply(*this, lhs, m);
        size_t lhsReg = getMaxReg();
        // make branch
        insert(createBranchInst(lhsReg, rhsLabel, falseLabel));

        // rhs block
        enterBlock(rhsLabel);
        apply(*this, rhs, m);
        size_t rhsReg = getMaxReg();
        // convert rhs to i1 if needed
        size_t rhsBool = getNewRegId();
        IcmpInst* icmp = createIcmpInst_ImmeRight(ICmpOp::NE, rhsReg, 0, rhsBool);
        insert(icmp);
        insert(createBranchInst(endLabel));

        // false block
        enterBlock(falseLabel);
        // result false -> 0 (handled in phi incoming)
        insert(createBranchInst(endLabel));

        // end block: phi
        enterBlock(endLabel);
        size_t dest = getNewRegId();
        PhiInst* phi = new PhiInst(DataType::I32, getRegOperand(dest));
        // incoming from rhsB is rhsBool, from falseB is 0
        phi->addIncoming(getRegOperand(rhsBool), getLabelOperand(rhsLabel));
        phi->addIncoming(getImmeI32Operand(0), getLabelOperand(falseLabel));
        insert(phi);
    }
    void ASTCodeGen::handleLogicalOr(
        FE::AST::BinaryExpr& node, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        (void)node;
        // 短路或: if lhs then val=1 else val=rhs
        size_t rhsLabel, trueLabel, endLabel;

        createBlock();
        trueLabel = getMaxLabel() - 1;
        createBlock();
        rhsLabel = getMaxLabel() - 1;
        createBlock();
        endLabel = getMaxLabel() - 1;

        apply(*this, lhs, m);
        size_t lhsReg = getMaxReg();
        insert(createBranchInst(lhsReg, trueLabel, rhsLabel));

        // true block
        enterBlock(trueLabel);
        insert(createBranchInst(endLabel));

        // rhs block
        enterBlock(rhsLabel);
        apply(*this, rhs, m);
        size_t rhsReg = getMaxReg();
        size_t rhsBool = getNewRegId();
        IcmpInst* icmp = createIcmpInst_ImmeRight(ICmpOp::NE, rhsReg, 0, rhsBool);
        insert(icmp);
        insert(createBranchInst(endLabel));

        // end block: phi
        enterBlock(endLabel);
        size_t dest = getNewRegId();
        PhiInst* phi = new PhiInst(DataType::I32, getRegOperand(dest));
        phi->addIncoming(getImmeI32Operand(1), getLabelOperand(trueLabel));
        phi->addIncoming(getRegOperand(rhsBool), getLabelOperand(rhsLabel));
        insert(phi);
    }
    void ASTCodeGen::visit(FE::AST::BinaryExpr& node, Module* m)
    {
        // 赋值
        if (node.op == FE::AST::Operator::ASSIGN)
        {
            // lhs 必须是 LeftValExpr
            FE::AST::LeftValExpr* lval = dynamic_cast<FE::AST::LeftValExpr*>(node.lhs);
            ASSERT(lval && "LHS of assign must be LeftValExpr");
            handleAssign(*lval, *node.rhs, m);
            return;
        }

        // 逻辑短路
        if (node.op == FE::AST::Operator::AND)
        {
            handleLogicalAnd(node, *node.lhs, *node.rhs, m);
            return;
        }
        if (node.op == FE::AST::Operator::OR)
        {
            handleLogicalOr(node, *node.lhs, *node.rhs, m);
            return;
        }

        // 其他二元算术/比较
        handleBinaryCalc(*node.lhs, *node.rhs, node.op, curBlock, m);
    }

    void ASTCodeGen::visit(FE::AST::CallExpr& node, Module* m)
    {
        // 生成实参
        CallInst::argList args;
        if (node.args)
        {
            for (auto a : *node.args)
            {
                apply(*this, *a, m);
                size_t reg = getMaxReg();
                DataType at = convert(a->attr.val.value.type);
                args.emplace_back(at, getRegOperand(reg));
            }
        }

        // 函数名
        std::string fname = node.func->getName();
        // 获取被调用函数的返回类型（若在 funcDecls 中）
        DataType retType = DataType::VOID;
        for (auto& pd : funcDecls)
        {
            if (pd.first->getName() == fname)
            {
                retType = convert(pd.second->retType);
                break;
            }
        }

        if (retType != DataType::VOID)
        {
            size_t res = getNewRegId();
            insert(createCallInst(retType, fname, args, res));
        }
        else
        {
            insert(createCallInst(retType, fname, args));
        }
    }

    void ASTCodeGen::visit(FE::AST::CommaExpr& node, Module* m)
    {
        for (auto e : *node.exprs)
        {
            apply(*this, *e, m);
        }
    }

}  // namespace ME

    