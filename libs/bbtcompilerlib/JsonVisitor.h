#pragma once
#include "ASTVisitor.h"
#include "nlohmann/json.hpp"

namespace BBTCompiler
{
    using Json = nlohmann::json;
    class ASTJSonVisitor : public ASTConstVisitor {
    public:
        void visit(const AssignmentExpr& expr) override;

        void visit(const BinaryExpr& expr) override;

        void visit(const UnaryExpr& expr) override;

        void visit(const LiteralExpr& expr) override;

        void visit(const GroupedExpr& expr) override;

        void visit(const VariableExpr& expr) override;

        void visit(const CallExpr& expr) override;

        void visit(const PrintStmt& stmt) override;

        void visit(const ExprStmt& stmt) override;

        void visit(const VariableStmt& stmt) override;

        void visit(const BlockStmt& stmt) override;

        void visit(const IfStmt& stmt) override;

        void visit(const WhileStmt& stmt) override;

        void visit(const FuncStmt& stmt) override;

        const Json& getJson() const;

        void print();
        std::string toString();
    private:
        Json& getCurrentJson() {return *m_CurrentJson; };
        Json& addNestedJson(const std::string& name);
        Json& addNestedJsonArray(const std::string& name);
        void setCurrentJson(Json& data) { m_CurrentJson = &data; }
    private:
        Json m_Json{};
        Json* m_CurrentJson{ &m_Json };
    };
}