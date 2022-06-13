#pragma once

namespace BBTCompiler
{
    class AssignmentExpr;
    class BinaryExpr;
    class UnaryExpr;
    class LiteralExpr;
    class GroupedExpr;
    class VariableExpr;
    class CallExpr;
    class ExprStmt;
    class PrintStmt;
    class VariableStmt;
    class BlockStmt;
    class IfStmt;
    class WhileStmt;
    class FuncStmt;
    class ReturnStmt;

    class ASTConstVisitor {
    public:
        virtual void visit(const AssignmentExpr& expr) = 0;
        virtual void visit(const BinaryExpr& expr) = 0;
        virtual void visit(const UnaryExpr& expr) = 0;
        virtual void visit(const LiteralExpr& expr) = 0;
        virtual void visit(const GroupedExpr& expr) = 0;
        virtual void visit(const VariableExpr& expr) = 0;
        virtual void visit(const CallExpr& expr) = 0;
        virtual void visit(const ExprStmt& stmt) = 0;
        virtual void visit(const PrintStmt& stmt) = 0;
        virtual void visit(const VariableStmt& stmt) = 0;
        virtual void visit(const BlockStmt& stmt) = 0;
        virtual void visit(const IfStmt& stmt) = 0;
        virtual void visit(const WhileStmt& stmt) = 0;
        virtual void visit(const FuncStmt& stmt) = 0;
        virtual void visit(const ReturnStmt& stmt) = 0;
    };
}