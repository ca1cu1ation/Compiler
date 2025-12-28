#include <backend/targets/aarch64/isel/aarch64_dag_isel.h>
#include <backend/target/target.h>
#include <backend/dag/selection_dag.h>
#include <backend/dag/isd.h>
#include <backend/dag/dag_builder.h>
#include <backend/targets/aarch64/dag/aarch64_dag_legalize.h>
#include <backend/targets/aarch64/aarch64_defs.h>
#include <middleend/module/ir_instruction.h>
#include <middleend/module/ir_function.h>
#include <debug.h>
#include <transfer.h>
#include <functional>
#include <cstring>

// 在 README 中已经说明了这里可能会出现 I32 op I64 的情况
// 并且 selectBinary 中我也将处理这个情况的代码删去了，请自行考虑如何解决这个问题
// 额外的，在 ../aarch64_def.h 中我定义了 LOC_STR 宏，它可以用于在创建指令时添加位置信息
// 启用这个宏也许可以在你 debug 时帮助你迅速定位到一条错误的指令是在哪里创建的

namespace BE::AArch64
{
    std::vector<const DAG::SDNode*> DAGIsel::scheduleDAG(const DAG::SelectionDAG& dag)
    {
        std::vector<const DAG::SDNode*> order;
        std::set<const DAG::SDNode*>    vis;

        std::function<void(const DAG::SDNode*)> dfs = [&](const DAG::SDNode* n) {
            if (!n) return;
            if (vis.count(n)) return;
            vis.insert(n);

            for (unsigned i = 0; i < n->getNumOperands(); ++i)
            {
                auto* opn = n->getOperand(i).getNode();
                if (opn) dfs(opn);
            }

            order.push_back(n);
        };

        for (auto* n : dag.getNodes()) dfs(n);

        return order;
    }

    void DAGIsel::allocateRegistersForNode(const DAG::SDNode* node)
    {
        // ============================================================================
        // 为调度后的 DAG 节点预分配虚拟寄存器
        // ============================================================================
        //
        // 为什么需要：
        // - 在指令选择前，先为每个计算结果分配虚拟寄存器
        // - 确保跨基本块的值（如 PHI 操作数）使用一致的寄存器映射
        // - 分离"分配"与"使用"两个阶段，简化后续的指令生成逻辑
        //
        // 策略：
        // - 若节点对应 IR 寄存器，复用 IR 寄存器映射
        // - 否则分配临时虚拟寄存器

        if (node->getNumValues() == 0) return;

        auto opcode = static_cast<DAG::ISD>(node->getOpcode());

        if (opcode == DAG::ISD::LABEL || opcode == DAG::ISD::SYMBOL || opcode == DAG::ISD::CONST_I32 ||
            opcode == DAG::ISD::CONST_I64 || opcode == DAG::ISD::CONST_F32 || opcode == DAG::ISD::FRAME_INDEX)
            return;

        BE::DataType* dt = node->getValueType(0);
        Register      vreg;

        if (node->hasIRRegId())
            vreg = getOrCreateVReg(node->getIRRegId(), dt);
        else
            vreg = getVReg(dt);

        nodeToVReg_[node] = vreg;
    }

