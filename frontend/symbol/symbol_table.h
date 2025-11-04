#ifndef __FRONTEND_SYMBOL_SYMBOL_TABLE_H__
#define __FRONTEND_SYMBOL_SYMBOL_TABLE_H__

#include <frontend/symbol/isymbol_table.h>
#include <map>
#include <vector>

namespace FE::Sym
{
    class SymTable : public iSymTable<SymTable>
    {
        friend iSymTable<SymTable>;

      private:
        // 符号表结构：每个作用域是一个map，整个符号表是作用域的栈
        std::vector<std::map<Entry*, FE::AST::VarAttr*>> symbolScopes;
        int scopeDepth = 0;

        void reset_impl();

        void              addSymbol_impl(Entry* entry, FE::AST::VarAttr& attr);
        FE::AST::VarAttr* getSymbol_impl(Entry* entry);
        void              enterScope_impl();
        void              exitScope_impl();

        bool isGlobalScope_impl();
        int  getScopeDepth_impl();
    };
}  // namespace FE::Sym

#endif  // __FRONTEND_SYMBOL_SYMBOL_TABLE_H__
