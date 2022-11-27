#ifndef BIS_VISITOR_HPP
#define BIS_VISITOR_HPP
#include <any>

struct AssignExpr;
struct BinaryExpr;
struct CallExpr;
struct GetExpr;
struct GroupingExpr;
struct LiteralExpr;
struct LogicalExpr;
struct SetExpr;
struct SuperExpr;
struct ThisExpr;
struct UnaryExpr;
struct VarExpr;
struct ListExpr;
struct IncrementExpr;
struct DecrementExpr;

template <typename T>
struct ExprVisitor
{
    virtual T visit(const AssignExpr& expr) = 0;
    virtual T visit(const BinaryExpr& expr) = 0;
    virtual T visit(const CallExpr& expr) = 0;
    virtual T visit(const GetExpr& expr) = 0;
    virtual T visit(const GroupingExpr& expr) = 0;
    virtual T visit(const LiteralExpr& expr) = 0;
    virtual T visit(const LogicalExpr& expr) = 0;
    virtual T visit(const SetExpr& expr) = 0;
    virtual T visit(const SuperExpr& expr) = 0;
    virtual T visit(const ThisExpr& expr) = 0;
    virtual T visit(const UnaryExpr& expr) = 0;
    virtual T visit(const VarExpr& expr) = 0;
    virtual T visit(const ListExpr& expr) = 0;
    virtual T visit(const IncrementExpr& expr) = 0;
    virtual T visit(const DecrementExpr& expr) = 0;
    ~ExprVisitor() = default;
};

struct Expr
{
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

struct BlockStmt;
struct ClassStmt;
struct ExprStmt;
struct FnStmt;
struct IfStmt;
struct PrintStmt;
struct ReturnStmt;
struct BreakStmt;
struct VarStmt;
struct WhileStmt;
struct ForStmt;
struct VarExpr;

template <typename T>
struct StmtVisitor
{
    virtual T visit(const BlockStmt& stmt) = 0;
    virtual T visit(const ClassStmt& stmt) = 0;
    virtual T visit(const ExprStmt& stmt) = 0;
    virtual T visit(const FnStmt& stmt) = 0;
    virtual T visit(const IfStmt& stmt) = 0;
    virtual T visit(const PrintStmt& stmt) = 0;
    virtual T visit(const ReturnStmt& stmt) = 0;
    virtual T visit(const BreakStmt& stmt) = 0;
    virtual T visit(const VarStmt& stmt) = 0;
    virtual T visit(const WhileStmt& stmt) = 0;
    virtual T visit(const ForStmt& stmt) = 0;
    ~StmtVisitor() = default;
};

struct Stmt
{
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor<std::any>& visitor) const = 0;
};

#endif