    Register DAGIsel::getOperandReg(const DAG::SDNode* node, BE::Block* m_block)
    {
        // ============================================================================
        // 操作数获取：统一的实例化入口
        // ============================================================================
        //
        // 作用：为 DAG 节点返回或实例化对应的寄存器
        //
        // 为什么需要：
        // - 指令选择时，需要将 DAG 节点（抽象）映射到寄存器（具体）
        // - 统一处理已分配寄存器、IR 寄存器映射、按需实例化三种情况

        if (!node) ERROR("Cannot get register for null node");

        auto opcode = static_cast<DAG::ISD>(node->getOpcode());

        // If this IR reg represents a stack object (alloca), it has no real def.
        // Always materialize its address as (sp + frame_offset) when used.
        if (opcode == DAG::ISD::REG && node->hasIRRegId() && ctx_.mfunc && ctx_.mfunc->frameInfo.hasObject(node->getIRRegId()))
        {
            Register addrReg = getVReg(BE::I64);
            Instr*   addrInst = createInstr2(Operator::ADD, new RegOperand(addrReg), new RegOperand(PR::sp));
            addrInst->fiop = new FrameIndexOperand(static_cast<int>(node->getIRRegId()));
            addrInst->use_fiops = true;
            m_block->insts.push_back(addrInst);
            nodeToVReg_[node] = addrReg;
            return addrReg;
        }

        auto it = nodeToVReg_.find(node);
        if (it != nodeToVReg_.end()) return it->second;

        if (opcode == DAG::ISD::REG && node->hasIRRegId())
            return getOrCreateVReg(node->getIRRegId(), node->getNumValues() > 0 ? node->getValueType(0) : BE::I64);

        if (opcode == DAG::ISD::CONST_I32 || opcode == DAG::ISD::CONST_I64)
        {
            DataType* dt = (node->getNumValues() > 0) ? node->getValueType(0) : BE::I32;
            if (!dt) dt = (opcode == DAG::ISD::CONST_I32) ? BE::I32 : BE::I64;

            Register destReg = getVReg(dt);
            int64_t  imm     = node->hasImmI64() ? node->getImmI64() : 0;

            if (imm == 0)
            {
                Register zeroReg  = (dt == BE::I32) ? PR::wzr : PR::xzr;
                nodeToVReg_[node] = zeroReg;
                return zeroReg;
            }

            auto   segments    = decomposeImm64(static_cast<unsigned long long>(imm));
            size_t numSegments = (dt == BE::I32) ? 2 : 4;

            bool first = true;
            for (size_t i = 0; i < numSegments; ++i)
            {
                if (segments[i] == 0 && !first) continue;
                if (first)
                {
                    m_block->insts.push_back(
                        createInstr2(Operator::MOVZ, new RegOperand(destReg), new ImmeOperand(segments[i])));
                    first = false;
                }
                else
                {
                    m_block->insts.push_back(createInstr3(Operator::MOVK,
                        new RegOperand(destReg),
                        new ImmeOperand(segments[i]),
                        new ImmeOperand(i * 16)));
                }
            }

            nodeToVReg_[node] = destReg;
            return destReg;
        }

        if (opcode == DAG::ISD::CONST_F32)
        {
            DataType* dt = (node->getNumValues() > 0) ? node->getValueType(0) : BE::F32;
            if (!dt) dt = BE::F32;
            Register dstReg = getVReg(dt);

            float fval = node->hasImmF32() ? node->getImmF32() : 0.0f;

            if (fval == 0.0f)
            {
                m_block->insts.push_back(createInstr2(Operator::FMOV, new RegOperand(dstReg), new RegOperand(PR::wzr)));
                nodeToVReg_[node] = dstReg;
                return dstReg;
            }

            int      bits  = FLOAT_TO_INT_BITS(fval);
            Register wTmp  = getVReg(BE::I32);
            auto     segs  = decomposeImm64(static_cast<unsigned long long>(static_cast<uint32_t>(bits)));
            bool     first = true;
            for (size_t i = 0; i < 2; ++i)
            {
                if (segs[i] == 0 && !first) continue;
                if (first)
                {
                    m_block->insts.push_back(
                        createInstr2(Operator::MOVZ, new RegOperand(wTmp), new ImmeOperand(segs[i])));
                    first = false;
                }
                else
                {
                    m_block->insts.push_back(createInstr3(
                        Operator::MOVK, new RegOperand(wTmp), new ImmeOperand(segs[i]), new ImmeOperand(i * 16)));
                }
            }
            m_block->insts.push_back(createInstr2(Operator::FMOV, new RegOperand(dstReg), new RegOperand(wTmp)));

            nodeToVReg_[node] = dstReg;
            return dstReg;
        }

        if (opcode == DAG::ISD::FRAME_INDEX || opcode == DAG::ISD::SYMBOL) return materializeAddress(node, m_block);

        ERROR("Node not scheduled or cannot be materialized: %s",
            DAG::toString(static_cast<DAG::ISD>(node->getOpcode())));
        return Register();
    }

    Register DAGIsel::materializeAddress(const DAG::SDNode* node, BE::Block* m_block)
    {
        // ============================================================================
        // 地址实例化：使用者负责实例化
        // ============================================================================
        //
        // 作用：将地址节点（FRAME_INDEX/SYMBOL）实例化为寄存器
        //
        // 为什么需要：
        // - 地址节点本身不生成独立的指令，而是由使用者（LOAD/STORE/ADD）决定如何实例化
        // - FRAME_INDEX 需要生成抽象的 FrameIndexOperand，由后续 Pass 替换为实际偏移
        // - SYMBOL 需要生成 LA 伪指令加载全局变量地址

        if (!node) ERROR("Cannot materialize null address");

        auto opcode = static_cast<DAG::ISD>(node->getOpcode());

        if (opcode == DAG::ISD::FRAME_INDEX && node->hasIRRegId())
        {
            size_t   ir_reg_id = node->getIRRegId();
            Register addrReg   = getVReg(BE::I64);

            Instr* addr_inst     = createInstr2(Operator::ADD, new RegOperand(addrReg), new RegOperand(PR::sp));
            addr_inst->fiop      = new FrameIndexOperand(ir_reg_id);
            addr_inst->use_fiops = true;
            m_block->insts.push_back(addr_inst);

            return addrReg;
        }

        if (opcode == DAG::ISD::SYMBOL && node->hasSymbol())
        {
            Register addrReg = getVReg(BE::I64);
            m_block->insts.push_back(
                createInstr2(Operator::LA, new RegOperand(addrReg), new SymbolOperand(node->getSymbol())));
            return addrReg;
        }

        auto it = nodeToVReg_.find(node);
        if (it != nodeToVReg_.end()) return it->second;

        if (opcode == DAG::ISD::REG && node->hasIRRegId())
            return getOrCreateVReg(node->getIRRegId(), node->getNumValues() > 0 ? node->getValueType(0) : BE::I64);

        ERROR("Cannot materialize address for opcode: %s", DAG::toString(opcode));
    }

    int DAGIsel::dataTypeSize(BE::DataType* dt)
    {
        if (dt == BE::I32 || dt == BE::F32) return 4;
        if (dt == BE::I64 || dt == BE::F64 || dt == BE::PTR) return 8;
        return 4;
    }

    Register DAGIsel::getOrCreateVReg(size_t ir_reg_id, BE::DataType* dt)
    {
        auto it = ctx_.vregMap.find(ir_reg_id);
        if (it != ctx_.vregMap.end()) return it->second;

        Register vreg           = getVReg(dt);
        ctx_.vregMap[ir_reg_id] = vreg;
        return vreg;
    }

