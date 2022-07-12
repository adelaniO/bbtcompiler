#include "Parser.h"
#include <iterator>
#include <iostream>
#include <sstream>

namespace BBTCompiler
{
    Token& Parser::advance()
    {
        if(!isAtEnd())
            std::advance(m_Current, 1);
        return previous();
    }

    Token& Parser::previous()
    {
        return m_Current != m_Tokens.begin() ? *std::prev(m_Current) : *m_Current;
    }

    Token& Parser::peek()
    {
        return *m_Current;
    }

    Token& Parser::consume(TokenType type, const std::string& errorMsg)
    {
        if(check(type))
            return advance();
        throw std::runtime_error(syntaxErrorMsg("file", peek(), errorMsg));
    }

    bool Parser::isAtEnd()
    {
        return m_Current->type == TokenType::END;
    }

    bool Parser::check(TokenType type)
    {
        return isAtEnd() ? false : peek().type == type;
    }

    bool Parser::check(const std::vector<TokenType>& types)
    {
        for(const auto& type : types)
            if(peek().type == type) return true;
        return false;
    }

    bool Parser::match(const TokenType& type)
    {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::match(const std::vector<TokenType>& types)
    {
        for (TokenType type : types)
        {
            if (match(type))
                return true;
        }
        return false;
    }

    AST& Parser::parse(std::string_view file)
    {
        return parse(std::stringstream(std::string(file)));
    }

    AST& Parser::parse(std::istream& file)
    {
        lexer.scan(file);
        m_Tokens = std::move(lexer.getTokens());
        m_Current = m_Tokens.begin();
        lexer.reset();

        ASTNode& translationUnit = m_Tree.createNode();
        size_t currentId{ translationUnit.getId() };
        while(!isAtEnd())
        {
            const auto next = parseDeclaration();
            currentId = m_Tree.setNext(currentId, next);
        }
        return m_Tree;
    }

    size_t Parser::parseBlock()
    {
        ASTNode& blockStmt = m_Tree.createNode<CompoundStmt>();
        size_t currentId{ blockStmt.getId() };
        size_t numStatements{};
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            const auto next = parseDeclaration();
            currentId = m_Tree.setNext(currentId, next);
            ++numStatements;
        }
        
        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        CompoundStmt& statement{ blockStmt.get<CompoundStmt>() };
        statement.m_num_statements = numStatements;
        return blockStmt.getId();
    }

    size_t Parser::parseDeclaration()
    {
        try
        {
            if(match(TokenType::FN)) return parseFunctionStatement("function");
            if(match(TokenType::LET)) return parseVariableDeclaration();
            return parseStatement();
        }
        catch (const std::exception& e)
        {
            synchronize();
            std::cerr << e.what();
        }
        return npos();
    }

    std::pair<size_t, size_t> Parser::parseNewVariable()
    {
        const Token& identifier{ consume(TokenType::IDENTIFIER, "Expect variable name.") };
        const size_t idenId = m_Tree.createNode<Token>(identifier).getId();
        consume(TokenType::COLON, "Expect ': <variable_type>' after variable name.");
        const Token& type { parseType() };
        const size_t typeId = m_Tree.createNode<Token>(type).getId();

        return { idenId, typeId };
    }

    Token& Parser::parseType()
    {
        if (check({ TokenType::INT, TokenType::CHAR, TokenType::BOOL, TokenType::FLOAT }))
            return advance();
        else if (check(TokenType::IDENTIFIER))
            return advance();
        else
            throw std::runtime_error(syntaxErrorMsg("file", peek(), "Expect '<variable type>'."));
    }
    
    size_t Parser::parseVariableDeclaration()
    {
        const size_t varDeclId{ m_Tree.createNode<VariableStmt>().getId() };
        const auto [name, type] = parseNewVariable();
        size_t initializer{ match(TokenType::EQ) ? parseExpression() : npos() };

        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        auto& varDecl{ m_Tree.getNode<VariableStmt>(varDeclId) };
        varDecl.m_name = name;
        varDecl.m_type = type;
        varDecl.m_initializer = initializer;
        return varDeclId;
    }

