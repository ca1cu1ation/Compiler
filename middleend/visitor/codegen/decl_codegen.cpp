#include <middleend/visitor/codegen/ast_codegen.h>
#include <debug.h>
#include <functional>

namespace ME
{
    void ASTCodeGen::visit(FE::AST::Initializer& node, Module* m)
    {
        (void)m;
        ERROR("Initializer should not appear here, at line %d", node.line_num);
    }
    void ASTCodeGen::visit(FE::AST::InitializerList& node, Module* m)
    {
        (void)m;
        ERROR("InitializerList should not appear here, at line %d", node.line_num);
    }
    void ASTCodeGen::visit(FE::AST::VarDeclarator& node, Module* m)
    {
        (void)m;
        ERROR("VarDeclarator should not appear here, at line %d", node.line_num);
    }
    void ASTCodeGen::visit(FE::AST::ParamDeclarator& node, Module* m)
    {
        (void)m;
        ERROR("ParamDeclarator should not appear here, at line %d", node.line_num);
    }

    void ASTCodeGen::visit(FE::AST::VarDeclaration& node, Module* m)
    {
        (void)m;
        // 在函数内生成局部变量分配（alloca）并处理可选初始化
        for (auto decl : *node.decls)
        {
            FE::AST::LeftValExpr* lval = dynamic_cast<FE::AST::LeftValExpr*>(decl->lval);
            ASSERT(lval && "VarDeclarator.lval should be LeftValExpr");

            FE::Sym::Entry* entry = lval->entry;
            DataType dt = convert(node.type);

            size_t valReg = static_cast<size_t>(-1);

            // 为该变量分配指针寄存器
            size_t ptrReg = getNewRegId();

            // 将 alloca 指令插入到入口块的前端，避免在循环中重复分配导致栈溢出
            Block* entryBlock = curFunc->getBlock(0);

            FE::AST::VarAttr attr;
            attr.type = node.type;

            if (lval->indices && !lval->indices->empty())
            {
                std::vector<int> dims;
                // 处理数组维度 - 计算维度数量
                for (auto expr : *lval->indices) {
                    if (expr->attr.val.isConstexpr) {
                        int val = expr->attr.val.getInt();
                        dims.push_back(val);
                        attr.arrayDims.push_back(val);
                    } else {
                        // Fallback for non-constant dimensions (should not happen in valid SysY)
                        dims.push_back(0); 
                        attr.arrayDims.push_back(0);
                    }
                }
                entryBlock->insertFront(new AllocaInst(dt, getRegOperand(ptrReg), dims));
            }
            else
            {
                entryBlock->insertFront(new AllocaInst(dt, getRegOperand(ptrReg)));
            }

            name2reg.addSymbol(entry, ptrReg);
            reg2attr[ptrReg] = attr;

            // 计算初始化值（如果有）
            if (decl->init && decl->init->singleInit)
            {
                FE::AST::Initializer* init = dynamic_cast<FE::AST::Initializer*>(decl->init);
                if (init && init->init_val)
                {
                    apply(*this, *init->init_val, m);
                    valReg = getMaxReg();
                    // 类型转换
                    DataType to = convert(node.type);
                    DataType from = convert(init->init_val->attr.val.value.type);
                    auto convs = createTypeConvertInst(from, to, valReg);
                    for (auto& ins : convs) insert(ins);
                    valReg = getMaxReg();
                    insert(createStoreInst(dt, valReg, getRegOperand(ptrReg)));
                }
            }
            else if (decl->init && !decl->init->singleInit)
            {
                // 处理数组初始化列表
                // 1. 先使用 memset 清零
                int totalElements = 1;
                for (int d : attr.arrayDims) totalElements *= d;
                int sizeBytes = totalElements * 4; // 假设 int/float 均为 4 字节

                std::vector<std::pair<DataType, Operand*>> args;
                args.push_back({DataType::PTR, getRegOperand(ptrReg)});
                args.push_back({DataType::I8, getImmeI32Operand(0)});
                args.push_back({DataType::I32, getImmeI32Operand(sizeBytes)});
                args.push_back({DataType::I1, getImmeI32Operand(0)});
                
                insert(new CallInst(DataType::VOID, "llvm.memset.p0.i32", args));

                // 2. 遍历初始化列表，对非零元素进行赋值
                FE::AST::InitializerList* initList = dynamic_cast<FE::AST::InitializerList*>(decl->init);
                if (initList) {
                    int currentOffset = 0;
                    
                    // 递归函数：处理初始化列表
                    // dims: 当前层级及以下的维度信息
                    // dimIdx: 当前处理的维度索引
                    std::function<void(FE::AST::InitDecl*, int)> traverse = 
                        [&](FE::AST::InitDecl* d, int dimIdx) {
                        
                        // 计算当前维度的步长（元素个数）
                        int currentDimSize = 1;
                        for (size_t i = dimIdx; i < attr.arrayDims.size(); ++i) {
                            currentDimSize *= attr.arrayDims[i];
                        }

                        if (auto* il = dynamic_cast<FE::AST::InitializerList*>(d)) {
                            if (il->init_list) {
                                int startOffset = currentOffset;
                                for (auto* child : *il->init_list) {
                                    // 如果是嵌套的列表，进入下一维
                                    // 如果是标量，说明是当前维度的元素（可能是扁平化写法）
                                    if (dynamic_cast<FE::AST::InitializerList*>(child)) {
                                        traverse(child, dimIdx + 1);
                                    } else {
                                        traverse(child, dimIdx); // 标量保持在当前维度逻辑中处理
                                    }
                                }
                                
                                if (il->init_list->empty()) {
                                    // 空列表，跳过当前上下文对应的维度大小
                                    currentOffset += currentDimSize; 
                                } else {
                                    // 非空列表，我们需要检查是否对齐
                                    int endOffset = startOffset + currentDimSize;
                                    if (currentOffset < endOffset) {
                                        currentOffset = endOffset;
                                    }
                                }
                            }
                        } else if (auto* init = dynamic_cast<FE::AST::Initializer*>(d)) {
                            // 计算初始值
                            apply(*this, *init->init_val, m);
                            size_t valReg = getMaxReg();
                            
                            // 类型转换
                            DataType to = convert(node.type);
                            DataType from = convert(init->init_val->attr.val.value.type);
                            auto convs = createTypeConvertInst(from, to, valReg);
                            for (auto& ins : convs) insert(ins);
                            valReg = getMaxReg();
                            
                            // 优化：如果是常量 0，则跳过 store（因为已经 memset）
                            bool isZero = false;
                            if (init->init_val->attr.val.isConstexpr) {
                                if (init->init_val->attr.val.value.type->getBaseType() == FE::AST::Type_t::INT && init->init_val->attr.val.getInt() == 0) isZero = true;
                                else if (init->init_val->attr.val.value.type->getBaseType() == FE::AST::Type_t::FLOAT && init->init_val->attr.val.getFloat() == 0.0f) isZero = true;
                            }
                            
                            if (!isZero) {
                                size_t elemPtrReg = getNewRegId();
                                
                                // 计算元素地址
                                std::vector<Operand*> indices;
                                indices.push_back(getImmeI32Operand(0)); // 解引用指针
                                
                                int tempOffset = currentOffset;
                                for (size_t i = 0; i < attr.arrayDims.size(); ++i) {
                                    int subSize = 1;
                                    for (size_t j = i + 1; j < attr.arrayDims.size(); ++j) {
                                        subSize *= attr.arrayDims[j];
                                    }
                                    int idx = tempOffset / subSize;
                                    tempOffset %= subSize;
                                    indices.push_back(getImmeI32Operand(idx));
                                }
                                
                                insert(new GEPInst(dt, DataType::I32, getRegOperand(ptrReg), getRegOperand(elemPtrReg), attr.arrayDims, indices));
                                insert(createStoreInst(dt, valReg, getRegOperand(elemPtrReg)));
                            }
                            currentOffset++;
                        }
                    };
                    
                    traverse(initList, 0);
                }
            }
        }
    }
}  // namespace ME
