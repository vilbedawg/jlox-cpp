#ifndef BIS_INTERPRETER_HPP
#define BIS_INTERPRETER_HPP

#include "Environment.hpp"
#include "Expr.hpp"
#include "RuntimeError.hpp"
#include "Stmt.hpp"
#include "Visitor.hpp"

class Interpreter : public ExprVisitor<std::any>, public StmtVisitor
{
private:
    std::unique_ptr<Environment> environment;

    class ScopedEnvironment
    {
    public:
        ScopedEnvironment(Interpreter& interpreter, std::unique_ptr<Environment> env);
        ~ScopedEnvironment();

    private:
        Interpreter& interpreter;
        std::unique_ptr<Environment> previous_env;
    };

public:
    void interpret(const std::vector<unique_stmt_ptr>& statements);
    Interpreter();

private:
    void checkNumberOperand(const Token& op, const std::any& operand) const;
    void checkNumberOperands(const Token& op, const std::any& lhs, const std::any& rhs) const;
    bool isTruthy(const std::any& object) const;
    bool isEqual(const std::any& lhs, const std::any& rhs) const;

    std::string stringify(std::any& object) const;
    std::any evaluate(const Expr& expr);
    void execute(const Stmt& stmt);
    void executeBlock(const std::vector<unique_stmt_ptr>& statements,
                      std::unique_ptr<Environment> new_env);

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
    void visit(const VarStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
};

#endif // BIS_INTERPRETER_HPP