    size_t Parser::parseStatement()
    {
        if(match(TokenType::FOR)) return parseForStatement();
        if(match(TokenType::IF)) return parseIfStatement();
        if(match(TokenType::PRINT)) return parsePrintStatement();
        if(match(TokenType::RETURN)) return parseReturnStatement();
        if(match(TokenType::WHILE)) return parseWhileStatement();
        if(match(TokenType::LEFT_BRACE)) return parseBlock();
        return parseExpressionStatement();
    }

    size_t Parser::parseExpressionStatement()
    {
        const size_t exprId{ parseExpression() };
        auto& node{ m_Tree.createNode<ExprStmt>() };
        auto& exprStmt{ node.get<ExprStmt>() };
        exprStmt.m_expression = exprId;
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return node.getId();
    }

    size_t Parser::parseFunctionStatement(const std::string& kind)
    {
        auto& node{ m_Tree.createNode<FuncStmt>() };
        auto& funcStmt{ node.get<FuncStmt>() };
        const Token& name{ consume(TokenType::IDENTIFIER, "Expect " + kind + "name.") };
        const size_t nameId{ m_Tree.createNode<Token>(name).getId() };
        consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
        size_t numParams{};
        if(!check(TokenType::RIGHT_PAREN))
        {
            do {
                parseNewVariable();
                ++numParams;
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
        size_t returnTypeId{ npos() };
        if(check(TokenType::RIGHT_ARROW))
        {
            advance();
            const Token& returnType = parseType();
            returnTypeId = m_Tree.createNode<Token>(returnType).getId();
        funcStmt.m_name = nameId;
        funcStmt.m_params.first = nameId + 1;
        funcStmt.m_params.second = numParams;
        funcStmt.m_return_type = returnTypeId;
        }
        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + "body.");
        auto bodyId{ parseBlock() };
        funcStmt.m_body = bodyId;
        return node.getId();
    }

    size_t Parser::parseReturnStatement()
    {
        auto& returnStmtNode{ m_Tree.createNode<ReturnStmt>() };
        auto& returnKeyNode{ m_Tree.createNode<Token>(previous()) };
        size_t value{ !check(TokenType::SEMICOLON) ? parseExpression() : npos() };

        consume(TokenType::SEMICOLON, "Expect ';' after return value.");
        auto& returnStmt{ returnStmtNode.get<ReturnStmt>() };
        returnStmt.m_returnToken = returnKeyNode.getId();
        returnStmt.m_value = value;
        return returnStmtNode.getId();
    }

    size_t Parser::parsePrintStatement()
    {
        size_t expression{ parseExpression() };
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return expression;
    }

    size_t Parser::parseForStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
        auto& forStmtNode{ m_Tree.createNode<ForStmt>() };
        auto& forStmt{ forStmtNode.get<ForStmt>() };
        size_t initializer;
        if(match(TokenType::SEMICOLON))
            initializer = npos();
        if(match(TokenType::LET))
            initializer = parseVariableDeclaration();
        else
            initializer = parseExpressionStatement();

        size_t condition{ npos() };
        if(!check(TokenType::SEMICOLON))
            condition = parseExpression();
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        size_t increment{ npos() };
        if(!check(TokenType::RIGHT_PAREN))
            increment = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        size_t body = parseStatement();
        forStmt.m_initializer = initializer;
        forStmt.m_condition = condition;
        forStmt.m_increment = increment;
        forStmt.m_body = body;
        //if(isValidNode(increment))
        //{
        //    std::vector<std::unique_ptr<Stmt>> innerBlock;
        //    innerBlock.push_back(std::move(body));
        //    innerBlock.push_back(std::make_unique<ExprStmt>(ExprStmt{ increment.release() }));
        //    body = std::make_unique<CompoundStmt>(std::move(innerBlock));
        //}

        //if(!condition) condition = std::make_unique<LiteralExpr>(LiteralExpr{ Token{TokenType::TRUE,{},"true"} });
        //body = std::make_unique<WhileStmt>(WhileStmt{ std::move(condition), std::move(body) });

        //if(initializer)
        //{
        //    std::vector<std::unique_ptr<Stmt>> innerBlock;
        //    innerBlock.push_back(std::move(initializer));
        //    innerBlock.push_back(std::move(body));
        //    body = std::make_unique<CompoundStmt>(std::move(innerBlock));
        //}
        return forStmtNode.getId();
    }

