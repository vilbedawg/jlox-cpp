#include "../include/Parser.hpp"
#define void_cast(x) (static_cast<void>(x))

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
    if (match({TokenType::FOR}))
        return forStatement();
    if (match({TokenType::IF}))
        return ifStatement();
    if (match({TokenType::PRINT}))
        return printStatement();
    if (match({TokenType::RETURN}))
        return returnStatement();
    if (match({TokenType::WHILE}))
        return whileStatement();
    if (match({TokenType::LEFT_BRACE}))
        return std::make_unique<BlockStmt>(block());
    if (match({TokenType::BREAK, TokenType::CONTINUE}))
        return controlStatement();

    return expressionStatement();
}

unique_stmt_ptr Parser::controlStatement()
{
    auto token = previous();
    unique_stmt_ptr stmt;
    if (token.type == TokenType::BREAK)
    {
        void_cast(consume(TokenType::SEMICOLON, "Expect ';' after break."));
        stmt = std::make_unique<BreakStmt>(std::move(token));
    }

    else if (token.type == TokenType::CONTINUE)
    {
        void_cast(consume(TokenType::SEMICOLON, "Expect ';' after continue)."));
        stmt = std::make_unique<ContinueStmt>(std::move(token));
    }

    return stmt;
}

unique_stmt_ptr Parser::forInitializer()
{
    if (match({TokenType::SEMICOLON}))
    {
        return nullptr;
    }
    else if (match({TokenType::VAR}))
    {
        return varDeclaration();
    }
    else
    {
        return expressionStatement();
    }
}

unique_expr_ptr Parser::forExpression(TokenType type, std::string msg)
{
    unique_expr_ptr expr;
    if (!check(type))
    {
        expr = expression();
    }

    void_cast(consume(type, std::move(msg)));

    return expr;
}

