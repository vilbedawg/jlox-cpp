#include "../include/ExprNode.hpp"
#include <cassert>
#include <utility>

AssignExpr::AssignExpr(Token identifier, unique_expr_ptr value)
    : identifier{std::move(identifier)}, value{std::move(value)}
{
    assert(this->value != nullptr);
}

std::any AssignExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

BinaryExpr::BinaryExpr(unique_expr_ptr left, Token op, unique_expr_ptr right)
    : left{std::move(left)}, op{std::move(op)}, right{std::move(right)}
{
    assert(this->left != nullptr);
    assert(this->right != nullptr);
}

std::any BinaryExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

UnaryExpr::UnaryExpr(Token op, unique_expr_ptr right) : op{std::move(op)}, right{std::move(right)}
{
    assert(this->right != nullptr);
}

std::any UnaryExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

IncrementExpr::IncrementExpr(Token variable, Type type)
    : identifier{std::move(variable)}, type{type}
{
    assert(this->identifier.type == TokenType::IDENTIFIER);
}

std::any IncrementExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

DecrementExpr::DecrementExpr(Token variable, Type type)
    : identifier{std::move(variable)}, type{type}
{
    assert(this->identifier.type == TokenType::IDENTIFIER);
}

std::any DecrementExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

CallExpr::CallExpr(unique_expr_ptr callee, Token paren, std::vector<unique_expr_ptr> args)
    : callee{std::move(callee)}, paren{std::move(paren)}, args{std::move(args)}
{
    assert(this->paren.type == TokenType::RIGHT_PAREN);
}

std::any CallExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

GetExpr::GetExpr(unique_expr_ptr object, Token identifier)
    : object{std::move(object)}, identifier{std::move(identifier)}
{
}

std::any GetExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

SetExpr::SetExpr(unique_expr_ptr object, Token identifier, unique_expr_ptr value)
    : object{std::move(object)}, identifier{std::move(identifier)}, value{std::move(value)}
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

LogicalExpr::LogicalExpr(unique_expr_ptr left, Token op, unique_expr_ptr right)
    : left{std::move(left)}, op{std::move(op)}, right{std::move(right)}
{
    assert(this->left != nullptr);
    assert(this->right != nullptr);
    assert(this->op.type == TokenType::AND || this->op.type == TokenType::OR);
}

std::any LogicalExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

SuperExpr::SuperExpr(Token keyword, Token method)
    : keyword{std::move(keyword)}, method{std::move(method)}
{
}

std::any SuperExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ThisExpr::ThisExpr(Token keyword) : keyword{std::move(keyword)}
{
    assert(this->keyword.type == TokenType::THIS);
}

std::any ThisExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

VarExpr::VarExpr(Token identifier) : identifier{std::move(identifier)}
{
}

std::any VarExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ListExpr::ListExpr(Token opening_bracket, std::vector<unique_expr_ptr> items)
    : opening_bracket{std::move(opening_bracket)}, items{std::move(items)}
{
}

std::any ListExpr::accept(ExprVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}
