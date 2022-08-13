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
            j = json{ {"value", node.value } };
            if(m_includeTokenPosition)
            {
                j["line"] = node.position.line;
                j["column"] = node.position.column;
            }
        }
        else if(std::holds_alternative<AssignmentExpr>(astNode))
        {
            const auto& node{ std::get<AssignmentExpr>(astNode) };
            json exprjson;
            exprjson["type"] = "AssignmentExpr";
            exprjson["name"] = toJson(node.m_name);
            exprjson["value"] = toJson(node.m_value);
            j["expression"] = exprjson;
        }
        else if(std::holds_alternative<BinaryExpr>(astNode))
        {
            const auto& node{ std::get<BinaryExpr>(astNode) };
            json exprjson;
            exprjson["type"] = "BinaryExpr";
            exprjson["lhs"] = toJson(node.m_left);
            exprjson["operator"] = toJson(node.m_operator);
            exprjson["rhs"] = toJson(node.m_right);
            j["expression"] = exprjson;
        }
        else if(std::holds_alternative<UnaryExpr>(astNode))
        {
            const auto& node{ std::get<UnaryExpr>(astNode) };
            json exprjson;
            exprjson["type"] = "UnaryExpr";
            exprjson["operator"] = toJson(node.m_operator);
            exprjson["rhs"] = toJson(node.m_right);
            j["expression"] = exprjson;
        }
        else if(std::holds_alternative<GroupedExpr>(astNode))
        {
            const auto& node{ std::get<GroupedExpr>(astNode) };
            json exprjson;
            exprjson["type"] = "GroupedExpr";
            exprjson["expression"] = toJson(node.m_expression);
            j["expression"] = exprjson;
        }
        else if(std::holds_alternative<VariableExpr>(astNode))
        {
            const auto& node{ std::get<VariableExpr>(astNode) };
            json exprjson;
            exprjson["type"] = "VariableExpr";
            exprjson["name"] = toJson(node.m_name);
            exprjson["var_type"] = toJson(node.m_type);
            j["expression"] = exprjson;
        }
        else if(std::holds_alternative<CallExpr>(astNode))
        {
            const auto& node{ std::get<CallExpr>(astNode) };
            json exprjson;
            exprjson["type"] = "CallExpr";
            //exprjson["name"] = toJson(node.m_args);
            exprjson["callee"] = toJson(node.m_callee);
            exprjson["patenthesis"] = toJson(node.m_paren);
            j["expression"] = exprjson;
        }
        else if(std::holds_alternative<PrintStmt>(astNode))
        {
            const auto& node{ std::get<PrintStmt>(astNode) };
            json exprjson;
            exprjson["type"] = "PrintStmt";
            exprjson["expression"] = toJson(node.m_expression);
            j["expression"] = exprjson;
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
            json stmtjson;
            stmtjson["type"] = "VariableStmt";
            stmtjson["initializer"] = toJson(node.m_initializer);
            stmtjson["name"] = toJson(node.m_name);
            stmtjson["var_type"] = toJson(node.m_type);
            j["expression"] = stmtjson;
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