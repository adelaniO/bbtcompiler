#pragma once

namespace BBTCompiler
{
    class AssignmentExpr;
    class BinaryExpr;
    class UnaryExpr;
    class PrimaryExpr;
    class GroupedExpr;

    class ExprConstVisitorBase {
    public:
        virtual void visit(const AssignmentExpr* expr) = 0;
        virtual void visit(const BinaryExpr* expr) = 0;
        virtual void visit(const UnaryExpr* expr) = 0;
        virtual void visit(const PrimaryExpr* expr) = 0;
        virtual void visit(const GroupedExpr* expr) = 0;
    };
}