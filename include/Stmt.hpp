#ifndef BIS_STMT_HPP
#define BIS_STMT_HPP

#include "Expr.hpp"
#include "Token.hpp"
#include "Typedef.hpp"
#include "Visitor.hpp"

#include <vector>

struct BlockStmt : Stmt
{
    std::vector<unique_stmt_ptr> statements;

    BlockStmt(std::vector<unique_stmt_ptr> _statements);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct ClassStmt : Stmt
{
    Token identifier;
    std::unique_ptr<VarExpr> superclass; // OPTIONAL
    std::vector<std::unique_ptr<FnStmt>> methods;

    ClassStmt(const Token& _identifier, std::vector<std::unique_ptr<FnStmt>> _methods,
              std::unique_ptr<VarExpr> _superclass);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct ExprStmt : Stmt
{
    unique_expr_ptr expr;

    ExprStmt(unique_expr_ptr _expr);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct FnStmt : Stmt
{
    Token identifier;
    std::vector<Token> params;
    std::vector<unique_stmt_ptr> body;

    FnStmt(const Token& _identifier, std::vector<Token> _params,
           std::vector<unique_stmt_ptr> _body);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct IfBranch
{
    unique_expr_ptr condition;
    unique_stmt_ptr statement;

    IfBranch(unique_expr_ptr _condition, unique_stmt_ptr _statement);
};

struct IfStmt : Stmt
{
    IfBranch main_branch;
    std::vector<IfBranch> elif_branches;
    unique_stmt_ptr else_branch; // OPTIONAL

    IfStmt(IfBranch _main_branch, std::vector<IfBranch> _elif_branches,
           unique_stmt_ptr _else_branch);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct PrintStmt : Stmt
{
    unique_expr_ptr expression; // OPTIONAL

    explicit PrintStmt(unique_expr_ptr _expr);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct ReturnStmt : Stmt
{
    Token keyword;
    unique_expr_ptr expression; // OPTIONAL

    explicit ReturnStmt(const Token& _keyword, unique_expr_ptr _expression);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct BreakStmt : Stmt
{
    Token keyword;

    explicit BreakStmt(const Token& _keyword);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct VarStmt : Stmt
{
    Token identifier;
    unique_expr_ptr initializer; // OPTIONAL

    VarStmt(const Token& _identifier, unique_expr_ptr _initializer);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct WhileStmt : Stmt
{
    unique_expr_ptr condition;
    unique_stmt_ptr body;

    WhileStmt(unique_expr_ptr _condition, unique_stmt_ptr _body);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};

struct ForStmt : Stmt
{
    unique_stmt_ptr initializer, body, increment; // OPTIONAL
    unique_expr_ptr condition;                    // OPTIONAL

    ForStmt(unique_stmt_ptr _initializer, unique_expr_ptr _condition, unique_stmt_ptr _increment,
            unique_stmt_ptr _body);
    std::any accept(StmtVisitor<std::any>& visitor) const override;
};
#endif // BIS_STMT_HPP