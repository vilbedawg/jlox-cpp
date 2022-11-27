#include "../include/Expr.hpp"
#include <cassert>

AssignExpr::AssignExpr(const Token& identifier, unique_expr_ptr value)
    : identifier{identifier}, value{std::move(value)}
{
    assert(this->value != nullptr);
}

std::any AssignExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

BinaryExpr::BinaryExpr(unique_expr_ptr left, const Token& op, unique_expr_ptr right)
    : left{std::move(left)}, op{op}, right{std::move(right)}
{
    assert(this->left != nullptr);
    assert(this->right != nullptr);
}

std::any BinaryExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

UnaryExpr::UnaryExpr(const Token& op, unique_expr_ptr right) : op{op}, right{std::move(right)}
{
    assert(this->right != nullptr);
}

std::any UnaryExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

IncrementExpr::IncrementExpr(std::unique_ptr<VarExpr> variable, Type type)
    : variable{std::move(variable)}, type{type}
{
    assert(this->variable != nullptr);
}

std::any IncrementExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

DecrementExpr::DecrementExpr(std::unique_ptr<VarExpr> variable, Type type)
    : variable{std::move(variable)}, type{type}
{
    assert(this->variable != nullptr);
}

std::any DecrementExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

CallExpr::CallExpr(unique_expr_ptr callee, const Token& paren, std::vector<unique_expr_ptr> args)
    : callee{std::move(callee)}, paren{paren}, args{std::move(args)}
{
    assert(paren.type == TokenType::RIGHT_PAREN);
}

std::any CallExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

GetExpr::GetExpr(unique_expr_ptr object, const Token& identifier)
    : object{std::move(object)}, identifier{identifier}
{
}

std::any GetExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

SetExpr::SetExpr(unique_expr_ptr object, const Token& identifier, unique_expr_ptr value)
    : object{std::move(object)}, identifier{identifier}, value{std::move(value)}
{
}

std::any SetExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

GroupingExpr::GroupingExpr(unique_expr_ptr expr) : expression{std::move(expr)}
{
    assert(this->expression != nullptr);
}

std::any GroupingExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

LiteralExpr::LiteralExpr(std::any literal) : literal{std::move(literal)}
{
}

std::any LiteralExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

LogicalExpr::LogicalExpr(unique_expr_ptr left, const Token& op, unique_expr_ptr right)
    : left{std::move(left)}, op{op}, right{std::move(right)}
{
    assert(this->left != nullptr);
    assert(this->right != nullptr);
    assert(this->op.type == TokenType::AND || this->op.type == TokenType::OR);
}

std::any LogicalExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

SuperExpr::SuperExpr(const Token& keyword, const Token& method) : keyword{keyword}, method{method}
{
}

std::any SuperExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ThisExpr::ThisExpr(const Token& keyword) : keyword{keyword}
{
    assert(this->keyword.type == TokenType::THIS);
}

std::any ThisExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

VarExpr::VarExpr(const Token& identifier) : identifier{identifier}
{
}

std::any VarExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ListExpr::ListExpr(const Token& opening_bracket, std::vector<unique_expr_ptr> items)
    : opening_bracket{opening_bracket}, items{std::move(items)}
{
}

std::any ListExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}
