#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include "Interpreter.hpp"
#include "Visitor.hpp"
#include <stack>
#include <unordered_map>
#include <vector>

class Resolver : public ExprVisitor<std::any>, public StmtVisitor
{
public:
    explicit Resolver(Interpreter& interpreter);

    void resolve(const std::vector<unique_stmt_ptr>& statements);

    enum class FuncType
    {
        NONE,
        FUNCTION
    };

    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const UnaryExpr& expr) override;
    std::any visit(const GroupingExpr& expr) override;
    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const AssignExpr& expr) override;
    std::any visit(const CallExpr& expr) override;
    std::any visit(const SetExpr& expr) override;
    std::any visit(const GetExpr& expr) override;
    std::any visit(const SuperExpr& expr) override;
    std::any visit(const LogicalExpr& expr) override;
    std::any visit(const ThisExpr& expr) override;
    std::any visit(const VarExpr& expr) override;
    std::any visit(const ListExpr& expr) override;
    std::any visit(const SubscriptExpr& expr) override;
    std::any visit(const IncrementExpr& expr) override;
    std::any visit(const DecrementExpr& expr) override;

    void visit(const BlockStmt& stmt) override;
    void visit(const ClassStmt& stmt) override;
    void visit(const ExprStmt& stmt) override;
    void visit(const FnStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const PrintStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;
    void visit(const BreakStmt& stmt) override;
    void visit(const ContinueStmt& stmt) override;
    void visit(const VarStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;
    void visit(const ForStmt& stmt) override;

private:
    Interpreter& interpreter;
    using Scope = std::unordered_map<std::string, bool>;
    std::vector<Scope> scopes;
    std::stack<FuncType> func_stack;
    size_t loop_nesting_level = 0u;

    void resolve(const Stmt& stmt);

    void resolve(const Expr& expr);

    void resolveLocal(const Expr* expr, const Token& name);

    void resolveFuntion(const FnStmt& stmt, FuncType type);

    void beginScope();

    void endScope();

    void declare(const Token& identifier);

    void define(const Token& identifer);
};

#endif // RESOLVER_HPP