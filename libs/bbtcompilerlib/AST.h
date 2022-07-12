#pragma once

#include <variant>
#include <limits>
#include <vector>
#include <cassert>
#include <nlohmann/json.hpp>

#include "Token.h"

using namespace nlohmann;

namespace BBTCompiler
{
    constexpr size_t npos() { return std::numeric_limits<size_t>::max(); }
    /*--------------------------Expressions--------------------------------------*/
    struct AssignmentExpr
    {
        size_t m_name{ npos() };
        size_t m_value{ npos() };
    };

    struct BinaryExpr
    {
        size_t m_left{ npos() }, m_right{ npos() };
        size_t m_operator{ npos() };
    };

    struct UnaryExpr
    {
        size_t m_right{ npos() };
        size_t m_operator{ npos() };
    };

    struct GroupedExpr
    {
        size_t m_expression{ npos() };
    };

    struct VariableExpr
    {
        size_t m_name{ npos() };
        size_t m_type{ npos() };
    };

    struct CallExpr
    {
        size_t m_paren{ npos() };
        std::pair<size_t, size_t> m_args{ npos(), npos() };
        size_t m_callee{ npos() };
    };

    /*--------------------------Statements--------------------------------------*/
    struct PrintStmt
    {
        size_t m_expression{ npos() };
    };

    struct ExprStmt
    {
        size_t m_expression{ npos() };
    };

    struct VariableStmt
    {
        size_t m_name{ npos() }, m_type{ npos() };
        size_t m_initializer{ npos() };
    };

    struct CompoundStmt
    {
        size_t m_num_statements{};
    };

    struct IfStmt
    {
        size_t m_condition{ npos() };
        size_t m_then{ npos() }, m_else{ npos() };
    };

    struct WhileStmt
    {
        size_t m_condition{ npos() };
        size_t m_body{ npos() };
    };

    struct ForStmt
    {
        size_t m_initializer{ npos() };
        size_t m_condition{ npos() };
        size_t m_increment{ npos() };
        size_t m_body{ npos() };
    };

    struct FuncStmt
    {
        size_t m_name{ npos() }, m_return_type{ npos() };
        std::pair<size_t, size_t> m_params{ npos(), npos() };
        size_t m_body{ npos() };
    };

    struct ReturnStmt
    {
        size_t m_returnToken{ npos() };
        size_t m_value{ npos() };
    };

    using ASTVariant = std::variant<
        std::monostate,
        Token,
        AssignmentExpr,
        BinaryExpr,
        UnaryExpr,
        GroupedExpr,
        VariableExpr,
        CallExpr,
        PrintStmt,
        ExprStmt,
        VariableStmt,
        CompoundStmt,
        IfStmt,
        WhileStmt,
        ForStmt,
        FuncStmt,
        ReturnStmt
    >;

    class ASTNode
    {
    public:
        ASTNode(size_t id) : m_id{ id }
        {}
        template<typename Node_t>
        ASTNode(size_t id, const Node_t node) : m_id{ id }, m_node{ node }
        {}
        template<typename Node_t> 
        Node_t& get() { return std::get<Node_t>(m_node); }
        template<typename Node_t> 
        const Node_t& get() const { return std::get<Node_t>(m_node); }
        const ASTVariant& getVariant() const { return m_node; }
        template<typename Node_t>
        Node_t* getIf()
        {
            return std::holds_alternative<Node_t>(m_node) ? &std::get<Node_t>(m_node) : nullptr;
        }

        size_t const getId() { return m_id; }
        void setNext(size_t id) { m_next = id; }
        bool hasNext() const { return m_next < npos(); }
        const ASTNode& nextNode(const std::vector<ASTNode>& tree) const { return tree[m_next]; }
        ASTNode& nextNode(std::vector<ASTNode>& tree) const { return tree[m_next]; }
        size_t next() const { return m_next; }
        void swapNodes(ASTNode& other) { m_node.swap(other.m_node); }

    private:
        size_t m_id{ npos() };
        size_t m_next{ npos() };
        ASTVariant m_node;
    };

    struct AST
    {
        ASTNode& createNode()
        {
            const size_t id{ m_nodes.size() };
            return m_nodes.emplace_back(id);
        }
        template<typename Node_t>
        ASTNode& createNode()
        {
            const size_t id{ m_nodes.size() };
            return m_nodes.emplace_back(id, Node_t{});
        }
        template<typename Node_t>
        ASTNode& createNode(const Node_t& node)
        {
            const size_t id{ m_nodes.size() };
            return m_nodes.emplace_back(id, node);
        }

        template<typename Node_t>
        Node_t& getNode(const size_t id)
        {
            assert(id < m_nodes.size());
            return m_nodes[id].get<Node_t>();
        }

        template<typename Node_t>
        Node_t* getNodeIf(const size_t id)
        {
            assert(id < m_nodes.size());
            return m_nodes[id].getIf<Node_t>();
        }
        void swapNodes(size_t node1, size_t node2)
        {
            assert(node1 < m_nodes.size() && node2 < m_nodes.size());
            m_nodes[node1].swapNodes(m_nodes[node2]);
        }

        size_t setNext(size_t currentId, size_t nextId)
        {
            m_nodes[currentId].setNext(nextId);
            return nextId;
        }

        size_t size() { return m_nodes.size(); }
        ASTNode& operator[](size_t id) { return m_nodes[id]; }
        const ASTNode& operator[](size_t id) const { return m_nodes[id]; }
    private:
        std::vector<ASTNode> m_nodes;
    };
}