    size_t Parser::parseWhileStatement()
    {
        auto& whileStmtNode{ m_Tree.createNode<WhileStmt>() };
        auto& whileStmt{ whileStmtNode.get<WhileStmt>() };
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        const size_t condition{ parseExpression() };
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        const size_t body{parseStatement()};
        whileStmt.m_condition = condition;
        whileStmt.m_body = body;
        return whileStmtNode.getId();
    }

    size_t Parser::parseIfStatement()
    {
        auto& ifStmtNode{ m_Tree.createNode<IfStmt>() };
        auto& ifStmt{ ifStmtNode.get<IfStmt>() };
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        const size_t condition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

        auto thenBranch = parseStatement();
        const size_t elseBranch{ match(TokenType::ELSE) ? parseStatement() : npos() };
        ifStmt.m_condition = condition;
        ifStmt.m_then = thenBranch;
        ifStmt.m_else = elseBranch;
        return ifStmtNode.getId();
    }

    size_t Parser::parseExpression()
    {
        return parseAssignmentExpr();
    }

    size_t Parser::parseAssignmentExpr()
    {
        const size_t expr = parseOrExpr();
        if(match(TokenType::EQ))
        {
            Token equals = previous();
            auto value = parseAssignmentExpr();

            if(VariableExpr* varExpr{ m_Tree.getNodeIf<VariableExpr>(expr) } )
            {
                auto& assignNode{ m_Tree.createNode<AssignmentExpr>() };
                auto& assignStmt{ assignNode.get<AssignmentExpr>() };
                size_t name = varExpr->m_name;
                assignStmt.m_name = varExpr->m_name;
                assignStmt.m_value = value;
                return assignNode.getId();
            }

            throw std::runtime_error(syntaxErrorMsg("file", equals, "Invalid assignment target."));
        }
        return expr;
    }

