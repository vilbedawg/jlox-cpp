#include "../include/Stmt.hpp"
#include <cassert>

BlockStmt::BlockStmt(std::vector<unique_stmt_ptr> statements) : statements{std::move(statements)}
{
}

std::any BlockStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ClassStmt::ClassStmt(const Token& identifier, std::vector<std::unique_ptr<FnStmt>> methods,
                     std::unique_ptr<VarExpr> superclass)
    : identifier{identifier}, superclass{std::move(superclass)}, methods{std::move(methods)}
{
    assert(this->identifier.type == TokenType::IDENTIFIER);
}

std::any ClassStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ExprStmt::ExprStmt(unique_expr_ptr expr) : expression{std::move(expr)}
{
    assert(this->expression != nullptr);
}

std::any ExprStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

FnStmt::FnStmt(const Token& identifier, std::vector<Token> params,
               std::vector<unique_stmt_ptr> body)
    : identifier{identifier}, params{std::move(params)}, body{std::move(body)}
{
    assert(this->identifier.type == TokenType::IDENTIFIER);
}

std::any FnStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

IfBranch::IfBranch(unique_expr_ptr condition, unique_stmt_ptr statement)
    : condition{std::move(condition)}, statement{std::move(statement)}
{
    assert(this->condition != nullptr);
    assert(this->statement != nullptr);
}

IfStmt::IfStmt(IfBranch main_branch, std::vector<IfBranch> elif_branches,
               unique_stmt_ptr else_branch)
    : main_branch{std::move(main_branch)}, elif_branches{std::move(elif_branches)},
      else_branch{std::move(else_branch)}
{
}

std::any IfStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

PrintStmt::PrintStmt(unique_expr_ptr expr) : expression{std::move(expr)}
{
    assert(this->expression != nullptr);
}

std::any PrintStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ReturnStmt::ReturnStmt(const Token& keyword, unique_expr_ptr expr)
    : keyword{keyword}, expression{std::move(expr)}
{
    assert(keyword.type == TokenType::RETURN);
}

std::any ReturnStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

BreakStmt::BreakStmt(const Token& keyword) : keyword{keyword}
{
    assert(this->keyword.type == TokenType::BREAK);
}

std::any BreakStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

VarStmt::VarStmt(const Token& identifier, unique_expr_ptr initializer)
    : identifier{identifier}, initializer{std::move(initializer)}
{
    assert(this->identifier.type == TokenType::IDENTIFIER);
}

std::any VarStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

WhileStmt::WhileStmt(unique_expr_ptr condition, unique_stmt_ptr body)
    : condition{std::move(condition)}, body{std::move(body)}
{
    assert(this->condition != nullptr);
    assert(this->body != nullptr);
}

std::any WhileStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}

ForStmt::ForStmt(unique_stmt_ptr initializer, unique_expr_ptr condition, unique_stmt_ptr increment,
                 unique_stmt_ptr body)
    : initializer{std::move(initializer)}, body{std::move(body)},
      increment{std::move(increment)}, condition{std::move(condition)}
{
    assert(this->initializer != nullptr);
    assert(this->condition != nullptr);
    assert(this->increment != nullptr);
    assert(this->body != nullptr);
}

std::any ForStmt::accept(StmtVisitor<std::any>& visitor) const
{
    return visitor.visit(*this);
}
