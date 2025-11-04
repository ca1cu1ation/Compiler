#include <frontend/symbol/symbol_table.h>
#include <debug.h>
#include <stdexcept>

namespace FE::Sym
{
    void SymTable::reset_impl()
    {
        // 清理所有作用域中的符号属性
        for (auto& scope : symbolScopes) {
            for (auto& [entry, attr] : scope) {
                if (attr) {
                    delete attr;
                }
            }
        }
        
        symbolScopes.clear();
        scopeDepth = 0;
        // 添加全局作用域
        enterScope_impl();
    }

    void SymTable::enterScope_impl()
    {
        symbolScopes.push_back(std::map<Entry*, FE::AST::VarAttr*>());
        scopeDepth++;
    }

    void SymTable::exitScope_impl()
    {
        if (symbolScopes.empty()) {
            throw std::runtime_error("Cannot exit scope: no active scopes");
        }
        
        // 删除当前作用域中的所有符号属性
        for (auto& [entry, attr] : symbolScopes.back()) {
            if (attr) {
                delete attr;
            }
        }
        
        symbolScopes.pop_back();
        scopeDepth--;
        
        // 确保至少保留全局作用域
        if (symbolScopes.empty()) {
            enterScope_impl();
        }
    }

    void SymTable::addSymbol_impl(Entry* entry, FE::AST::VarAttr& attr)
    {
        if (symbolScopes.empty()) {
            throw std::runtime_error("No active scope to add symbol");
        }
        
        if (!entry) {
            throw std::runtime_error("Cannot add null symbol entry");
        }
        
        auto& currentScope = symbolScopes.back();
        
        // 检查当前作用域是否已存在该符号
        if (currentScope.find(entry) != currentScope.end()) {
            throw std::runtime_error("Symbol '" + entry->getName() + "' already defined in current scope");
        }
        
        // 创建新的VarAttr副本并存储
        FE::AST::VarAttr* newAttr = new FE::AST::VarAttr(attr);
        currentScope[entry] = newAttr;
    }

    FE::AST::VarAttr* SymTable::getSymbol_impl(Entry* entry)
    {
        if (!entry) {
            return nullptr;
        }
        
        // 从当前作用域开始向外查找（最近嵌套作用域规则）
        for (auto it = symbolScopes.rbegin(); it != symbolScopes.rend(); ++it) {
            auto found = it->find(entry);
            if (found != it->end()) {
                return found->second;
            }
        }
        
        return nullptr; // 符号未找到
    }

    bool SymTable::isGlobalScope_impl()
    {
        return scopeDepth == 1; // 全局作用域深度为1
    }

    int SymTable::getScopeDepth_impl()
    {
        return scopeDepth;
    }
}  // namespace FE::Sym
