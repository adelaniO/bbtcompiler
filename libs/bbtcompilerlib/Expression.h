#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include "ExpressionVisitor.h"

namespace BBTCompiler
{
    class Expr
    {
    public:
        size_t getLevel() const { return m_Level; }
        void setLevel(size_t level) { m_Level = level; }
        virtual void accept(ExprConstVisitorBase& visitor) const = 0;
    private:
        size_t m_Level;
    };

    class AssignmentExpr : public Expr
    {
    public:
        AssignmentExpr(Token name, Expr* value)
            : m_Name{ name }, m_Value{ value }
        {
            m_Value->setLevel(getLevel()+1);
        }
        virtual void accept(ExprConstVisitorBase& visitor) const override
        {
            visitor.visit(*this);
        }
        Token m_Name;
        std::unique_ptr<Expr> m_Value;
    };

    class BinaryExpr : public Expr
    {
    public:
        BinaryExpr(Expr* left, Token op, Expr* right)
            : m_Left{ left }, m_Operator{ op }, m_Right{ right }
        {
            m_Left->setLevel(getLevel()+1);
            m_Right->setLevel(getLevel()+1);
        }
        virtual void accept(ExprConstVisitorBase& visitor) const override
        {
            visitor.visit(*this);
        }
        std::unique_ptr<Expr> m_Left, m_Right;
        Token m_Operator;
    };

    class UnaryExpr : public Expr
    {
    public:
        UnaryExpr(Token op, Expr* right)
            : m_Operator{ op }, m_Right{ right }
        {
            m_Right->setLevel(getLevel()+1);
        }
        virtual void accept(ExprConstVisitorBase& visitor) const override
        {
            visitor.visit(*this);
        }
        std::unique_ptr<Expr> m_Right;
        Token m_Operator;
    };

    class GroupedExpr : public Expr
    {
    public:
        GroupedExpr(Expr* expr)
            : m_Expression{ expr }
        {
            m_Expression->setLevel(getLevel() + 1);
        }
        virtual void accept(ExprConstVisitorBase& visitor) const override
        {
            visitor.visit(*this);
        }
        std::unique_ptr<Expr> m_Expression;
    };

    class PrimaryExpr : public Expr
    {
    public:
        PrimaryExpr(Token token)
            : m_Token{token}
        {}
        virtual void accept(ExprConstVisitorBase& visitor) const override
        {
            visitor.visit(*this);
        }
        Token m_Token;
    };

    class ASTJSonVisitor : public ExprConstVisitorBase {
        using json = nlohmann::json;
    public:
        void visit(const AssignmentExpr& expr) override
        {
        }
        void visit(const BinaryExpr& expr) override
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

        void visit(const UnaryExpr& expr) override
        {
            auto& exprJson = getCurrentJson();
            auto& rightExprJson = addNestedJson("rhs");
            exprJson["type"] = "UnaryExpression";
            exprJson["operator"] = expr.m_Operator.value;
            setCurrentJson(rightExprJson);
            expr.m_Right->accept(*this);
        }

        void visit(const PrimaryExpr& expr) override
        {
            auto& exprJson = getCurrentJson();
            exprJson["type"] = "PrimaryExpression";
            exprJson["value"] = expr.m_Token.value;
        }

        void visit(const GroupedExpr& expr) override
        {
            auto& exprJson = getCurrentJson();
            auto& groupJson = addNestedJson("expression");
            exprJson["type"] = "GroupedExpression";
            setCurrentJson(groupJson);
            expr.m_Expression->accept(*this);
        }

        const json& getJson() const { return m_Json; }
        json& getJson() { return m_Json; }

        void print()
        {
            std::cout << '\n' << m_Json.dump(4) << '\n';
        }
    private:
        json& getCurrentJson() {return *m_CurrentJson; };
        json& addNestedJson(const std::string& name)
        {
            (*m_CurrentJson)[name] = json({});
            return *(m_CurrentJson->find(name));
        }
        void setCurrentJson(json& data)
        {
            m_CurrentJson = &data;
        }
    private:
        json m_Json{};
        json* m_CurrentJson{ &m_Json };

    };
}