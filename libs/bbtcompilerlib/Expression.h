#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include "ASTVisitor.h"
#include "Lexer.h"

namespace BBTCompiler
{
    class Expr
    {
    public:
        virtual ~Expr() = default;
        virtual void accept(ASTConstVisitor& visitor) const = 0;
    private:
    };

    class AssignmentExpr : public Expr
    {
    public:
        AssignmentExpr(Token name, Expr* value)
            : m_Name{ name }, m_Value{ value }
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
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
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
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
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
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
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
        {
            visitor.visit(*this);
        }
        std::unique_ptr<Expr> m_Expression;
    };

    class LiteralExpr : public Expr
    {
    public:
        LiteralExpr(Token token)
            : m_Token{token}
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
        {
            visitor.visit(*this);
        }
        Token m_Token;
    };

    class VariableExpr : public Expr
    {
    public:
        VariableExpr(Token name)
            : m_Name{name}
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
        {
            visitor.visit(*this);
        }
        Token m_Name;
        Token m_Type;
    };

    class CallExpr : public Expr
    {
    public:
        CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguements)
            : m_Callee{ std::move(callee) }, m_Paren{ paren }, m_Args{ std::move(arguements) }
        {}
        virtual void accept(ASTConstVisitor& visitor) const override
        {
            visitor.visit(*this);
        }
        Token m_Paren;
        std::vector<std::unique_ptr<Expr>> m_Args;
        std::unique_ptr<Expr> m_Callee;
    };
}