    void DAGIsel::importGlobals()
    {
        auto mapType = [](ME::DataType t) -> BE::DataType* {
            switch (t)
            {
                case ME::DataType::I32: return BE::I32;
                case ME::DataType::I64: return BE::I64;
                case ME::DataType::F32: return BE::F32;
                case ME::DataType::DOUBLE: return BE::F64;
                default: return BE::I32;
            }
        };

        for (auto* gv : ir_module_->globalVars)
        {
            if (!gv) continue;

            auto* beTy = mapType(gv->dt);
            auto* bgv  = new BE::GlobalVariable(beTy, gv->name);

            bgv->dims = gv->initList.arrayDims;

            auto pushVal = [&](const FE::AST::VarValue& v) {
                // 统一按 int64 储存，再由 codegen 按位数输出
                if (beTy == BE::F32)
                    bgv->initVals.push_back(static_cast<int>(FLOAT_TO_INT_BITS(v.getFloat())));
                else if (beTy == BE::F64)
                    bgv->initVals.push_back(static_cast<int>(v.getLL()));
                else
                    bgv->initVals.push_back(static_cast<int>(v.getLL()));
            };

            if (!gv->initList.arrayDims.empty())
            {
                for (auto& v : gv->initList.initList) pushVal(v);
            }
            else if (gv->init)
            {
                if (gv->init->getType() == ME::OperandType::IMMEI32)
                    bgv->initVals.push_back(static_cast<ME::ImmeI32Operand*>(gv->init)->value);
                else if (gv->init->getType() == ME::OperandType::IMMEF32)
                {
                    auto f = static_cast<ME::ImmeF32Operand*>(gv->init)->value;
                    bgv->initVals.push_back(static_cast<int>(FLOAT_TO_INT_BITS(f)));
                }
                else
                    bgv->initVals.push_back(0);
            }

            m_backend_module->globals.push_back(bgv);
        }
    }

    void DAGIsel::collectAllocas(ME::Function* ir_func)
    {
        auto sz = [&](ME::DataType t) {
            switch (t)
            {
                case ME::DataType::I32:
                case ME::DataType::F32: return 4;
                case ME::DataType::I64:
                case ME::DataType::PTR:
                case ME::DataType::DOUBLE: return 8;
                default: return 4;
            }
        };

        for (auto& [_, block] : ir_func->blocks)
        {
            for (auto* inst : block->insts)
            {
                auto* allocaInst = dynamic_cast<ME::AllocaInst*>(inst);
                if (!allocaInst) continue;

                int bytes = sz(allocaInst->dt);
                int cnt   = 1;
                for (int d : allocaInst->dims) cnt *= d;
                bytes *= cnt;

                size_t irReg = static_cast<ME::RegOperand*>(allocaInst->res)->getRegNum();
                ctx_.allocaFI[irReg] = ctx_.mfunc->frameInfo.createOrGetObject(irReg, bytes);
            }
        }
    }

    void DAGIsel::setupParameters(ME::Function* ir_func)
    {
        auto* entryBlock = ctx_.mfunc->blocks.begin()->second;

        int intIdx   = 0;
        int floatIdx = 0;
        int stackOff = 0;

        // Our frame lowering saves a fixed set of callee-saved regs and then does:
        //   stp x29, x30, [sp, #-16]!
        //   mov x29, sp
        // Therefore, caller stack arguments (at entry SP) live at [x29, #kSavedAreaBytes + off].
        constexpr int kSavedAreaBytes = 160;  // 4*16 (d8-d15) + 5*16 (x19-x28) + 1*16 (fp/lr)

        for (auto& [dt, op] : ir_func->funcDef->argRegs)
        {
            auto* regOp = dynamic_cast<ME::RegOperand*>(op);
            if (!regOp) continue;

            BE::DataType* ty = (dt == ME::DataType::F32 || dt == ME::DataType::DOUBLE) ? BE::F32 : BE::I64;
            if (dt == ME::DataType::I32) ty = BE::I32;

            Register vreg = getOrCreateVReg(regOp->getRegNum(), ty);

            bool isFloat = (ty == BE::F32 || ty == BE::F64);
            if (!isFloat && intIdx < A64_GPR_ARG_COUNT)
            {
                Register preg = (ty == BE::I32) ? Register(intIdx, BE::I32, false) : Register(intIdx, BE::I64, false);
                entryBlock->insts.push_front(BE::AArch64::createMove(new RegOperand(vreg), new RegOperand(preg)));
                ctx_.mfunc->params.push_back(preg);
                ++intIdx;
            }
            else if (isFloat && floatIdx < A64_FPR_ARG_COUNT)
            {
                Register preg = (ty == BE::F32) ? Register(floatIdx, BE::F32, false) : Register(floatIdx, BE::F64, false);
                entryBlock->insts.push_front(BE::AArch64::createMove(new RegOperand(vreg), new RegOperand(preg)));
                ctx_.mfunc->params.push_back(preg);
                ++floatIdx;
            }
            else
            {
                int off      = stackOff;
                stackOff += 8;
                // Incoming stack parameters: load from caller stack (do NOT reserve outgoing param area here).
                entryBlock->insts.push_front(
                    createInstr2(Operator::LDR, new RegOperand(vreg), new MemOperand(PR::x29, kSavedAreaBytes + off)));
                ctx_.mfunc->hasStackParam = true;
            }
        }
    }

