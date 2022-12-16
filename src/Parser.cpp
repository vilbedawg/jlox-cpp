#include "../include/Parser.hpp"

/**
 *  Each productions body roughly translates to the following
 *
 *  Grammar notation -> Code representation
 *  ----------------------------------------
 *  Terminal            Code to match and consume a token
 *  Nonterminal         Call to that rule's function
 *  |                   if or switch statement
 *  * or +              while or for loop
 *  ?                   if statement
 */

Parser::Parser(std::vector<Token> tokens) : tokens{std::move(tokens)}
{
}

std::vector<unique_stmt_ptr> Parser::parse()
{
    std::vector<unique_stmt_ptr> statements;
    while (!isAtEnd())
    {
        statements.emplace_back(declaration());
    }

    return statements;
}

unique_stmt_ptr Parser::statement()
{
    // statement -> exprStmt
    //             | forStmt
    //             | ifStmt
    //             | printStmt
    //             | returnStmt
    //             | whileStmt
    //             | block;
    if (match(TokenType::FOR))
        return forStatement();
    if (match(TokenType::IF))
        return ifStatement();
    if (match(TokenType::PRINT))
        return printStatement();
    if (match(TokenType::WHILE))
        return whileStatement();
    if (match(TokenType::LEFT_BRACE))
        return std::make_unique<BlockStmt>(block());

    return expressionStatement();
}

unique_stmt_ptr Parser::forInitializer()
{
    if (match(TokenType::SEMICOLON))
    {
        return nullptr;
    }
    else if (match(TokenType::VAR))
    {
        return varDeclaration();
    }
    else
    {
        return expressionStatement();
    }
}

