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

        // 为形参在函数定义中创建寄存器，并记录映射
        if (node.params)
        {
            for (auto p : *node.params)
            {
                DataType at = convert(p->type);
                if (p->dims) at = DataType::PTR;
                size_t preg = getNewRegId();
                // 将参数寄存器加入 FuncDefInst 的 argRegs
                fdef->argRegs.emplace_back(at, getRegOperand(preg));
                // 为参数创建本地 alloca，并把参数存入其中，这样变量访问仍使用指针
                size_t ptrReg = getNewRegId();
                insert(new AllocaInst(at, getRegOperand(ptrReg)));
                insert(createStoreInst(at, preg, getRegOperand(ptrReg)));

                name2reg.addSymbol(p->entry, ptrReg);
                FE::AST::VarAttr va; va.type = p->type;
                reg2attr[ptrReg] = va;
                if (p->dims) paramPtrTab[preg] = true;
            }
        }

        // 生成函数体
        if (node.body) apply(*this, *node.body, m);

        // 如果最后一个基本块没有 terminator，则补上返回指令
        if (curBlock && !curBlock->insts.empty())
        {
            Instruction* last = curBlock->insts.back();
            if (!last->isTerminator())
            {
                if (retType == DataType::VOID) insert(createRetInst());
                else if (retType == DataType::I32) insert(createRetInst(0));
                else if (retType == DataType::F32) insert(createRetInst(0.0f));
            }
        }

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

        // 跳转到 cond
        insert(createBranchInst(condL));

        // cond
        enterBlock(condL);
        apply(*this, *node.cond, m);
        size_t condReg = getMaxReg();
        insert(createBranchInst(condReg, bodyL, endL));

        // body
        // 记录循环标签，支持 break/continue
        size_t prevLoopStart = curFunc->loopStartLabel;
        size_t prevLoopEnd = curFunc->loopEndLabel;
        curFunc->loopStartLabel = condL;
        curFunc->loopEndLabel = endL;

        enterBlock(bodyL);
        apply(*this, *node.body, m);
        // 若 body 未以 terminator 结束，则回到 cond
        if (!curBlock->insts.empty() && !curBlock->insts.back()->isTerminator()) insert(createBranchInst(condL));

        // 恢复循环标签
        curFunc->loopStartLabel = prevLoopStart;
        curFunc->loopEndLabel = prevLoopEnd;

        // end
        enterBlock(endL);
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
        if (!curFunc) ERROR("break not in function");
        insert(createBranchInst(curFunc->loopEndLabel));
    }

    void ASTCodeGen::visit(FE::AST::ContinueStmt& node, Module* m)
    {
        (void)node;
        (void)m;
        if (!curFunc) ERROR("continue not in function");
        insert(createBranchInst(curFunc->loopStartLabel));
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

        insert(createBranchInst(condL));

        // cond
        enterBlock(condL);
        if (node.cond)
        {
            apply(*this, *node.cond, m);
            size_t condReg = getMaxReg();
            insert(createBranchInst(condReg, bodyL, endL));
        }
        else
        {
            // 无条件跳转到 body
            insert(createBranchInst(bodyL));
        }

        // body
        size_t prevLoopStart = curFunc->loopStartLabel;
        size_t prevLoopEnd = curFunc->loopEndLabel;
        curFunc->loopStartLabel = stepL;
        curFunc->loopEndLabel = endL;

        enterBlock(bodyL);
        apply(*this, *node.body, m);
        if (!curBlock->insts.empty() && !curBlock->insts.back()->isTerminator()) insert(createBranchInst(stepL));

        // step
        enterBlock(stepL);
        if (node.step) apply(*this, *node.step, m);
        insert(createBranchInst(condL));

        curFunc->loopStartLabel = prevLoopStart;
        curFunc->loopEndLabel = prevLoopEnd;

        enterBlock(endL);
    }
}  // namespace ME

    