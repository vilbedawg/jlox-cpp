#ifndef EXPR_HPP
#define EXPR_HPP

#include "Token.hpp"
#include "Typedef.hpp"
#include "Visitor.hpp"
#include <vector>

struct AssignExpr : Expr
{
    Token identifier;
    unique_expr_ptr value;

    AssignExpr(Token identifier, unique_expr_ptr value);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct BinaryExpr : Expr
{
    unique_expr_ptr left;
    Token op;
    unique_expr_ptr right;

    BinaryExpr(unique_expr_ptr left, Token op, unique_expr_ptr right);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct UnaryExpr : Expr
{
    Token op;
    unique_expr_ptr right;

    UnaryExpr(Token op, unique_expr_ptr right);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct IncrementExpr : Expr
{
    enum class Type
    {
        POSTFIX,
        PREFIX
    };

    Token identifier;
    Type type;

    IncrementExpr(Token identifier, Type type);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct DecrementExpr : Expr
{
    enum class Type
    {
        POSTFIX,
        PREFIX
    };

    Token identifier;
    Type type;

    DecrementExpr(Token identifier, Type type);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct CallExpr : Expr
{
    unique_expr_ptr callee;
    Token paren;
    std::vector<unique_expr_ptr> args;

    CallExpr(unique_expr_ptr callee, Token paren, std::vector<unique_expr_ptr> args);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct GetExpr : Expr
{
    unique_expr_ptr object;
    Token identifier;

    GetExpr(unique_expr_ptr object, Token identifier);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct SetExpr : Expr
{
    unique_expr_ptr object;
    Token identifier;
    unique_expr_ptr value;

    SetExpr(unique_expr_ptr object, Token identifier, unique_expr_ptr value);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct GroupingExpr : Expr
{
    unique_expr_ptr expression;

    explicit GroupingExpr(unique_expr_ptr expression);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct LiteralExpr : Expr
{
    std::any literal;

    explicit LiteralExpr(std::any literal);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct LogicalExpr : Expr
{
    unique_expr_ptr left;
    Token op;
    unique_expr_ptr right;

    LogicalExpr(unique_expr_ptr left, Token op, unique_expr_ptr right);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct SuperExpr : Expr
{
    Token keyword;
    Token method;

    SuperExpr(Token keyword, Token method);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct ThisExpr : Expr
{
    Token keyword;

    explicit ThisExpr(Token keyword);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct VarExpr : Expr
{
    Token identifier;

    explicit VarExpr(Token identifier);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct ListExpr : Expr
{
    Token opening_bracket;
    std::vector<unique_expr_ptr> items;

    ListExpr(Token opening_bracket, std::vector<unique_expr_ptr> items);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct SubscriptExpr : Expr
{
    Token identifier;
    unique_expr_ptr index;
    unique_expr_ptr value;

    SubscriptExpr(Token identifier, unique_expr_ptr index, unique_expr_ptr value);

    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

#endif // EXPR_HPP