unique_expr_ptr Parser::forCondition()
{
    unique_expr_ptr condition;
    if (!check(TokenType::SEMICOLON))
    {
        condition = expression();
    }
    expect(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    return condition;
}

unique_expr_ptr Parser::forIncrement()
{
    unique_expr_ptr increment;
    if (!check(TokenType::RIGHT_PAREN))
    {
        increment = expression();
    }
    expect(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    return increment;
}

unique_stmt_ptr Parser::forStatement()
{
    /**
     * forStmt ->   "for" "(" ( varDecl | exprStmt | ";" )
     *              expression? ";"
     *              expression? ")" statement ;
     */
    expect(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    auto initializer = forInitializer();
    auto condition = forCondition();
    auto increment = forIncrement();
    auto body = statement();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                     std::move(increment), std::move(body));
}

unique_stmt_ptr Parser::ifStatement()
{
    // ifStmt -> "if" "(" expression ")" statement ("elif" statement )? ( "else" statement )? ;
    expect(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto if_condition = expression();
    expect(TokenType::RIGHT_PAREN, "Expect ')' after 'if condition.");
    auto then_statement = statement();

    IfBranch main_branch{std::move(if_condition), std::move(then_statement)};
    std::vector<IfBranch> elif_branches;
    while (match(TokenType::ELIF))
    {
        expect(TokenType::LEFT_PAREN, "Expect '(' after 'elif'.");
        auto elif_condition = expression();
        expect(TokenType::RIGHT_PAREN, "Expect ')' after 'elif'.");
        auto elif_statement = statement();
        elif_branches.emplace_back(std::move(elif_condition), std::move(elif_statement));
    }

    unique_stmt_ptr else_branch;
    if (match(TokenType::ELSE))
    {
        else_branch = statement();
    }

    return std::make_unique<IfStmt>(std::move(main_branch), std::move(elif_branches),
                                    std::move(else_branch));
}

unique_stmt_ptr Parser::declaration()
{
    // declaration → varDecl | statement ;
    try
    {
        if (match(TokenType::VAR))
        {
            return varDeclaration();
        }

        return statement();
    }
    catch (const ParseError& error)
    {
        synchronize();
        return nullptr;
    }
}

unique_stmt_ptr Parser::printStatement()
{
    // printStmt -> "print" expression ";" ;
    auto value = expression();
    expect(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<PrintStmt>(std::move(value));
}

unique_stmt_ptr Parser::varDeclaration()
{
    // varDeclaration -> IDENTIFIER ("=" expression)? ";" ;
    expect(TokenType::IDENTIFIER, "Expect variable name.");
    auto identifier = previous();
    auto initializer = match(TokenType::EQUAL) ? expression() : nullptr;
    expect(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    return std::make_unique<VarStmt>(identifier, std::move(initializer));
}

unique_stmt_ptr Parser::whileStatement()
{
    // whileStmt -> "while" "(" expression ")" statement ;
    expect(TokenType::LEFT_PAREN, "Expect a '(' after 'while'.");
    auto condition = expression();
    expect(TokenType::RIGHT_PAREN, "Expect a ')' after 'while'.");
    auto body = statement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

unique_stmt_ptr Parser::expressionStatement()
{
    // exprStmt -> expression ";" ;
    auto expr = expression();
    expect(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<ExprStmt>(std::move(expr));
}

std::vector<unique_stmt_ptr> Parser::block()
{
    // block -> "{" declaration* "}" ;
    std::vector<unique_stmt_ptr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements.emplace_back(declaration());
    }

    expect(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return statements;
}

unique_expr_ptr Parser::assignment()
{
    // assignment → IDENTIFIER "=" assignment | logic_or ;
    auto expr = orExpression();

    if (match(TokenType::EQUAL))
    {
        if (dynamic_cast<VarExpr*>(expr.get()))
        {
            auto value = assignment();
            auto identifier{static_cast<VarExpr*>(expr.release())->identifier};

            return std::make_unique<AssignExpr>(identifier, std::move(value));
        }

        Error::addError(previous(), "Invalid assignment target.");
    }

    return expr;
}

unique_expr_ptr Parser::orExpression()
{
    // logic_or -> logic_and ( "or" logic_and )* ;
    auto expr = andExpression();

    while (match(TokenType::OR))
    {
        const auto& _operator = previous();
        auto right = andExpression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::andExpression()
{
    // logic_and -> equality ( "and" equality )* ;
    auto expr = equality();

    while (match(TokenType::AND))
    {
        const auto& _operator = previous();
        auto right = andExpression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::expression()
{
    // expression -> assignment ;
    return assignment();
}

unique_expr_ptr Parser::equality()
{
    // equality -> comparison ( ( "!=" | "==" ) comparison )* ;
    auto expr = comparison();

    while (match(TokenType::EXCLAMATION_EQUAL, TokenType::EQUAL_EQUAL))
    {
        const auto& _operator = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::comparison()
{
    // comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
    auto expr = term();

    while (
        match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL))
    {
        const auto& _operator = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::term()
{
    // term -> factor ( (  "-" | "+" ) factor )* ;
    auto expr = factor();

    while (match(TokenType::MINUS, TokenType::PLUS))
    {
        const auto& _operator = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::factor()
{
    // factor → unary ( ( "/" | "*" ) unary )* ;
    auto expr = unary();

    while (match(TokenType::SLASH, TokenType::STAR))
    {
        const auto& _operator = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::unary()
{
    // unary -> ( "!" | "-" ) unary | prefix ;
    if (match(TokenType::EXCLAMATION, TokenType::MINUS))
    {
        const auto& _operator = previous();
        auto right = unary();

        return std::make_unique<UnaryExpr>(_operator, std::move(right));
    }

    return prefix();
}

unique_expr_ptr Parser::prefix()
{
    // prefix -> ( "++" | "--" ) prefix | lvalue | postfix;
    if (match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS))
    {
        const auto& _operator = previous();
        auto right = prefix();
        if (dynamic_cast<IncrementExpr*>(right.get()) || dynamic_cast<DecrementExpr*>(right.get()))
        {
            throw error(peek(), "Operators '++' and '--' cannot be concatenated.");
        }

        if (!dynamic_cast<VarExpr*>(right.get()))
        {
            throw error(_operator,
                        "Operators '++' and '--' must be applied to and lvalue operand.");
        }

        std::unique_ptr<VarExpr> identifier{static_cast<VarExpr*>(right.release())};
        if (_operator.type == TokenType::PLUS_PLUS)
        {
            return std::make_unique<IncrementExpr>(std::move(identifier),
                                                   IncrementExpr::Type::PREFIX);
        }
        else
        {
            return std::make_unique<DecrementExpr>(std::move(identifier),
                                                   DecrementExpr::Type::PREFIX);
        }
    }

    return postfix();
}

unique_expr_ptr Parser::postfix()
{
    // postfix -> call ( ( "++" | "--" ) lvalue );
    auto expr = call();

    if (!match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS))
    {
        return expr;
    }

    const auto& _operator = previous();
    if (!dynamic_cast<VarExpr*>(expr.get())) // If lvalue doesnt exist
    {
        throw error(_operator, "Operators '++' and '--' must be applied to and lvalue operand.");
    }

    std::unique_ptr<VarExpr> identifier{static_cast<VarExpr*>(expr.release())};
    if (_operator.type == TokenType::PLUS_PLUS)
    {
        expr = std::make_unique<IncrementExpr>(std::move(identifier), IncrementExpr::Type::POSTFIX);
    }
    else
    {
        expr = std::make_unique<DecrementExpr>(std::move(identifier), DecrementExpr::Type::POSTFIX);
    }

    if (match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS))
    {
        throw error(peek(), "Operators '++' and '--' cannot be concatenated.");
    }

    return expr;
}

unique_expr_ptr Parser::call()
{
    // call -> primary ( "(" arguments? ")" )* ;
    auto expr = primary();

    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
        {
            expr = finishCall(std::move(expr));
        }
        else
        {
            break;
        }
    }

    return expr;
}

unique_expr_ptr Parser::finishCall(unique_expr_ptr callee)
{
}

// unique_expr_ptr Parser::list()
//{
//     return nullptr;
// }

unique_expr_ptr Parser::primary()
{
    using enum TokenType;
    /**
     * primary  -> LIST | NUMBER | STRING | "true" | "false" | "nil"
     *             IDENTIFIER |  "(" expression ")" ;
     */
    // if (match(TokenType::LEFT_BRACKET))
    // {
    //     return list();
    // }
    if (match(NUMBER))
    {
        return std::make_unique<LiteralExpr>(std::strtod(previous().lexeme.c_str(), nullptr));
    }

    if (match(STRING))
    {
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    if (match(_FALSE))
    {
        return std::make_unique<LiteralExpr>(false);
    }

    if (match(_TRUE))
    {
        return std::make_unique<LiteralExpr>(true);
    }

    if (match(NIL))
    {
        return std::make_unique<LiteralExpr>(std::any{});
    }

    if (match(IDENTIFIER))
    {
        return std::make_unique<VarExpr>(previous());
    }

    if (match(LEFT_PAREN))
    {
        auto expr = expression();
        expect(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

template <typename... Args>
bool Parser::match(Args... args)
{
    std::initializer_list<bool> results{check(args)...};
    bool is_match = std::ranges::any_of(results.begin(), results.end(), [](bool x) { return x; });
    if (is_match)
    {
        advance();
    }

    return is_match;
}

void Parser::expect(TokenType type, std::string msg)
{
    if (!check(type))
    {
        throw error(peek(), std::move(msg));
    }

    advance();
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
    {
        return false;
    }

    return peek().type == type;
}

void Parser::advance()
{
    if (!isAtEnd())
    {
        current++;
    }
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::_EOF;
}

Token& Parser::peek()
{
    return tokens[current];
}

Token& Parser::previous()
{
    return tokens[current - 1];
}

Parser::ParseError Parser::error(const Token& token, std::string msg) const
{
    Error::addError(token, std::move(msg));
    return ParseError();
}

void Parser::synchronize()
{
    advance();
    while (!isAtEnd())
    {
        using enum TokenType;
        if (previous().type == SEMICOLON)
            return;

        switch (peek().type)
        {
        case CLASS:
        case FN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
        case BREAK:
            return;
        default:
            advance();
        }
    }
}