    bool DAGIsel::selectAddress(const DAG::SDNode* addrNode, const DAG::SDNode*& baseNode, int64_t& offset)
    {
        auto opcode = static_cast<DAG::ISD>(addrNode->getOpcode());

        if (opcode == DAG::ISD::FRAME_INDEX || opcode == DAG::ISD::SYMBOL)
        {
            baseNode = addrNode;
            offset   = 0;
            return true;
        }

        if (opcode == DAG::ISD::ADD)
        {
            const DAG::SDNode* lhs = addrNode->getOperand(0).getNode();
            const DAG::SDNode* rhs = addrNode->getOperand(1).getNode();

            const DAG::SDNode* lhsBase;
            int64_t            lhsOffset = 0;
            if (selectAddress(lhs, lhsBase, lhsOffset))
            {
                if ((static_cast<DAG::ISD>(rhs->getOpcode()) == DAG::ISD::CONST_I32 ||
                        static_cast<DAG::ISD>(rhs->getOpcode()) == DAG::ISD::CONST_I64) &&
                    rhs->hasImmI64())
                {
                    baseNode = lhsBase;
                    offset   = lhsOffset + rhs->getImmI64();
                    // AArch64 LDR/STR immediate offset limits:
                    // - Unscaled: [-256, 255]
                    // - Scaled (unsigned): [0, 32760] (aligned)
                    // For simplicity, we only accept small offsets here to be safe.
                    // Large offsets will be materialized into a register by the fallback path.
                    if (offset >= -256 && offset <= 255) return true;
                    if (offset >= 0 && offset <= 4095 && (offset % 4 == 0)) return true; // Rough check for aligned
                    return false;
                }
                return false;
            }

            const DAG::SDNode* rhsBase;
            int64_t            rhsOffset = 0;
            if (selectAddress(rhs, rhsBase, rhsOffset))
            {
                if ((static_cast<DAG::ISD>(lhs->getOpcode()) == DAG::ISD::CONST_I32 ||
                        static_cast<DAG::ISD>(lhs->getOpcode()) == DAG::ISD::CONST_I64) &&
                    lhs->hasImmI64())
                {
                    baseNode = rhsBase;
                    offset   = rhsOffset + lhs->getImmI64();
                    if (offset >= -256 && offset <= 255) return true;
                    if (offset >= 0 && offset <= 4095 && (offset % 4 == 0)) return true;
                    return false;
                }
                return false;
            }

            return false;
        }

        return false;
    }

    void DAGIsel::selectCopy(const DAG::SDNode* node, BE::Block* m_block)
    {
        if (node->getNumOperands() < 1) return;

        const DAG::SDNode* src = node->getOperand(0).getNode();
        if (!src) return;

        Register dst    = getOperandReg(node, m_block);
        Register srcReg = getOperandReg(src, m_block);

        m_block->insts.push_back(BE::AArch64::createMove(new RegOperand(dst), new RegOperand(srcReg), LOC_STR));
    }

    void DAGIsel::selectConst(const DAG::SDNode* node, BE::Block* m_block)
    {
        // 常量在 getOperandReg 中被实例化，这里只确保映射建立
        getOperandReg(node, m_block);
    }

    void DAGIsel::selectPhi(const DAG::SDNode* node, BE::Block* m_block)
    {
        Register dst = nodeToVReg_.at(node);
        auto*    phi = new PhiInst(dst, LOC_STR);

        for (unsigned i = 0; i + 1 < node->getNumOperands(); i += 2)
        {
            const DAG::SDNode* lbNode  = node->getOperand(i).getNode();
            const DAG::SDNode* valNode = node->getOperand(i + 1).getNode();
            if (!lbNode || !valNode) continue;

            int   labelId = lbNode->hasImmI64() ? static_cast<int>(lbNode->getImmI64()) : 0;
            
            Operand* valOp = nullptr;
            auto     opc   = static_cast<DAG::ISD>(valNode->getOpcode());
            if (opc == DAG::ISD::CONST_I32 || opc == DAG::ISD::CONST_I64)
            {
                valOp = new ImmeOperand(static_cast<int>(valNode->getImmI64()));
            }
            else
            {
                valOp = new RegOperand(getOperandReg(valNode, m_block));
            }
            phi->incomingVals[labelId] = valOp;
        }

        m_block->insts.push_back(phi);
    }

