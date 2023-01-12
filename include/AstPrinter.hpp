#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP
#include "ExprNode.hpp"
#include "Visitor.hpp"
#include <sstream>

class AstPrinter : public ExprVisitor<std::any>
{
private:
    void parenthesize(const std::string_view name, std::initializer_list<const Expr*> exprs);
    std::stringstream stream;

public:
    std::string print(const std::vector<unique_expr_ptr>& expressions);

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
#endif // ASTPRINTER_HPP