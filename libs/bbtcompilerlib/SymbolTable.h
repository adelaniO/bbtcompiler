#pragma once

#include<vector>
#include<map>
#include<optional>

namespace BBTCompiler
{
    enum class SymbolType { VARIABLE, FUNCTION, TYPE, INVALID };
    class SymbolTable
    {
    public:
        struct Symbol
        {
            SymbolType type{ SymbolType::INVALID };

        };
        std::map<std::string, Symbol>& pushScope() { return m_symbols.emplace_back(); }
        void popScope() { m_symbols.pop_back(); }
        std::optional<Symbol> find(std::string_view symbol)
        {
            for(auto it{m_symbols.rbegin()}; it != m_symbols.rend(); ++it)
            {
                if(auto search{ it->find(symbol.data()) }; search != it->end())
                    return { search->second };
            }
            return std::nullopt;
        }
        Symbol& addSymbol(std::string_view name, SymbolType type)
        {
            auto result = m_symbols.back().insert_or_assign(name.data(), Symbol{type}).first;
            return result->second;
        }
    private:
        std::vector<std::map<std::string, Symbol>> m_symbols;
    };
}