    void DAGIsel::selectBinary(const DAG::SDNode* node, BE::Block* m_block)
    {
        // ============================================================================
        // 选择二元运算节点（示例实现，保留）
        // ============================================================================
        //
        // AArch64 特殊处理：
        // - 寄存器位宽匹配：x/w 寄存器别名需要类型转换
        // - MOD 运算：展开为 a - (a / b) * b

        if (node->getNumOperands() < 2) return;

        auto opcode = static_cast<DAG::ISD>(node->getOpcode());

        Register dst = nodeToVReg_.at(node);

        const DAG::SDNode* lhs = node->getOperand(0).getNode();
        const DAG::SDNode* rhs = node->getOperand(1).getNode();

        Register lhsReg = getOperandReg(lhs, m_block);
        Register rhsReg = getOperandReg(rhs, m_block);

        Operator op;
        bool     isFloat = (dst.dt == BE::F32 || dst.dt == BE::F64);

        if (!isFloat && dst.dt == BE::I32)
        {
            if (lhsReg.dt == BE::I64) lhsReg = Register(lhsReg.rId, BE::I32, lhsReg.isVreg);
            if (rhsReg.dt == BE::I64) rhsReg = Register(rhsReg.rId, BE::I32, rhsReg.isVreg);
        }

        switch (opcode)
        {
            case DAG::ISD::ADD: op = isFloat ? Operator::FADD : Operator::ADD; break;
            case DAG::ISD::SUB: op = isFloat ? Operator::FSUB : Operator::SUB; break;
            case DAG::ISD::MUL:
            {
                if (!isFloat)
                {
                    auto isPowerOf2 = [](int64_t n) { return n > 0 && (n & (n - 1)) == 0; };
                    auto log2       = [](int64_t n) {
                        int s = 0;
                        while ((n >> s) > 1) s++;
                        return s;
                    };

                    auto rhsOp = static_cast<DAG::ISD>(rhs->getOpcode());
                    if ((rhsOp == DAG::ISD::CONST_I32 || rhsOp == DAG::ISD::CONST_I64) &&
                        rhs->hasImmI64() && isPowerOf2(rhs->getImmI64()))
                    {
                        m_block->insts.push_back(createInstr3(Operator::LSL,
                            new RegOperand(dst),
                            new RegOperand(lhsReg),
                            new ImmeOperand(log2(rhs->getImmI64()))));
                        return;
                    }

                    auto lhsOp = static_cast<DAG::ISD>(lhs->getOpcode());
                    if ((lhsOp == DAG::ISD::CONST_I32 || lhsOp == DAG::ISD::CONST_I64) &&
                        lhs->hasImmI64() && isPowerOf2(lhs->getImmI64()))
                    {
                        m_block->insts.push_back(createInstr3(Operator::LSL,
                            new RegOperand(dst),
                            new RegOperand(rhsReg),
                            new ImmeOperand(log2(lhs->getImmI64()))));
                        return;
                    }
                }
                op = isFloat ? Operator::FMUL : Operator::MUL;
                break;
            }
            case DAG::ISD::DIV: op = isFloat ? Operator::FDIV : Operator::SDIV; break;
            case DAG::ISD::FADD: op = Operator::FADD; break;
            case DAG::ISD::FSUB: op = Operator::FSUB; break;
            case DAG::ISD::FMUL: op = Operator::FMUL; break;
            case DAG::ISD::FDIV: op = Operator::FDIV; break;
            case DAG::ISD::AND: op = Operator::AND; break;
            case DAG::ISD::OR: op = Operator::ORR; break;
            case DAG::ISD::XOR: op = Operator::EOR; break;
            case DAG::ISD::SHL: op = Operator::LSL; break;
            case DAG::ISD::ASHR: op = Operator::ASR; break;
            case DAG::ISD::LSHR: op = Operator::LSR; break;
            case DAG::ISD::MOD:
            {
                // Integer remainder: a % b = a - (a / b) * b
                if (isFloat) ERROR("MOD is not defined for float in this backend");
                Register q   = getVReg(dst.dt);
                Register prod = getVReg(dst.dt);
                m_block->insts.push_back(
                    createInstr3(Operator::SDIV, new RegOperand(q), new RegOperand(lhsReg), new RegOperand(rhsReg)));
                m_block->insts.push_back(
                    createInstr3(Operator::MUL, new RegOperand(prod), new RegOperand(q), new RegOperand(rhsReg)));
                m_block->insts.push_back(
                    createInstr3(Operator::SUB, new RegOperand(dst), new RegOperand(lhsReg), new RegOperand(prod)));
                return;
            }
            default: ERROR("Unsupported binary operator: %d", node->getOpcode()); return;
        }

        m_block->insts.push_back(createInstr3(op, new RegOperand(dst), new RegOperand(lhsReg), new RegOperand(rhsReg)));
    }

    void DAGIsel::selectUnary(const DAG::SDNode* node, BE::Block* m_block)
    {
        (void)node;
        (void)m_block;
    }

    void DAGIsel::selectLoad(const DAG::SDNode* node, BE::Block* m_block)
    {
        // ============================================================================
        // 选择 LOAD 节点, 作为示例实现，仅供参考
        // ============================================================================
        //
        // 作用：
        // 为 LOAD 节点生成目标相关的访存指令（LDR）。
        //
        // 为什么需要地址选择：
        // - 地址可能是简单的 [base + offset]，也可能是复杂表达式
        // - 声明式地址选择（selectAddress）可将常见模式折叠到访存指令的立即数字段
        // - 若地址过于复杂，需先完整计算到寄存器

        if (node->getNumOperands() < 2) return;

        Register           dst  = nodeToVReg_.at(node);
        const DAG::SDNode* addr = node->getOperand(1).getNode();

        const DAG::SDNode* baseNode;
        int64_t            offset = 0;

        if (selectAddress(addr, baseNode, offset))
        {
            Register baseReg;

            if (static_cast<DAG::ISD>(baseNode->getOpcode()) == DAG::ISD::FRAME_INDEX)
            {
                int fi              = baseNode->getFrameIndex();
                baseReg             = getVReg(BE::I64);
                Instr* addrInst     = createInstr2(Operator::ADD, new RegOperand(baseReg), new RegOperand(PR::sp));
                addrInst->fiop      = new FrameIndexOperand(fi);
                addrInst->use_fiops = true;
                m_block->insts.push_back(addrInst);
            }
            else if (static_cast<DAG::ISD>(baseNode->getOpcode()) == DAG::ISD::SYMBOL && baseNode->hasSymbol())
            {
                std::string symbol = baseNode->getSymbol();
                baseReg            = getVReg(BE::I64);
                m_block->insts.push_back(
                    createInstr2(Operator::LA, new RegOperand(baseReg), new SymbolOperand(symbol)));
            }
            else { baseReg = getOperandReg(baseNode, m_block); }

            m_block->insts.push_back(
                createInstr2(Operator::LDR, new RegOperand(dst), new MemOperand(baseReg, static_cast<int>(offset))));
        }
        else
        {
            Register addrReg = getOperandReg(addr, m_block);
            m_block->insts.push_back(createInstr2(Operator::LDR, new RegOperand(dst), new MemOperand(addrReg, 0)));
        }
    }

