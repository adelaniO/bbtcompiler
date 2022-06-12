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
        PrintStmt(std::unique_ptr<Expr> expression)
            : m_Expression{std::move(expression)}
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
        VariableStmt(Token name, Token type, std::unique_ptr<Expr> initializer)
            : m_Name{name}, m_Type{type}, m_Initializer{std::move(initializer)}
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        Token m_Name, m_Type;
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

    class IfStmt : public Stmt
    {
    public:
        IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
            : m_Condition{std::move(condition)},m_ThenBranch{std::move(thenBranch)},m_ElseBranch{std::move(elseBranch)}
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        std::unique_ptr<Expr> m_Condition;
        std::unique_ptr<Stmt> m_ThenBranch, m_ElseBranch;
    };

    class WhileStmt : public Stmt
    {
    public:
        WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
            : m_Condition{std::move(condition)},m_Body{std::move(body)}
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        std::unique_ptr<Expr> m_Condition;
        std::unique_ptr<Stmt> m_Body;
    };

    class FuncStmt : public Stmt
    {
    public:
        FuncStmt(Token name, std::vector<std::pair<Token,Token>> params, std::vector<std::unique_ptr<Stmt>> body)
            : m_Name{ name }, m_Params{ std::move(params) }, m_Body{ std::move(body) }
        {}

        void accept(ASTJSonVisitor& visitor) const override { visitor.visit(*this); }
        Token m_Name;
        std::vector<std::pair<Token,Token>> m_Params;
        std::vector<std::unique_ptr<Stmt>> m_Body;
    };
}