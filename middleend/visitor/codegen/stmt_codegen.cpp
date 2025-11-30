#include <middleend/visitor/codegen/ast_codegen.h>

namespace ME
{
    void ASTCodeGen::visit(FE::AST::ExprStmt& node, Module* m)
    {
        if (!node.expr) return;
        apply(*this, *node.expr, m);
    }

    void ASTCodeGen::visit(FE::AST::FuncDeclStmt& node, Module* m)
    {
        // 创建 FuncDefInst 和 Function
        DataType retType = convert(node.retType);
        // 先创建空的 FuncDefInst
        FuncDefInst* fdef = new FuncDefInst(retType, node.entry->getName());
        Function* func = new Function(fdef);
        m->functions.emplace_back(func);

        // 进入函数环境
        enterFunc(func);

        // 创建入口基本块（先创建入口块，再在其中生成参数的 alloca/store）
        Block* entry = createBlock();
        enterBlock(entry);
        // 为函数建立新的局部符号作用域，避免不同函数之间符号表污染
        name2reg.enterScope();

        // 为形参在函数定义中创建寄存器，并记录映射
        if (node.params)
        {
            for (auto p : *node.params)
            {
                DataType at = convert(p->type);
                if (p->dims && !p->dims->empty()) at = DataType::PTR;
                size_t preg = getNewRegId();
                // 将参数寄存器加入 FuncDefInst 的 argRegs
                fdef->argRegs.emplace_back(at, getRegOperand(preg));
                // 为参数创建本地 alloca，并把参数存入其中，这样变量访问仍使用指针
                size_t ptrReg = getNewRegId();
                // 对于数组参数（指针），我们只分配一个指针变量，而不是数组
                insert(new AllocaInst(at, getRegOperand(ptrReg)));
                insert(createStoreInst(at, preg, getRegOperand(ptrReg)));

                name2reg.addSymbol(p->entry, ptrReg);
                FE::AST::VarAttr va; 
                va.type = p->type;
                if (p->dims && !p->dims->empty()) {
                    va.isParamPtr = true;
                    // 提取维度信息
                    for (auto expr : *p->dims) {
                        if (expr && expr->attr.val.isConstexpr) {
                            va.arrayDims.push_back(expr->attr.val.getInt());
                        } else {
                            va.arrayDims.push_back(0); // 第一维通常为空或0
                        }
                    }
                }
                reg2attr[ptrReg] = va;
                if (p->dims && !p->dims->empty()) paramPtrTab[preg] = true;
            }
        }

        // 生成函数体
        if (node.body) apply(*this, *node.body, m);

        // 如果最后一个基本块没有 terminator，则补上返回指令
        if (curBlock && (curBlock->insts.empty() || !curBlock->insts.back()->isTerminator()))
        {
            if (retType == DataType::VOID) insert(createRetInst());
            else if (retType == DataType::I32) insert(createRetInst(0));
            else if (retType == DataType::F32) insert(createRetInst(0.0f));
        }

        // 清理每个基本块：若存在多个终结指令或终结指令后还有指令，移除多余部分
        for (auto& p : curFunc->blocks)
        {
            Block* b = p.second;
            bool seenTerm = false;
            std::deque<Instruction*> newInsts;
            for (auto inst : b->insts)
            {
                if (!seenTerm)
                {
                    newInsts.push_back(inst);
                    if (inst->isTerminator()) seenTerm = true;
                }
                else
                {
                    // 多余的指令（包括重复的 terminator），释放以避免内存泄漏
                    delete inst;
                }
            }
            b->insts = std::move(newInsts);
        }

        // 退出函数的局部符号作用域
        name2reg.exitScope();
        exitBlock();
        exitFunc();
    }

    void ASTCodeGen::visit(FE::AST::VarDeclStmt& node, Module* m)
    {
        // 将具体实现委托给 VarDeclaration 处理器
        apply(*this, *node.decl, m);
    }

    void ASTCodeGen::visit(FE::AST::BlockStmt& node, Module* m)
    {
        // 进入新的符号/寄存器作用域
        name2reg.enterScope();
        // 顺序生成子语句
        if (node.stmts)
        {
            for (auto s : *node.stmts)
            {
                apply(*this, *s, m);
            }
        }
        name2reg.exitScope();
    }

    void ASTCodeGen::visit(FE::AST::ReturnStmt& node, Module* m)
    {
        if (!node.retExpr)
        {
            insert(createRetInst());
            return;
        }

        apply(*this, *node.retExpr, m);
        size_t retReg = getMaxReg();
        DataType from = convert(node.retExpr->attr.val.value.type);
        DataType to = curFunc->funcDef->retType;
        auto convs = createTypeConvertInst(from, to, retReg);
        for (auto& c : convs) insert(c);
        retReg = getMaxReg();
        insert(createRetInst(to, retReg));
    }