    void DAGIsel::selectStore(const DAG::SDNode* node, BE::Block* m_block)
    {
        if (node->getNumOperands() < 3) return;

        const DAG::SDNode* valNode  = node->getOperand(1).getNode();
        const DAG::SDNode* addrNode = node->getOperand(2).getNode();
        if (!valNode || !addrNode) return;

        Register src = getOperandReg(valNode, m_block);

        const DAG::SDNode* baseNode;
        int64_t            offset = 0;

        if (selectAddress(addrNode, baseNode, offset))
        {
            Register baseReg;
            if (static_cast<DAG::ISD>(baseNode->getOpcode()) == DAG::ISD::FRAME_INDEX)
            {
                int fi          = baseNode->getFrameIndex();
                baseReg         = getVReg(BE::I64);
                Instr* addrInst = createInstr2(Operator::ADD, new RegOperand(baseReg), new RegOperand(PR::sp));
                addrInst->fiop  = new FrameIndexOperand(fi);
                addrInst->use_fiops = true;
                m_block->insts.push_back(addrInst);
            }
            else if (static_cast<DAG::ISD>(baseNode->getOpcode()) == DAG::ISD::SYMBOL && baseNode->hasSymbol())
            {
                baseReg = getVReg(BE::I64);
                m_block->insts.push_back(createInstr2(Operator::LA, new RegOperand(baseReg), new SymbolOperand(baseNode->getSymbol())));
            }
            else { baseReg = getOperandReg(baseNode, m_block); }

            m_block->insts.push_back(createInstr2(Operator::STR, new RegOperand(src), new MemOperand(baseReg, static_cast<int>(offset))));
        }
        else
        {
            Register addrReg = getOperandReg(addrNode, m_block);
            m_block->insts.push_back(createInstr2(Operator::STR, new RegOperand(src), new MemOperand(addrReg, 0)));
        }
    }

    void DAGIsel::selectICmp(const DAG::SDNode* node, BE::Block* m_block)
    {
        if (node->getNumOperands() < 2) return;

        Register dst = nodeToVReg_.at(node);

        Register lhs = getOperandReg(node->getOperand(0).getNode(), m_block);
        Register rhs = getOperandReg(node->getOperand(1).getNode(), m_block);

        m_block->insts.push_back(createInstr2(Operator::CMP, new RegOperand(lhs), new RegOperand(rhs)));

        int cc = node->hasImmI64() ? static_cast<int>(node->getImmI64()) : 0;
        int condImm = 0;
        switch (static_cast<ME::ICmpOp>(cc))
        {
            case ME::ICmpOp::EQ: condImm = 0; break;
            case ME::ICmpOp::NE: condImm = 1; break;
            case ME::ICmpOp::UGT: condImm = 8; break;
            case ME::ICmpOp::UGE: condImm = 2; break;
            case ME::ICmpOp::ULT: condImm = 3; break;
            case ME::ICmpOp::ULE: condImm = 9; break;
            case ME::ICmpOp::SGT: condImm = 12; break;
            case ME::ICmpOp::SGE: condImm = 10; break;
            case ME::ICmpOp::SLT: condImm = 11; break;
            case ME::ICmpOp::SLE: condImm = 13; break;
            default: condImm = 0; break;
        }

        m_block->insts.push_back(createInstr2(Operator::CSET, new RegOperand(dst), new ImmeOperand(condImm)));
    }

    void DAGIsel::selectFCmp(const DAG::SDNode* node, BE::Block* m_block)
    {
        if (node->getNumOperands() < 2) return;

        Register dst = nodeToVReg_.at(node);

        Register lhs = getOperandReg(node->getOperand(0).getNode(), m_block);
        Register rhs = getOperandReg(node->getOperand(1).getNode(), m_block);

        m_block->insts.push_back(createInstr2(Operator::FCMP, new RegOperand(lhs), new RegOperand(rhs)));

        int cc = node->hasImmI64() ? static_cast<int>(node->getImmI64()) : 0;
        int condImm = 0;
        switch (static_cast<ME::FCmpOp>(cc))
        {
            case ME::FCmpOp::OEQ: condImm = 0; break;
            case ME::FCmpOp::ONE: condImm = 1; break;
            case ME::FCmpOp::OLT: condImm = 11; break;
            case ME::FCmpOp::OLE: condImm = 13; break;
            case ME::FCmpOp::OGT: condImm = 12; break;
            case ME::FCmpOp::OGE: condImm = 10; break;
            default: condImm = 0; break;
        }

        m_block->insts.push_back(createInstr2(Operator::CSET, new RegOperand(dst), new ImmeOperand(condImm)));
    }

