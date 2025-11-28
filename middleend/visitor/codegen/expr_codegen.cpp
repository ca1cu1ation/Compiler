#include <middleend/visitor/codegen/ast_codegen.h>

namespace ME
{
    void ASTCodeGen::visit(FE::AST::LeftValExpr& node, Module* m)
    {
        // 查找变量位置（全局或局部），处理数组下标/GEP，必要时发出 load
        FE::Sym::Entry* entry = node.entry;

        // 优先判断局部变量（支持遮蔽），否则使用全局变量
        Operand* basePtr = nullptr;
        DataType baseType = DataType::UNK;
        size_t reg = name2reg.getReg(entry);
        if (reg != static_cast<size_t>(-1))
        {
            // 局部变量：name2reg 存储了指针寄存器
            basePtr = getRegOperand(reg);
            auto it = reg2attr.find(reg);
            if (it != reg2attr.end()) baseType = convert(it->second.type);
        }
        else
        {
            // 全局变量使用 GlobalOperand
            auto git = glbSymbols.find(entry);
            if (git != glbSymbols.end())
            {
                basePtr = getGlobalOperand(entry->getName());
                baseType = convert(git->second.type);
            }
            else
            {
                ERROR("Unknown variable");
            }
        }

        // 处理数组下标：如果有 indices，则生成 GEP
        if (node.indices && !node.indices->empty())
        {
            std::vector<Operand*> idxOps;
            // 第一个索引是基址偏移，通常为0
            idxOps.push_back(getImmeI32Operand(0));
            for (auto idxExpr : *node.indices)
            {
                apply(*this, *idxExpr, m);
                size_t idxReg = getMaxReg();
                DataType idxType = convert(idxExpr->attr.val.value.type);
                if (idxType != DataType::I32) {
                    auto convs = createTypeConvertInst(idxType, DataType::I32, idxReg);
                    for (auto& inst : convs) insert(inst);
                    idxReg = getMaxReg();
                }
                idxOps.push_back(getRegOperand(idxReg));
            }

            size_t resReg = getNewRegId();
            // 对于数组访问，基类型应该是元素类型
            DataType elemType = baseType;
            // 如果是多维数组，需要调整类型
            auto git = glbSymbols.find(entry);
            if (git != glbSymbols.end() && !git->second.arrayDims.empty()) {
                // 获取数组元素的基础类型
                elemType = convert(git->second.type);
                // 如果是数组类型，转换为对应的指针或元素类型
                // 这里简化处理，假设都是 I32 类型
                if (elemType == DataType::PTR) {
                    elemType = DataType::I32;
                }
            } else if (reg != static_cast<size_t>(-1)) {
                auto rit = reg2attr.find(reg);
                if (rit != reg2attr.end() && !rit->second.arrayDims.empty()) {
                    elemType = convert(rit->second.type);
                    if (elemType == DataType::PTR) {
                        elemType = DataType::I32;
                    }
                }
            }
            auto gep = createGEP_I32Inst(elemType, basePtr, {}, idxOps, resReg);
            insert(gep);
            lval2ptr[&node] = getRegOperand(resReg);
            
            // 始终生成 load，以便表达式求值产生值寄存器
            size_t valReg = getNewRegId();
            auto ld = createLoadInst(elemType, getRegOperand(resReg), valReg);
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
        // 根据字面量的基础类型进行处理
        FE::AST::Type* literalType = node.literal.type;
        
        // 使用 getBaseType() 方法获取基础类型枚举值
        FE::AST::Type_t baseType = literalType->getBaseType();
        switch (baseType)
        {
            case FE::AST::Type_t::INT:
            case FE::AST::Type_t::LL:  // treat as I32
            case FE::AST::Type_t::BOOL:
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
            default: 
                ERROR("Unsupported literal type: %d", static_cast<int>(baseType));
        }
    }

    void ASTCodeGen::visit(FE::AST::UnaryExpr& node, Module* m)
    {   
        // 调用handleUnaryCalc递归处理所有嵌套的一元运算
        handleUnaryCalc(*node.expr, node.op, curBlock, m);
    }

    void ASTCodeGen::handleAssign(FE::AST::LeftValExpr& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        // 先计算右值并进行必要的类型转换（在生成左值/地址之前完成），以避免后续插入的指令改变寄存器编号
        apply(*this, rhs, m);
        size_t rhsReg = getMaxReg();

        // 确定目标类型
        DataType toType = DataType::UNK;
        // 优先使用局部变量类型（若存在），否则查找全局
        size_t preg = name2reg.getReg(lhs.entry);
        if (preg != static_cast<size_t>(-1))
        {
            auto rit = reg2attr.find(preg);
            if (rit != reg2attr.end()) toType = convert(rit->second.type);
        }
        else
        {
            auto git = glbSymbols.find(lhs.entry);
            if (git != glbSymbols.end()) toType = convert(git->second.type);
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
        
        // 赋值表达式的结果是右值，需要重新加载
        size_t resultReg = getNewRegId();
        insert(createLoadInst(toType, ptr, resultReg));
    }
    void ASTCodeGen::handleLogicalAnd(
        FE::AST::BinaryExpr& node, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        (void)node;
        // 使用临时内存实现短路与：避免 PHI 相关问题
        // 评估 lhs 并归一为 i1
        apply(*this, lhs, m);
        size_t lhsReg = getMaxReg();
        DataType lhsType = convert(lhs.attr.val.value.type);
        size_t lhsBool;
        if (lhsType == DataType::I1)
            lhsBool = lhsReg;
        else
        {
            lhsBool = getNewRegId();
            IcmpInst* icmp = createIcmpInst_ImmeRight(ICmpOp::NE, lhsReg, 0, lhsBool);
            insert(icmp);
        }

        // 在当前函数中分配一个临时 i32 用于保存结果（0/1）
        size_t tmpPtr = getNewRegId();
        insert(new AllocaInst(DataType::I32, getRegOperand(tmpPtr)));

        // 创建块：rhs, false, end（在评估 lhs 后创建，避免标签错位）
        createBlock();
        size_t rhsLabel = getMaxLabel() - 1;
        createBlock();
        size_t falseLabel = getMaxLabel() - 1;
        createBlock();
        size_t endLabel = getMaxLabel() - 1;

        // 根据 lhs 分支
        insert(createBranchInst(lhsBool, rhsLabel, falseLabel));

        // rhs 分支：计算 rhs 并将其转换为 i32 存入 tmp
        enterBlock(rhsLabel);
        apply(*this, rhs, m);
        size_t rhsReg = getMaxReg();
        DataType rhsType = convert(rhs.attr.val.value.type);
        if (rhsType != DataType::I32)
        {
            auto convs = createTypeConvertInst(rhsType, DataType::I32, rhsReg);
            for (auto& c : convs) insert(c);
            rhsReg = getMaxReg();
        }
        insert(createStoreInst(DataType::I32, getRegOperand(rhsReg), getRegOperand(tmpPtr)));
        insert(createBranchInst(endLabel));

        // false 分支：写入 0
        enterBlock(falseLabel);
        insert(createStoreInst(DataType::I32, getImmeI32Operand(0), getRegOperand(tmpPtr)));
        insert(createBranchInst(endLabel));

        // end：加载 tmp 并转换为 i1 作为表达式值
        enterBlock(endLabel);
        size_t loadReg = getNewRegId();
        insert(createLoadInst(DataType::I32, getRegOperand(tmpPtr), loadReg));
        auto convs = createTypeConvertInst(DataType::I32, DataType::I1, loadReg);
        for (auto& c : convs) insert(c);
    }
    void ASTCodeGen::handleLogicalOr(
        FE::AST::BinaryExpr& node, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        (void)node;
        // 使用临时内存实现短路或：如果 lhs 为真，结果为 1；否则为 rhs
        apply(*this, lhs, m);
        size_t lhsReg = getMaxReg();
        DataType lhsType = convert(lhs.attr.val.value.type);
        size_t lhsBool;
        if (lhsType == DataType::I1)
            lhsBool = lhsReg;
        else
        {
            lhsBool = getNewRegId();
            IcmpInst* icmp = createIcmpInst_ImmeRight(ICmpOp::NE, lhsReg, 0, lhsBool);
            insert(icmp);
        }

        // 分配临时 i32
        size_t tmpPtr = getNewRegId();
        insert(new AllocaInst(DataType::I32, getRegOperand(tmpPtr)));

        // 创建块
        createBlock();
        size_t trueLabel = getMaxLabel() - 1;
        createBlock();
        size_t rhsLabel = getMaxLabel() - 1;
        createBlock();
        size_t endLabel = getMaxLabel() - 1;

        // 根据 lhs 分支
        insert(createBranchInst(lhsBool, trueLabel, rhsLabel));

        // true: 写入 1
        enterBlock(trueLabel);
        insert(createStoreInst(DataType::I32, getImmeI32Operand(1), getRegOperand(tmpPtr)));
        insert(createBranchInst(endLabel));

        // rhs: evaluate rhs, convert to i32, store
        enterBlock(rhsLabel);
        apply(*this, rhs, m);
        size_t rhsReg = getMaxReg();
        DataType rhsType = convert(rhs.attr.val.value.type);
        if (rhsType != DataType::I32)
        {
            auto convs = createTypeConvertInst(rhsType, DataType::I32, rhsReg);
            for (auto& c : convs) insert(c);
            rhsReg = getMaxReg();
        }
        insert(createStoreInst(DataType::I32, getRegOperand(rhsReg), getRegOperand(tmpPtr)));
        insert(createBranchInst(endLabel));

        // end: load tmp, convert to i1
        enterBlock(endLabel);
        size_t loadReg = getNewRegId();
        insert(createLoadInst(DataType::I32, getRegOperand(tmpPtr), loadReg));
        auto convs = createTypeConvertInst(DataType::I32, DataType::I1, loadReg);
        for (auto& c : convs) insert(c);
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
        // 函数名
        std::string fname = node.func->getName();
        
        // 获取被调用函数的返回类型和参数类型（若在 funcDecls 中）
        DataType retType = DataType::VOID;
        std::vector<DataType> targetArgTypes;
        for (auto& pd : funcDecls)
        {   
            if (pd.first->getName() == fname)
            {                   
                retType = convert(pd.second->retType);
                if(pd.second->params){
                    for (auto param : *pd.second->params) {
                        targetArgTypes.push_back(convert(param->type));                
                    }
                }
                break;
            }
        }
        
        // 生成实参并做类型转换
        CallInst::argList args;
        if (node.args) {
            size_t idx = 0;
            for (auto a : *node.args) {
                apply(*this, *a, m);
                size_t reg = getMaxReg();
                DataType at = convert(a->attr.val.value.type);
                DataType targetType = (idx < targetArgTypes.size()) ? targetArgTypes[idx] : at;
                if (at != targetType) {
                    auto convs = createTypeConvertInst(at, targetType, reg);
                    for (auto& inst : convs) insert(inst);
                    reg = getMaxReg();
                    at = targetType;
                }
                args.emplace_back(at, getRegOperand(reg));
                idx++;
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
        // 逗号表达式的结果是最后一个表达式的值
        // 不需要额外处理，因为最后一个表达式的结果已经在寄存器中
    }

}  // namespace ME

    