    void ASTCodeGen::visit(FE::AST::WhileStmt& node, Module* m)
    {
        // 生成三个块：cond, body, end
        createBlock();
        size_t condL = getMaxLabel() - 1;
        createBlock();
        size_t bodyL = getMaxLabel() - 1;
        createBlock();
        size_t endL = getMaxLabel() - 1;

        // 将循环标签压栈
        pushLoop(condL, endL);

        // 跳转到 cond
        insert(createBranchInst(condL));

        // cond
        enterBlock(condL);
        apply(*this, *node.cond, m);
        size_t condReg = getMaxReg();
        DataType condType = convert(node.cond->attr.val.value.type);
        if (condType != DataType::I1) {
            auto convs = createTypeConvertInst(condType, DataType::I1, condReg);
            for (auto& inst : convs) insert(inst);
            condReg = getMaxReg();
        }
        insert(createBranchInst(condReg, bodyL, endL));

        // body
        enterBlock(bodyL);
        if (node.body) apply(*this, *node.body, m);
        // 若 body 未以 terminator 结束，则回到 cond
        if (curBlock->insts.empty() || !curBlock->insts.back()->isTerminator()) 
            insert(createBranchInst(condL));

        // end
        enterBlock(endL);

        // 弹出循环标签
        popLoop();
    }

    void ASTCodeGen::visit(FE::AST::IfStmt& node, Module* m)
    {
        // then, else(optional), end
        createBlock();
        size_t thenL = getMaxLabel() - 1;
        size_t elseL = static_cast<size_t>(-1);
        if (node.elseStmt)
        {
            createBlock();
            elseL = getMaxLabel() - 1;
        }
        createBlock();
        size_t endL = getMaxLabel() - 1;

        apply(*this, *node.cond, m);
        size_t condReg = getMaxReg();
        DataType condType = convert(node.cond->attr.val.value.type);
        if (condType != DataType::I1) {
            auto convs = createTypeConvertInst(condType, DataType::I1, condReg);
            for (auto& inst : convs) insert(inst);
            condReg = getMaxReg();
        }
        insert(createBranchInst(condReg, thenL, node.elseStmt ? elseL : endL));

        // then
        enterBlock(thenL);        
        name2reg.enterScope();
        if (node.thenStmt) {
            apply(*this, *node.thenStmt, m);
        }
        name2reg.exitScope();
        // 如果 then 块为空或没有终结指令，补充跳转到 end
        if (curBlock->insts.empty() || !curBlock->insts.back()->isTerminator())
            insert(createBranchInst(endL));

        // else
        if (node.elseStmt) {
            enterBlock(elseL);
            name2reg.enterScope();
            apply(*this, *node.elseStmt, m);
            name2reg.exitScope();
            if (curBlock->insts.empty() || !curBlock->insts.back()->isTerminator())
            insert(createBranchInst(endL));
        }

        // end
        enterBlock(endL);
    }

    void ASTCodeGen::visit(FE::AST::BreakStmt& node, Module* m)
    {
        (void)node;
        (void)m;
        if (!inLoop()) ERROR("break not in loop");
        LoopLabels current = getCurrentLoop();
        insert(createBranchInst(current.end));
    }

    void ASTCodeGen::visit(FE::AST::ContinueStmt& node, Module* m)
    {
        (void)node;
        (void)m;
        if (!inLoop()) ERROR("continue not in loop");
        LoopLabels current = getCurrentLoop();
        insert(createBranchInst(current.start));
    }

    void ASTCodeGen::visit(FE::AST::ForStmt& node, Module* m)
    {
        // for(init; cond; step) body
        if (node.init) apply(*this, *node.init, m);

        createBlock();
        size_t condL = getMaxLabel() - 1;
        createBlock();
        size_t bodyL = getMaxLabel() - 1;
        createBlock();
        size_t stepL = getMaxLabel() - 1;
        createBlock();
        size_t endL = getMaxLabel() - 1;

        // 将循环标签压栈
        pushLoop(stepL, endL);

        insert(createBranchInst(condL));

        // cond
        enterBlock(condL);
        if (node.cond)
        {
            apply(*this, *node.cond, m);
            size_t condReg = getMaxReg();
            DataType condType = convert(node.cond->attr.val.value.type);
            if (condType != DataType::I1) {
                auto convs = createTypeConvertInst(condType, DataType::I1, condReg);
                for (auto& inst : convs) insert(inst);
                condReg = getMaxReg();
            }
            insert(createBranchInst(condReg, bodyL, endL));
        }
        else
        {
            // 无条件跳转到 body
            insert(createBranchInst(bodyL));
        }

        // body
        enterBlock(bodyL);
        if (node.body) apply(*this, *node.body, m);
        if (curBlock->insts.empty() || !curBlock->insts.back()->isTerminator()) 
            insert(createBranchInst(stepL));

        // step
        enterBlock(stepL);
        if (node.step) apply(*this, *node.step, m);
        insert(createBranchInst(condL));

        // end
        enterBlock(endL);

        // 弹出循环标签
        popLoop();
    }
}  // namespace ME

    