    void DAGIsel::selectBranch(const DAG::SDNode* node, BE::Block* m_block)
    {
        auto opc = static_cast<DAG::ISD>(node->getOpcode());

        if (opc == DAG::ISD::BR)
        {
            const DAG::SDNode* target = node->getOperand(0).getNode();
            int                 tid    = target && target->hasImmI64() ? static_cast<int>(target->getImmI64()) : 0;
            m_block->insts.push_back(createInstr1(Operator::B, new LabelOperand(tid)));
            return;
        }

        if (node->getNumOperands() < 3) return;

        Register condReg = getOperandReg(node->getOperand(0).getNode(), m_block);
        const DAG::SDNode* tNode = node->getOperand(1).getNode();
        const DAG::SDNode* fNode = node->getOperand(2).getNode();
        int tId = tNode && tNode->hasImmI64() ? static_cast<int>(tNode->getImmI64()) : 0;
        int fId = fNode && fNode->hasImmI64() ? static_cast<int>(fNode->getImmI64()) : 0;

        m_block->insts.push_back(createInstr2(Operator::CMP, new RegOperand(condReg), new ImmeOperand(0)));
        m_block->insts.push_back(createInstr1(Operator::BNE, new LabelOperand(tId)));
        m_block->insts.push_back(createInstr1(Operator::B, new LabelOperand(fId)));
    }

    void DAGIsel::selectCall(const DAG::SDNode* node, BE::Block* m_block)
    {
        if (node->getNumOperands() < 2) return;

        const DAG::SDNode* symNode = node->getOperand(1).getNode();
        if (!symNode || !symNode->hasSymbol()) return;
        std::string callee = symNode->getSymbol();

        // IMPORTANT:
        // - Argument values may temporarily live in x0-x7.
        // - If we assign x0-x7 first, then later spill stack arguments, we can accidentally
        //   spill the *clobbered* values (especially in large-arg calls).
        // Strategy:
        // 1) Evaluate all argument values in order (emitting any needed code).
        // 2) Spill all stack-passed arguments to the outgoing argument area.
        // 3) Move register-passed arguments into x0-x7 / s0-d7.

        struct ArgLoc
        {
            Register src;
            bool     isFloat = false;
            bool     inReg   = false;
            Register preg;
            int      stackOff = -1;
            int      tmpOff   = -1;  // staging slot for reg-passed args
        };

        std::vector<ArgLoc> args;
        args.reserve(node->getNumOperands() > 2 ? node->getNumOperands() - 2 : 0);

        int intIdx = 0, floatIdx = 0, stackOff = 0;

        for (unsigned i = 2; i < node->getNumOperands(); ++i)
        {
            const DAG::SDNode* argNode = node->getOperand(i).getNode();
            if (!argNode) continue;

            Register src = getOperandReg(argNode, m_block);
            bool     isFloat = (src.dt == BE::F32 || src.dt == BE::F64);

            ArgLoc loc;
            loc.src     = src;
            loc.isFloat = isFloat;

            if (!isFloat && intIdx < A64_GPR_ARG_COUNT)
            {
                loc.inReg = true;
                loc.preg  = (src.dt == BE::I32) ? Register(intIdx, BE::I32, false) : Register(intIdx, BE::I64, false);
                ++intIdx;
            }
            else if (isFloat && floatIdx < A64_FPR_ARG_COUNT)
            {
                loc.inReg = true;
                loc.preg  = (src.dt == BE::F32) ? Register(floatIdx, BE::F32, false) : Register(floatIdx, BE::F64, false);
                ++floatIdx;
            }
            else
            {
                loc.inReg    = false;
                loc.stackOff = stackOff;
                stackOff += 8;
            }

            args.push_back(loc);
        }

        // Ensure outgoing argument area is large enough for both stack-passed args and
        // temporary staging slots for register-passed args.
        int regTmpOff = stackOff;
        for (auto& a : args)
        {
            if (!a.inReg) continue;
            a.tmpOff = regTmpOff;
            regTmpOff += 8;
        }

        if (regTmpOff > 0) ctx_.mfunc->frameInfo.setParamAreaSize(regTmpOff);

        // 1) Store ALL args to outgoing area first (safe even if sources live in x0-x7).
        for (const auto& a : args)
        {
            const int off = a.inReg ? a.tmpOff : a.stackOff;
            m_block->insts.push_back(createInstr2(Operator::STR, new RegOperand(a.src), new MemOperand(PR::sp, off)));
        }

        // 2) Load register-passed args from staging slots into x0-x7 / s0-d7.
        for (const auto& a : args)
        {
            if (!a.inReg) continue;
            m_block->insts.push_back(createInstr2(Operator::LDR, new RegOperand(a.preg), new MemOperand(PR::sp, a.tmpOff)));
        }

        m_block->insts.push_back(createInstr1(Operator::BL, new SymbolOperand(callee)));

        if (node->getNumValues() > 0)
        {
            Register dst = nodeToVReg_.at(node);
            Register src;
            if (dst.dt == BE::F32)
                src = PR::s0;
            else if (dst.dt == BE::F64)
                src = PR::d0;
            else if (dst.dt == BE::I32)
                src = PR::w0;
            else
                src = PR::x0;

            m_block->insts.push_back(BE::AArch64::createMove(new RegOperand(dst), new RegOperand(src), LOC_STR));
        }
    }

