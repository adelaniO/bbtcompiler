#include "JsonVisitor.h"
#include "Expression.h"
#include "Statement.h"

namespace BBTCompiler
{
    void ASTJSonVisitor::visit(const AssignmentExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        exprJson["type"] = "AssignmentExpression";
        exprJson["name"] = expr.m_Name.value;
        auto& valueExprJson = addNestedJson("value");
        setCurrentJson(valueExprJson);
        expr.m_Value->accept(*this);

    }

    void ASTJSonVisitor::visit(const BinaryExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        exprJson["type"] = "BinaryExpression";
        exprJson["operator"] = expr.m_Operator.value;
        auto& leftExprJson = addNestedJson("lhs");
        auto& rightExprJson = addNestedJson("rhs");
        setCurrentJson(leftExprJson);
        expr.m_Left->accept(*this);
        setCurrentJson(rightExprJson);
        expr.m_Right->accept(*this);
    }
    void ASTJSonVisitor::visit(const UnaryExpr& expr)
    {
        auto& exprJson = getCurrentJson();
        exprJson["type"] = "UnaryExpression";
        exprJson["operator"] = expr.m_Operator.value;
        auto& rightExprJson = addNestedJson("rhs");
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
        exprJson["type"] = "GroupedExpression";
        auto& groupJson = addNestedJson("expression");
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
        exprJson["type"] = "ExpressionStatement";
        auto& expresson = addNestedJson("expression");
        setCurrentJson(expresson);
        stmt.m_Expression->accept(*this);
    }

    void ASTJSonVisitor::visit(const VariableStmt& stmt)
    {
        auto& exprJson = getCurrentJson();
        exprJson["type"] = "VariableStatement";
        exprJson["name"] = stmt.m_Name.value;
        auto& expresson = addNestedJson("initializer");
        setCurrentJson(expresson);
        if(stmt.m_Initializer.get())
            stmt.m_Initializer->accept(*this);
    }

    void ASTJSonVisitor::visit(const BlockStmt& stmt)
    {
        auto& stmtJson = getCurrentJson();
        stmtJson["type"] = "BlockStatement";
        auto& stmtJsonArray = addNestedJsonArray("statements");
        for (auto& statement : stmt.m_Statements)
        {
            auto& element = stmtJsonArray.emplace_back(Json({}));
            setCurrentJson(element);
            statement->accept(*this);
        }
    }

    void ASTJSonVisitor::visit(const IfStmt& stmt)
    {
        auto& stmtJson = getCurrentJson();
        stmtJson["type"] = "IfStatement";
        auto& conditionExpr = addNestedJson("condition");
        auto& thenStmt = addNestedJson("then");
        auto& elseStmt = addNestedJson("else");
        setCurrentJson(conditionExpr);
        stmt.m_Condition->accept(*this);
        setCurrentJson(thenStmt);
        stmt.m_ThenBranch->accept(*this);
        setCurrentJson(elseStmt);
        if(stmt.m_ElseBranch)
            stmt.m_ElseBranch->accept(*this);
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