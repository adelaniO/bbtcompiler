#pragma once

#include "ASTVisitor.h"
#include "JsonVisitor.h"

namespace BBTCompiler
{
    class Stmt
    {
    public:
        virtual ~Stmt() = default;
        virtual void accept(ASTJSonVisitor& visitor) const = 0;
    private:
    };

    class PrintStmt : public Stmt
    {
    public:
        PrintStmt(Expr* expression)
            : m_Expression{expression}
        {}
        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        std::unique_ptr<Expr> m_Expression;
    };

    class ExprStmt : public Stmt
    {
    public:
        ExprStmt(Expr* expression)
            : m_Expression{expression}
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        std::unique_ptr<Expr> m_Expression;
    };

    class VariableStmt : public Stmt
    {
    public:
        VariableStmt(Token name, Expr* initializer)
            : m_Name{name}, m_Initializer{initializer}
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        Token m_Name;
        std::unique_ptr<Expr> m_Initializer;
    };

    class BlockStmt : public Stmt
    {
    public:
        BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
            : m_Statements{std::move(statements)}
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        std::vector<std::unique_ptr<Stmt>> m_Statements;
    };
}