    void DAGIsel::selectRet(const DAG::SDNode* node, BE::Block* m_block)
    {
        // 操作数 0 是 Chain（保证副作用顺序），操作数 1 是实际返回值
        // 如有返回值，则将返回值移动到 x0/w0/s0/d0
        if (node->getNumOperands() > 1)
        {
            const DAG::SDNode* retValNode = node->getOperand(1).getNode();
            Register           retReg     = getOperandReg(retValNode, m_block);

            DataType* retType = retValNode->getNumValues() > 0 ? retValNode->getValueType(0) : BE::I32;

            Register destReg;
            if (retType == BE::F32 || retType == BE::F64)
                destReg = (retType == BE::F32) ? PR::s0 : PR::d0;
            else
                destReg = (retType == BE::I32) ? PR::w0 : PR::x0;

            if (retReg.dt->equal(I64) && destReg.dt->equal(I32)) destReg.dt = I64;
            m_block->insts.push_back(BE::AArch64::createMove(new RegOperand(destReg), new RegOperand(retReg), LOC_STR));
        }

        m_block->insts.push_back(createInstr0(Operator::RET));
    }

    void DAGIsel::selectCast(const DAG::SDNode* node, BE::Block* m_block)
    {
        auto opc = static_cast<DAG::ISD>(node->getOpcode());
        Register dst = nodeToVReg_.at(node);
        Register src = getOperandReg(node->getOperand(0).getNode(), m_block);

        if (opc == DAG::ISD::ZEXT)
        {
            m_block->insts.push_back(createInstr2(Operator::UXTW, new RegOperand(dst), new RegOperand(src)));
        }
        else if (opc == DAG::ISD::SITOFP)
        {
            m_block->insts.push_back(createInstr2(Operator::SCVTF, new RegOperand(dst), new RegOperand(src)));
        }
        else if (opc == DAG::ISD::FPTOSI)
        {
            m_block->insts.push_back(createInstr2(Operator::FCVTZS, new RegOperand(dst), new RegOperand(src)));
        }
    }

    void DAGIsel::selectNode(const DAG::SDNode* node, BE::Block* m_block)
    {
        if (!node) return;

        auto opcode = static_cast<DAG::ISD>(node->getOpcode());

        switch (opcode)
        {
            case DAG::ISD::LABEL:
            case DAG::ISD::SYMBOL:
            case DAG::ISD::ENTRY_TOKEN:
            case DAG::ISD::TOKEN_FACTOR:
            case DAG::ISD::FRAME_INDEX:
            case DAG::ISD::REG: break;
            case DAG::ISD::COPY: selectCopy(node, m_block); break;
            case DAG::ISD::PHI: selectPhi(node, m_block); break;
            case DAG::ISD::CONST_I32:
            case DAG::ISD::CONST_I64:
            case DAG::ISD::CONST_F32: selectConst(node, m_block); break;
            case DAG::ISD::ADD:
            case DAG::ISD::SUB:
            case DAG::ISD::MUL:
            case DAG::ISD::DIV:
            case DAG::ISD::MOD:
            case DAG::ISD::AND:
            case DAG::ISD::OR:
            case DAG::ISD::XOR:
            case DAG::ISD::SHL:
            case DAG::ISD::ASHR:
            case DAG::ISD::LSHR:
            case DAG::ISD::FADD:
            case DAG::ISD::FSUB:
            case DAG::ISD::FMUL:
            case DAG::ISD::FDIV: selectBinary(node, m_block); break;
            case DAG::ISD::LOAD: selectLoad(node, m_block); break;
            case DAG::ISD::STORE: selectStore(node, m_block); break;
            case DAG::ISD::ICMP: selectICmp(node, m_block); break;
            case DAG::ISD::FCMP: selectFCmp(node, m_block); break;
            case DAG::ISD::BR:
            case DAG::ISD::BRCOND: selectBranch(node, m_block); break;
            case DAG::ISD::CALL: selectCall(node, m_block); break;
            case DAG::ISD::RET: selectRet(node, m_block); break;
            case DAG::ISD::ZEXT:
            case DAG::ISD::SITOFP:
            case DAG::ISD::FPTOSI: selectCast(node, m_block); break;
            default: ERROR("Unsupported DAG node: %s", DAG::toString(static_cast<DAG::ISD>(node->getOpcode())));
        }
        selected_.insert(node);
    }

    void DAGIsel::selectBlock(ME::Block* ir_block, const DAG::SelectionDAG& dag)
    {
        (void)ir_block;

        nodeToVReg_.clear();
        selected_.clear();

        auto*  m_block = ctx_.mfunc->blocks[ir_block->blockId];
        auto   order   = scheduleDAG(dag);
        for (auto* n : order) allocateRegistersForNode(n);
        for (auto* n : order) selectNode(n, m_block);
    }

    void DAGIsel::selectFunction(ME::Function* ir_func)
    {
        ctx_ = FunctionContext();

        ctx_.mfunc = new BE::Function(ir_func->funcDef->funcName);
        m_backend_module->functions.push_back(ctx_.mfunc);

        // 创建基本块
        for (auto& [bid, _] : ir_func->blocks) ctx_.mfunc->blocks[static_cast<uint32_t>(bid)] = new BE::Block(bid);

        collectAllocas(ir_func);
        setupParameters(ir_func);

        // 选择每个块
        for (auto& [bid, ir_block] : ir_func->blocks)
        {
            (void)bid;
            auto* dag = target_->block_dags[ir_block];
            BE::AArch64::DAGLegalizer legalizer;
            legalizer.run(*dag);
            selectBlock(ir_block, *dag);
        }
    }

    void DAGIsel::runImpl()
    {
        importGlobals();

        target_->buildDAG(ir_module_);

        for (auto* f : ir_module_->functions) selectFunction(f);
    }

}  // namespace BE::AArch64
