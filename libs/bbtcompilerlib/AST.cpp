#include "AST.h"
#include "AST.h"

namespace BBTCompiler
{
    json AST::toJson(const size_t iNode) const
    {
        const ASTVariant& astNode{ m_nodes[iNode].getVariant() };
        json j;
        if(std::holds_alternative<Token>(astNode))
        {
            const auto& node{ std::get<Token>(astNode) };
            j = json{ {"type", "Token" }, {"value", node.value } };
            if(m_includeTokenPosition)
            {
                j["line"] = node.position.line;
                j["column"] = node.position.column;
            }
        }
        else if(std::holds_alternative<AssignmentExpr>(astNode))
        {
            const auto& node{ std::get<AssignmentExpr>(astNode) };
        }
        else if(std::holds_alternative<BinaryExpr>(astNode))
        {
            const auto& node{ std::get<BinaryExpr>(astNode) };
            node.m_operator;
            node.m_right;
            j = json{ {"type", "BinaryExpr"} };
            j["left"] = toJson(node.m_left);
            j["operator"] = toJson(node.m_operator);
            j["right"] = toJson(node.m_right);
        }
        else if(std::holds_alternative<UnaryExpr>(astNode))
        {
            const auto& node{ std::get<UnaryExpr>(astNode) };
        }
        else if(std::holds_alternative<GroupedExpr>(astNode))
        {
            const auto& node{ std::get<GroupedExpr>(astNode) };
        }
        else if(std::holds_alternative<VariableExpr>(astNode))
        {
            const auto& node{ std::get<VariableExpr>(astNode) };
        }
        else if(std::holds_alternative<CallExpr>(astNode))
        {
            const auto& node{ std::get<CallExpr>(astNode) };
        }
        else if(std::holds_alternative<PrintStmt>(astNode))
        {
            const auto& node{ std::get<PrintStmt>(astNode) };
        }
        else if(std::holds_alternative<ExprStmt>(astNode))
        {
            const auto& node{ std::get<ExprStmt>(astNode) };
            j = json::array();
            j.emplace_back(toJson(node.m_expression));
            nextNodeToJson(iNode, j);
        }
        else if(std::holds_alternative<VariableStmt>(astNode))
        {
            const auto& node{ std::get<VariableStmt>(astNode) };
        }
        else if(std::holds_alternative<CompoundStmt>(astNode))
        {
            const auto& node{ std::get<CompoundStmt>(astNode) };
        }
        else if(std::holds_alternative<IfStmt>(astNode))
        {
            const auto& node{ std::get<IfStmt>(astNode) };
        }
        else if(std::holds_alternative<WhileStmt>(astNode))
        {
            const auto& node{ std::get<WhileStmt>(astNode) };
        }
        else if(std::holds_alternative<ForStmt>(astNode))
        {
            const auto& node{ std::get<ForStmt>(astNode) };
        }
        else if(std::holds_alternative<FuncStmt>(astNode))
        {
            const auto& node{ std::get<FuncStmt>(astNode) };
        }
        else if(std::holds_alternative<ReturnStmt>(astNode))
        {
            const auto& node{ std::get<ReturnStmt>(astNode) };
        }
        else if(std::holds_alternative<std::monostate>(astNode))
        {
            const auto& node{ std::get<std::monostate>(astNode) };
            j = json::array();
            auto next{iNode};
            nextNodeToJson(iNode, j);
        }
        else {
            throw ASTException{ "Unimplimented Json function" };
        }
        return j;
    }

    void AST::nextNodeToJson(const size_t& iNode, nlohmann::json& j) const
    {
        auto next{ iNode };
        while (m_nodes[next].hasNext())
        {
            next = m_nodes[next].next();
            j.emplace_back(toJson(next));
        }
    }
}