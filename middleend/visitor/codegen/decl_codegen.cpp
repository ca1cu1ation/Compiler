#include <middleend/visitor/codegen/ast_codegen.h>
#include <debug.h>

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
            // 先计算初始化表达式（如果有），以避免后续 alloca 改变寄存器编号顺序
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
                }
            }

            // 为该变量分配指针寄存器
            size_t ptrReg = getNewRegId();
            if (lval->indices && !lval->indices->empty())
            {
                std::vector<int> dims;
                insert(new AllocaInst(dt, getRegOperand(ptrReg), dims));
            }
            else
            {
                insert(new AllocaInst(dt, getRegOperand(ptrReg)));
            }

            name2reg.addSymbol(entry, ptrReg);
            FE::AST::VarAttr attr;
            attr.type = node.type;
            reg2attr[ptrReg] = attr;

            // 如果之前计算了初始化值，则将其存入新分配的地址
            if (valReg != static_cast<size_t>(-1))
            {
                insert(createStoreInst(dt, valReg, getRegOperand(ptrReg)));
            }
        }
    }
}  // namespace ME
