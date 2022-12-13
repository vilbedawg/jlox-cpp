#include "../include/AstPrinter.hpp"

std::string AstPrinter::print(const std::vector<unique_expr_ptr>& expressions)
{
    for (const auto& expr : expressions)
    {
        expr.get()->accept(*this);
    }

    return stream.str();
}

std::any AstPrinter::visit(const BinaryExpr& expr)
{
    parenthesize(expr.op.lexeme, {std::move(expr.left.get()), std::move(expr.right.get())});
    return {};
}

std::any AstPrinter::visit(const UnaryExpr& expr)
{
    parenthesize(expr.op.lexeme, {std::move(expr.right.get())});
    return {};
}

std::any AstPrinter::visit(const GroupingExpr& expr)
{
    parenthesize("group", {std::move(expr.expression.get())});
    return {};
}

std::any AstPrinter::visit(const LiteralExpr& expr)
{
    if (expr.literal.type() == typeid(double))
    {
        stream << std::any_cast<double>(expr.literal);
    }
    else if (expr.literal.type() == typeid(std::string))
    {
        stream << std::any_cast<std::string>(expr.literal);
    }
    else
    {
        stream << "nil";
    }

    return {};
}

std::any AstPrinter::visit(const AssignExpr& expr)
{
    parenthesize("=" + expr.identifier.lexeme, {std::move(expr.value.get())});
    return {};
}

std::any AstPrinter::visit(const CallExpr& expr)
{
    stream << "(call ";
    expr.callee.get()->accept(*this);
    for (auto& arg : expr.args)
    {
        stream << " ";
        arg.get()->accept(*this);
    }
    stream << ")";
    return {};
}

std::any AstPrinter::visit(const GetExpr& expr)
{
    stream << "(. ";
    expr.object.get()->accept(*this);
    stream << expr.identifier.lexeme + ")";
    return {};
}

std::any AstPrinter::visit(const SetExpr& expr)
{
    stream << "(= ";
    expr.object.get()->accept(*this);
    stream << " " + expr.identifier.lexeme + " ";
    expr.value.get()->accept(*this);
    stream << ")";
    return {};
}

std::any AstPrinter::visit(const SuperExpr& expr)
{
    stream << "(super " + expr.method.lexeme + ")";
    return {};
}

std::any AstPrinter::visit(const LogicalExpr& expr)
{
    parenthesize(expr.op.lexeme, {std::move(expr.left.get()), std::move(expr.right.get())});
    return {};
}

std::any AstPrinter::visit(const ThisExpr& expr)
{
    stream << "this";
    return {};
}

std::any AstPrinter::visit(const VarExpr& expr)
{
    stream << expr.identifier.lexeme;
    return {};
}

std::any AstPrinter::visit(const ListExpr& expr)
{
    stream << "(list [ ";
    for (auto& item : expr.items)
    {
        stream << " ";
        item.get()->accept(*this);
    }
    stream << " ])";
    return {};
}

std::any AstPrinter::visit(const IncrementExpr& expr)
{
    parenthesize("++", {std::move(expr.identifier.get())});
    return {};
}

std::any AstPrinter::visit(const DecrementExpr& expr)
{
    parenthesize("--", {std::move(expr.identifier.get())});
    return {};
}

void AstPrinter::parenthesize(const std::string_view name, std::initializer_list<const Expr*> exprs)
{

    stream << "(" << name;
    for (const auto& expr : exprs)
    {
        stream << " ";
        expr->accept(*this);
    }

    stream << ")";
}
