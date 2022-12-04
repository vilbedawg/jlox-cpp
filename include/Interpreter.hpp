#ifndef BIS_INTERPRETER_HPP
#define BIS_INTERPRETER_HPP
#include "Expr.hpp"
#include "RuntimeError.hpp"
#include "Visitor.hpp"

class Interpreter : public ExprVisitor<std::any>
{
public:
    void interpret(unique_expr_ptr expr);

private:
    void checkNumberOperand(const Token& op, const std::any& operand) const;
    void checkNumberOperands(const Token& op, const std::any& lhs, const std::any& rhs) const;
    bool isTruthy(const std::any& object) const;

    std::string stringify(std::any& object);
    std::any evaluate(const Expr& expr);

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
};

std::any operator+(const std::any& lhs, const std::any& rhs);
double operator-(const std::any& operand);
double operator-(const std::any& lhs, const std::any& rhs);
double operator/(const std::any& lhs, const std::any& rhs);
double operator*(const std::any& lhs, const std::any& rhs);
bool operator>(const std::any& lhs, const std::any& rhs);
bool operator>=(const std::any& lhs, const std::any& rhs);
bool operator<(const std::any& lhs, const std::any& rhs);
bool operator<=(const std::any& lhs, const std::any& rhs);
bool operator==(const std::any& lhs, const std::any& rhs);
bool operator!=(const std::any& lhs, const std::any& rhs);

#endif // BIS_INTERPRETER_HPP