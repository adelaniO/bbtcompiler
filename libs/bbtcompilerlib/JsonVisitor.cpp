#include "JsonVisitor.h"
#include "Expression.h"
#include "Statement.h"

namespace BBTCompiler
{
    void ASTJSonVisitor::visit(const AssignmentExpr& expr)
    {
    }
    void ASTJSonVisitor::visit(const BinaryExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        auto& leftExprJson = addNestedJson("lhs");
        auto& rightExprJson = addNestedJson("rhs");
        exprJson["type"] = "BinaryExpression";
        exprJson["operator"] = expr.m_Operator.value;
        setCurrentJson(leftExprJson);
        expr.m_Left->accept(*this);
        setCurrentJson(rightExprJson);
        expr.m_Right->accept(*this);
    }
    void ASTJSonVisitor::visit(const UnaryExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        auto& rightExprJson = addNestedJson("rhs");
        exprJson["type"] = "UnaryExpression";
        exprJson["operator"] = expr.m_Operator.value;
        setCurrentJson(rightExprJson);
        expr.m_Right->accept(*this);
    }


    void ASTJSonVisitor::visit(const LiteralExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        exprJson["type"] = "PrimaryExpression";
        exprJson["value"] = expr.m_Token.value;
    }

    void ASTJSonVisitor::visit(const GroupedExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        auto& groupJson = addNestedJson("expression");
        exprJson["type"] = "GroupedExpression";
        setCurrentJson(groupJson);
        expr.m_Expression->accept(*this);
    }

    void ASTJSonVisitor::visit(const VariableExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        exprJson["type"] = "Variable";
        exprJson["value"] = expr.m_Name.value;
    }

    void ASTJSonVisitor::visit(const PrintStmt& stmt)
    {
        auto& exprJson = getCurrentJson();
        auto& expresson = addNestedJson("expression");
        exprJson["type"] = "PrintStatement";
        setCurrentJson(expresson);
        stmt.m_Expression->accept(*this);
    }

    void ASTJSonVisitor::visit(const ExprStmt& stmt)
    {
        auto& exprJson = getCurrentJson();
        auto& expresson = addNestedJson("expression");
        exprJson["type"] = "ExpressionStatement";
        setCurrentJson(expresson);
        stmt.m_Expression->accept(*this);
    }

    void ASTJSonVisitor::visit(const VariableStmt& stmt)
    {
        auto& exprJson = getCurrentJson();
        auto& expresson = addNestedJson("expression");
        exprJson["type"] = "VariableStatement";
        exprJson["name"] = stmt.m_Name.value;
        setCurrentJson(expresson);
        stmt.m_Initializer->accept(*this);
    }

    void ASTJSonVisitor::visit(const BlockStmt& stmt)
    {
        //auto& stmtJson = getCurrentJson();
        //auto& stmtJsonArray = addNestedJsonArray("statements");
        //stmtJson["type"] = "BlockStatement";
        //for (auto& statement : stmt.m_Statements)
        //{
        //    stmtJsonArray
        //        setCurrentJson(stmtJsonArray);
        //}
    }

    const Json& ASTJSonVisitor::getJson() const { return m_Json; }

    void ASTJSonVisitor::print()
    {
        std::cout << '\n' << m_Json.dump(4) << '\n';
    }
    Json& ASTJSonVisitor::addNestedJson(const std::string& name)
    {
        (*m_CurrentJson)[name] = Json({});
        return *(m_CurrentJson->find(name));
    }

    Json& ASTJSonVisitor::addNestedJsonArray(const std::string& name)
    {
        (*m_CurrentJson)[name] = Json::array();
        return *(m_CurrentJson->find(name));
    }
}