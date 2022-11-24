#ifndef BIS_EXPR_HPP
#define BIS_EXPR_HPP

#include "Token.hpp"
#include "Typedef.hpp"
#include <vector>

struct AssignExpr : Expr
{
    Token identifier;
    unique_expr_ptr value;

    AssignExpr(const Token& _identifier, unique_expr_ptr _value);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct BinaryExpr : Expr
{
    unique_expr_ptr left;
    Token op;
    unique_expr_ptr right;

    BinaryExpr(unique_expr_ptr _left, const Token& _op, unique_expr_ptr _right);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct UnaryExpr : Expr
{
    Token op;
    unique_expr_ptr right;

    UnaryExpr(const Token& _op, unique_expr_ptr _right);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct IncrementExpr : Expr
{
    enum class Type
    {
        POSTFIX,
        PREFIX
    };

    Type type;
    std::unique_ptr<VarExpr> variable;

    IncrementExpr(std::unique_ptr<VarExpr> _variable, Type _type);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct DecrementExpr : Expr
{
    enum class Type
    {
        POSTFIX,
        PREFIX
    };

    Type type;
    std::unique_ptr<VarExpr> variable;

    DecrementExpr(std::unique_ptr<VarExpr> _variable, Type _type);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct CallExpr : Expr
{
    unique_expr_ptr callee;
    Token paren;
    std::vector<unique_expr_ptr> args;

    CallExpr(unique_expr_ptr _callee, const Token& _paren, std::vector<unique_expr_ptr> _args);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct GetExpr : Expr
{
    unique_expr_ptr object;
    Token identifier;

    GetExpr(unique_expr_ptr _object, const Token& _identifier);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct SetExpr : Expr
{
    unique_expr_ptr object;
    Token identifier;
    unique_expr_ptr value;

    SetExpr(unique_expr_ptr _object, const Token& _identifier, unique_expr_ptr _value);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct GroupingExpr : Expr
{
    unique_expr_ptr expression;

    GroupingExpr(unique_expr_ptr _expression);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct LiteralExpr : Expr
{
    std::any literal;

    LiteralExpr(std::any _literal);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct LogicalExpr : Expr
{
    unique_expr_ptr left;
    Token op;
    unique_expr_ptr right;

    LogicalExpr(unique_expr_ptr _left, const Token& _op, unique_expr_ptr _right);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct SuperExpr : Expr
{
    Token keyword;
    Token method;

    SuperExpr(const Token& _keyword, const Token& _method);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct ThisEpxr : Expr
{
    Token keyword;

    ThisEpxr(const Token& _keyword);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct VarExpr : Expr
{
    Token identifier;

    VarExpr(const Token& _identifier);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

struct ListExpr : Expr
{
    Token opening_bracket;
    std::vector<unique_expr_ptr> items;

    ListExpr(const Token& _opening_bracket, std::vector<unique_expr_ptr> _items);
    std::any accept(ExprVisitor<std::any>& visitor) const override;
};

#endif // BIS_EXPR_HPP