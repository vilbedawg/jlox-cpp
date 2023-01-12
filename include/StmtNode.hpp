#ifndef STMT_HPP
#define STMT_HPP

#include "ExprNode.hpp"
#include "Token.hpp"
#include "Typedef.hpp"
#include "Visitor.hpp"
#include <vector>

struct BlockStmt : Stmt
{
    std::vector<unique_stmt_ptr> statements;

    explicit BlockStmt(std::vector<unique_stmt_ptr> statements);

    void accept(StmtVisitor& visitor) const override;
};

struct ClassStmt : Stmt
{
    Token identifier;
    std::unique_ptr<VarExpr> superclass; // OPTIONAL
    std::vector<std::unique_ptr<FnStmt>> methods;

    ClassStmt(Token identifier, std::vector<std::unique_ptr<FnStmt>> methods,
              std::unique_ptr<VarExpr> superclass);

    void accept(StmtVisitor& visitor) const override;
};

struct ExprStmt : Stmt
{
    unique_expr_ptr expression;

    explicit ExprStmt(unique_expr_ptr expr);

    void accept(StmtVisitor& visitor) const override;
};

struct FnStmt : Stmt
{
    Token identifier;
    std::vector<Token> params;
    std::vector<unique_stmt_ptr> body;

    FnStmt(Token identifier, std::vector<Token> params, std::vector<unique_stmt_ptr> body);

    void accept(StmtVisitor& visitor) const override;
};

struct IfBranch
{
    unique_expr_ptr condition;
    unique_stmt_ptr statement;

    IfBranch(unique_expr_ptr condition, unique_stmt_ptr statement);
};

struct IfStmt : Stmt
{
    IfBranch main_branch;
    std::vector<IfBranch> elif_branches;
    unique_stmt_ptr else_branch; // OPTIONAL

    IfStmt(IfBranch main_branch, std::vector<IfBranch> elif_branches, unique_stmt_ptr else_branch);

    void accept(StmtVisitor& visitor) const override;
};

struct PrintStmt : Stmt
{
    unique_expr_ptr expression; // OPTIONAL

    explicit PrintStmt(unique_expr_ptr expr);

    void accept(StmtVisitor& visitor) const override;
};

struct ReturnStmt : Stmt
{
    Token keyword;
    unique_expr_ptr expression; // OPTIONAL

    ReturnStmt(Token keyword, unique_expr_ptr expr);

    void accept(StmtVisitor& visitor) const override;
};

struct ContinueStmt : Stmt
{
    Token keyword;

    explicit ContinueStmt(Token keyword);

    void accept(StmtVisitor& visitor) const override;
};

struct BreakStmt : Stmt
{
    Token keyword;

    explicit BreakStmt(Token keyword);

    void accept(StmtVisitor& visitor) const override;
};

struct VarStmt : Stmt
{
    Token identifier;
    unique_expr_ptr initializer; // OPTIONAL

    VarStmt(Token identifier, unique_expr_ptr initializer);

    void accept(StmtVisitor& visitor) const override;
};

struct WhileStmt : Stmt
{
    unique_expr_ptr condition;
    unique_stmt_ptr body;

    WhileStmt(unique_expr_ptr condition, unique_stmt_ptr body);

    void accept(StmtVisitor& visitor) const override;
};

struct ForStmt : Stmt
{
    unique_stmt_ptr initializer;
    unique_expr_ptr condition;
    unique_expr_ptr increment;
    unique_stmt_ptr body;

    ForStmt(unique_stmt_ptr initializer, unique_expr_ptr condition, unique_expr_ptr increment,
            unique_stmt_ptr body);

    void accept(StmtVisitor& visitor) const override;
};

#endif // STMT_HPP