unique_stmt_ptr Parser::forStatement()
{
    void_cast(consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'."));
    auto initializer = forInitializer();
    auto condition = forExpression(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    auto increment = forExpression(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    auto body = statement();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                     std::move(increment), std::move(body));
}

unique_stmt_ptr Parser::ifStatement()
{
    void_cast(consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'."));
    auto if_condition = expression();
    void_cast(consume(TokenType::RIGHT_PAREN, "Expect ')' after 'if condition."));
    auto then_statement = statement();

    IfBranch main_branch{std::move(if_condition), std::move(then_statement)};
    std::vector<IfBranch> elif_branches;

    while (match({TokenType::ELIF}))
    {
        void_cast(consume(TokenType::LEFT_PAREN, "Expect '(' after 'elif'."));
        auto elif_condition = expression();
        void_cast(consume(TokenType::RIGHT_PAREN, "Expect ')' after 'elif'."));
        auto elif_statement = statement();
        elif_branches.emplace_back(std::move(elif_condition), std::move(elif_statement));
    }

    unique_stmt_ptr else_branch;
    if (match({TokenType::ELSE}))
    {
        else_branch = statement();
    }

    return std::make_unique<IfStmt>(std::move(main_branch), std::move(elif_branches),
                                    std::move(else_branch));
}

unique_stmt_ptr Parser::declaration()
{
    try
    {
        if (match({TokenType::VAR}))
            return varDeclaration();
        if (match({TokenType::FN}))
            return function("function");

        return statement();
    }
    catch (ParseError&)
    {
        synchronize();
        return nullptr;
    }
}

unique_stmt_ptr Parser::printStatement()
{
    auto identifier = previous();

    if (!match({TokenType::LEFT_PAREN}))
    {
        throw error(identifier, "Expect '(' after 'print'.");
    }

    auto expr = finishCall(std::make_unique<VarExpr>(std::move(identifier)));
    void_cast(consume(TokenType::SEMICOLON, "Expect ';' after print statement."));

    return std::make_unique<PrintStmt>(std::move(expr));
}

unique_stmt_ptr Parser::returnStatement()
{
    auto keyword = previous();
    unique_expr_ptr value;
    if (!check(TokenType::SEMICOLON))
    {
        value = expression();
    }

    void_cast(consume(TokenType::SEMICOLON, "Expect ';' after return value."));

    return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
}

unique_stmt_ptr Parser::varDeclaration()
{
    auto identifier = consume(TokenType::IDENTIFIER, "Expect variable name.");
    auto initializer = match({TokenType::EQUAL}) ? expression() : nullptr;

    void_cast(consume(TokenType::SEMICOLON, "Expect ';' after variable declaration."));

    return std::make_unique<VarStmt>(std::move(identifier), std::move(initializer));
}

unique_stmt_ptr Parser::whileStatement()
{
    void_cast(consume(TokenType::LEFT_PAREN, "Expect a '(' after 'while'."));
    auto condition = expression();
    void_cast(consume(TokenType::RIGHT_PAREN, "Expect a ')' after 'while'."));
    auto body = statement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

unique_stmt_ptr Parser::expressionStatement()
{
    auto expr = expression();
    void_cast(consume(TokenType::SEMICOLON, "Expect ';' after value."));

    return std::make_unique<ExprStmt>(std::move(expr));
}

unique_stmt_ptr Parser::function(const std::string& kind)
{
    auto identifier = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    void_cast(consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name."));
    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (params.size() > 254)
            {
                error(peek(), "Can't exceed more than 254 parameters.");
            }

            auto parameter = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            params.emplace_back(parameter);
        } while (match({TokenType::COMMA}));
    }

    void_cast(consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters."));
    void_cast(consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body."));

    auto body = block();

    return std::make_unique<FnStmt>(std::move(identifier), std::move(params), std::move(body));
}

std::vector<unique_stmt_ptr> Parser::block()
{
    std::vector<unique_stmt_ptr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements.emplace_back(declaration());
    }

    void_cast(consume(TokenType::RIGHT_BRACE, "Expect '}' after block."));

    return statements;
}

unique_expr_ptr Parser::assignment()
{
    auto expr = lambda();

    if (match({TokenType::EQUAL}))
    {
        auto value = assignment();

        // Check if the left-hand side of the assign expression is a regular identifier.
        if (dynamic_cast<VarExpr*>(expr.get()))
        {
            return std::make_unique<AssignExpr>(
                std::move(dynamic_cast<VarExpr*>(expr.release())->identifier), std::move(value));
        }

        // Check if the left-hand side expression is a subscript expression.
        if (auto subscript_ptr = dynamic_cast<SubscriptExpr*>(expr.release()))
        {
            return std::make_unique<SubscriptExpr>(std::move(subscript_ptr->identifier),
                                                   std::move(subscript_ptr->index),
                                                   std::move(value));
        }

        // Otherwise throw error.
        throw error(previous(), "Invalid assignment target.");
    }

    return expr;
}

unique_expr_ptr Parser::lambda()
{
    // Todo;
    auto expr = orExpression();

    return expr;
}

unique_expr_ptr Parser::orExpression()
{
    auto expr = andExpression();

    while (match({TokenType::OR}))
    {
        auto op = previous();
        auto right = andExpression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::andExpression()
{
    auto expr = equality();

    while (match({TokenType::AND}))
    {
        auto op = previous();
        auto right = andExpression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::expression()
{
    return assignment();
}

template <typename Fn>
unique_expr_ptr Parser::binary(Fn func, const std::initializer_list<TokenType>& token_args)
{
    auto expr = func();

    while (match(token_args))
    {
        auto op = previous();
        auto right = func();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::equality()
{
    return binary([this]() { return comparison(); },
                  {TokenType::EXCLAMATION_EQUAL, TokenType::EQUAL_EQUAL});
}

unique_expr_ptr Parser::comparison()
{
    return binary([this]() { return term(); }, {TokenType::GREATER, TokenType::GREATER_EQUAL,
                                                TokenType::LESS, TokenType::LESS_EQUAL});
}

unique_expr_ptr Parser::term()
{
    return binary([this]() { return factor(); }, {TokenType::MINUS, TokenType::PLUS});
}

unique_expr_ptr Parser::factor()
{
    return binary([this]() { return unary(); }, {TokenType::SLASH, TokenType::STAR});
}

unique_expr_ptr Parser::unary()
{
    if (match({TokenType::EXCLAMATION, TokenType::MINUS}))
    {
        auto op = previous();
        auto right = unary();

        return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
    }

    return prefix();
}

unique_expr_ptr Parser::prefix()
{
    if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
    {
        const auto op = previous();
        auto lvalue = consume(TokenType::IDENTIFIER,
                              "Operators '++' and '--' must be applied to and lvalue operand.");

        if (lvalue.type == TokenType::PLUS_PLUS || lvalue.type == TokenType::MINUS_MINUS)
        {
            throw error(peek(), "Operators '++' and '--' cannot be concatenated.");
        }

        if (op.type == TokenType::PLUS_PLUS)
        {
            return std::make_unique<IncrementExpr>(std::move(lvalue), IncrementExpr::Type::PREFIX);
        }
        else
        {
            return std::make_unique<DecrementExpr>(std::move(lvalue), DecrementExpr::Type::PREFIX);
        }
    }

    return postfix();
}

unique_expr_ptr Parser::postfix()
{
    auto expr = call();

    if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
    {
        const auto op = previous();
        // Forbid incrementing/decrementing rvalues.
        if (!dynamic_cast<VarExpr*>(expr.get()))
        {
            throw error(op, "Operators '++' and '--' must be applied to and lvalue operand.");
        }

        // Concatenating increment/decrement operators is not allowed.
        if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
        {
            throw error(op, "Operators '++' and '--' cannot be concatenated.");
        }

        if (op.type == TokenType::PLUS_PLUS)
        {
            expr = std::make_unique<IncrementExpr>(
                dynamic_cast<VarExpr*>(expr.release())->identifier, IncrementExpr::Type::POSTFIX);
        }
        else
        {
            expr = std::make_unique<DecrementExpr>(
                dynamic_cast<VarExpr*>(expr.release())->identifier, DecrementExpr::Type::POSTFIX);
        }
    }

    return expr;
}

unique_expr_ptr Parser::finishCall(unique_expr_ptr callee)
{
    std::vector<unique_expr_ptr> arguments;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (arguments.size() > 254)
            {
                error(peek(), "Argument limit exceeded. Can't have more than 254 arguments.");
            }

            arguments.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    auto paren = consume(TokenType::RIGHT_PAREN, "Except ')' after arguments.");

    return std::make_unique<CallExpr>(std::move(callee), std::move(paren), std::move(arguments));
}

unique_expr_ptr Parser::call()
{
    auto expr = subscript();

    while (true)
    {
        if (match({TokenType::LEFT_PAREN}))
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

unique_expr_ptr Parser::finishSubscript(unique_expr_ptr identifier)
{
    auto index = orExpression();
    void_cast(consume(TokenType::RIGHT_BRACKET, "Expect ']' after arguments."));

    // Forbid calling rvalues.
    if (!dynamic_cast<VarExpr*>(identifier.get()))
    {
        throw error(peek(), "Object is not subscriptable.");
    }

    auto var = dynamic_cast<VarExpr*>(identifier.release())->identifier;

    return std::make_unique<SubscriptExpr>(std::move(var), std::move(index), nullptr);
}

unique_expr_ptr Parser::subscript()
{
    auto expr = primary();

    while (true)
    {
        if (match({TokenType::LEFT_BRACKET}))
        {
            expr = finishSubscript(std::move(expr));
        }
        else
        {
            break;
        }
    }

    return expr;
}

std::vector<unique_expr_ptr> Parser::list()
{
    std::vector<unique_expr_ptr> items;
    // Return an empty list if there are no values.
    if (check(TokenType::RIGHT_BRACKET))
    {
        return items;
    }

    do
    {
        if (check(TokenType::RIGHT_BRACKET))
        {
            break;
        }

        items.emplace_back(orExpression());

        if (items.size() > 100)
        {
            error(peek(), "Cannot have more than 100 items in a list.");
        }

    } while (match({TokenType::COMMA}));

    return items;
}

unique_expr_ptr Parser::primary()
{
    using enum TokenType;

    if (match({NUMBER}))
    {
        return std::make_unique<LiteralExpr>(std::strtod(previous().lexeme.c_str(), nullptr));
    }

    if (match({STRING}))
    {
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    if (match({_FALSE}))
    {
        return std::make_unique<LiteralExpr>(false);
    }

    if (match({_TRUE}))
    {
        return std::make_unique<LiteralExpr>(true);
    }

    if (match({NIL}))
    {
        return std::make_unique<LiteralExpr>(std::any{});
    }

    if (match({IDENTIFIER}))
    {
        return std::make_unique<VarExpr>(previous());
    }

    if (match({LEFT_PAREN}))
    {
        auto expr = expression();
        void_cast(consume(RIGHT_PAREN, "Expect ')' after expression."));

        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (match({LEFT_BRACKET}))
    {
        auto opening_bracket = previous();
        auto expr = list();
        void_cast(consume(TokenType::RIGHT_BRACKET, "Expect ']' at the end of a list."));

        return std::make_unique<ListExpr>(std::move(opening_bracket), std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::match(const std::initializer_list<TokenType> token_args)
{
    // Check if any of the tokens match with the next token.
    bool is_match =
        std::any_of(token_args.begin(), token_args.end(), [this](TokenType x) { return check(x); });

    if (is_match)
    {
        advance();
    }

    return is_match;
}

Token Parser::consume(TokenType type, std::string msg)
{
    // Expects the type of the next token in the token sequence.
    if (!check(type))
    {
        // If not what expected, throw error.
        throw error(peek(), std::move(msg));
    }

    // Advance to the next token.
    advance();

    return previous();
}

bool Parser::check(TokenType type) const
{
    // Return false if all the tokens have been consumed.
    if (isAtEnd())
    {
        return false;
    }

    // Otherwise compare the next tokens type.
    return peek().type == type;
}

void Parser::advance()
{
    // Advance to the next token.
    if (!isAtEnd())
    {
        current++;
    }
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::_EOF;
}

const Token& Parser::peek() const
{
    return tokens[current];
}

Token Parser::previous() const
{
    return tokens[current - 1];
}

Parser::ParseError Parser::error(const Token& token, std::string msg) const
{
    Error::addError(token, std::move(msg));
    return {};
}

void Parser::synchronize()
{
    // The synchronize function is a method of error recovery for the parser.
    // It is called when a parse error occurs and its purpose is to bring the parser back into a
    // state where it can continue parsing. It does so by skipping tokens until it finds a token
    // that is likely to start a new statement or declaration.
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