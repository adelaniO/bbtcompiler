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
        size_t getLevel() const { return m_Level; }
        void setLevel(size_t level) { m_Level = level; }
        virtual void accept(ASTConstVisitor& visitor) const = 0;
    private:
        size_t m_Level{};
    };

    class AssignmentExpr : public Expr
    {
    public:
        AssignmentExpr(Token name, Expr* value)
            : m_Name{ name }, m_Value{ value }
        {
            m_Value->setLevel(getLevel()+1);
        }
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
        {
            m_Left->setLevel(getLevel()+1);
            m_Right->setLevel(getLevel()+1);
        }
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
        {
            m_Right->setLevel(getLevel()+1);
        }
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
        {
            m_Expression->setLevel(getLevel() + 1);
        }
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
    };
}