    size_t Parser::parseOrExpr()
    {
        auto expr = parseAndExpr();
        while(match(TokenType::OR))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            auto right = parseAndExpr();
            BinaryExpr binExpr;
            binExpr.m_left = expr;
            binExpr.m_right = right;
            binExpr.m_operator = opId;
            expr = m_Tree.createNode<BinaryExpr>(binExpr).getId();
        }
        return expr;
    }

    size_t Parser::parseAndExpr()
    {
        auto expr = parseEqualityExpr();
        while(match(TokenType::AND))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            auto right = parseEqualityExpr();
            BinaryExpr binExpr;
            binExpr.m_left = expr;
            binExpr.m_right = right;
            binExpr.m_operator = opId;
            expr = m_Tree.createNode<BinaryExpr>(binExpr).getId();
        }
        return expr;
    }

    size_t Parser::parseEqualityExpr()
    {
        size_t expression{parseComparisonExpr()};
        while (match({ TokenType::NOT_EQ, TokenType::EQ_EQ }))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            size_t right{ parseComparisonExpr() };
            BinaryExpr binExpr;
            binExpr.m_left = expression;
            binExpr.m_right = right;
            binExpr.m_operator = opId;
            expression = m_Tree.createNode<BinaryExpr>(binExpr).getId();
        }
        return expression;
    }

    size_t Parser::parseComparisonExpr()
    {
        size_t expression{parseAdditiveExpr()};
        while (match({ TokenType::GREATER, TokenType::LESS,TokenType::GREATER_EQ, TokenType::LESS_EQ }))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            size_t right{ parseAdditiveExpr() };
            BinaryExpr binExpr;
            binExpr.m_left = expression;
            binExpr.m_right = right;
            binExpr.m_operator = opId;
            expression = m_Tree.createNode<BinaryExpr>(binExpr).getId();
        }
        return expression;
    }

    size_t Parser::parseAdditiveExpr()
    {
        size_t expression{parseMultiplicativeExpr()};
        while (match({ TokenType::MINUS, TokenType::PLUS }))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            size_t right{ parseMultiplicativeExpr() };
            BinaryExpr binExpr;
            binExpr.m_left = expression;
            binExpr.m_right = right;
            binExpr.m_operator = opId;
            expression = m_Tree.createNode<BinaryExpr>(binExpr).getId();
        }
        return expression;
    }

    size_t Parser::parseMultiplicativeExpr()
    {
        size_t expression{parseUnaryExpr()};
        while (match({ TokenType::SLASH, TokenType::STAR }))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            size_t right{ parseUnaryExpr() };
            BinaryExpr binExpr;
            binExpr.m_left = expression;
            binExpr.m_right = right;
            binExpr.m_operator = opId;
            expression = m_Tree.createNode<BinaryExpr>(binExpr).getId();
        }
        return expression;
    }

    size_t Parser::parseUnaryExpr()
    {
        if(match({TokenType::NOT, TokenType::MINUS}))
        {
            const size_t opId{ m_Tree.createNode<Token>(previous()).getId() };
            size_t right{ parseUnaryExpr() };
            UnaryExpr unaryExpr;
            unaryExpr.m_operator = opId;
            unaryExpr.m_right = right;
            return m_Tree.createNode<UnaryExpr>(unaryExpr).getId();
        }

        return parseCallExpr();
    }

    size_t Parser::parseCallExpr()
    {
        auto expr{ parsePrimaryExpr() };

        while(true)
        {
            if(match(TokenType::LEFT_PAREN))
                expr = finishCall(std::move(expr));
            else
                break;
        }
        return expr;
    }

    size_t Parser::parsePrimaryExpr()
    {
        if (match({
                TokenType::TRUE, TokenType::FALSE, TokenType::NIL, TokenType::INT_LITERAL,
                TokenType::FLOAT_LITERAL, TokenType::STRING_LITERAL
            }))
        {
            return m_Tree.createNode<Token>(previous()).getId();
        }

        if (match(TokenType::IDENTIFIER))
        {
            return m_Tree.createNode<Token>(previous()).getId();
            //return std::make_unique<VariableExpr>(VariableExpr(previous()));
        }

        if (match(TokenType::LEFT_PAREN))
        {
            size_t expr = parseExpression();
            consume(TokenType::RIGHT_PAREN, "expected ')' after expression.");
            return m_Tree.createNode<GroupedExpr>(GroupedExpr{expr}).getId();
        }

        throw std::runtime_error(syntaxErrorMsg("file", peek(), "expected expression."));
    }

    size_t Parser::finishCall(size_t callee)
    {
        auto& node{ m_Tree.createNode<CallExpr>() };
        const size_t callExprId{ node.getId() };
        auto& callExpr{ node.get<CallExpr>() };
        size_t numArgs{};
        if(!check(TokenType::RIGHT_PAREN))
        {
            do {
                parseExpression();
                ++numArgs;
            } while (match(TokenType::COMMA));
        }

        Token paren = consume(TokenType::RIGHT_PAREN, "Expect '(' after arguments");
        const size_t parenId{ m_Tree.createNode<Token>().getId() };
        callExpr.m_callee = callee;
        callExpr.m_paren = parenId;
        callExpr.m_args.first = callExprId + 1;
        callExpr.m_args.second = numArgs;
        return callExprId;
    }

    std::string Parser::syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg)
    {
        return "<"+filename+">:" +
                std::to_string(token.position.line) + ":" + std::to_string(token.position.column) +
                ": syntax error: " + msg;
    }

    void Parser::synchronize()
    {
        advance();
        while(!isAtEnd())
        {
            if(previous().type == TokenType::SEMICOLON)
                return;
            
            switch (peek().type)
            {
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            }

            advance();
        }
    }
}