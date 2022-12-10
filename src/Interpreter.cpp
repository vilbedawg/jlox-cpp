#include "../include/Interpreter.hpp"
#include "../include/Logger.hpp"


void Interpreter::interpret(const std::vector<unique_stmt_ptr>& statements)
{
    try
    {
        for (const auto& ptr : statements)
        {
            assert(ptr != nullptr);
            execute(*ptr);
        }
    }
    catch (const RuntimeError& error)
    {
        Error::addRuntimeError(error);
    }
}

std::string Interpreter::stringify(std::any& object) const
{
    if (!object.has_value())
    {
        return "nil";
    }

    if (object.type() == typeid(bool))
    {
        return std::any_cast<bool>(object) ? "true" : "false";
    }

    if (object.type() == typeid(double))
    {
        auto num_as_string = std::to_string(std::any_cast<double>(object));
        // remove trailing zeroes.
        num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
        num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
        return num_as_string;
    }

    if (object.type() == typeid(std::string))
    {
        return std::any_cast<std::string>(object);
    }

    return {};
}

std::any Interpreter::evaluate(const Expr& expr)
{
    return expr.accept(*this);
}

void Interpreter::execute(const Stmt& stmt)
{
    stmt.accept(*this);
}

void Interpreter::checkNumberOperand(const Token& op, const std::any& operand) const
{
    if (operand.type() != typeid(double))
    {
        throw RuntimeError(op, "Operand must be a number.");
    }
}

void Interpreter::checkNumberOperands(const Token& op, const std::any& lhs,
                                      const std::any& rhs) const
{
    if (lhs.type() != typeid(double) || rhs.type() != typeid(double))
    {
        throw RuntimeError(op, "Operands must be numbers.");
    }
}

bool Interpreter::isTruthy(const std::any& object) const
{
    if (!object.has_value())
    {
        return false;
    }

    if (object.type() == typeid(bool))
    {
        return std::any_cast<bool>(object);
    }

    return true;
}

bool Interpreter::isEqual(const std::any& lhs, const std::any& rhs) const
{
    if (!lhs.has_value() && !rhs.has_value())
    {
        return true;
    }
    if (!lhs.has_value())
    {
        return false;
    }

    if (lhs.type() != rhs.type())
    {
        return false;
    }

    if (lhs.type() == typeid(bool))
    {
        return std::any_cast<bool>(lhs) == std::any_cast<bool>(rhs);
    }

    if (lhs.type() == typeid(double))
    {
        return std::any_cast<double>(lhs) == std::any_cast<double>(rhs);
    }

    if (lhs.type() == typeid(std::string))
    {
        return std::any_cast<std::string>(lhs) == std::any_cast<std::string>(rhs);
    }

    return false;
}

void Interpreter::visit(const ExprStmt& stmt)
{
    evaluate(*stmt.expression);
}

void Interpreter::visit(const PrintStmt& stmt)
{
    if (stmt.expression.get() == nullptr)
    {
        std::cout << '\n';
        return;
    }

    auto value = evaluate(*stmt.expression);
    std::cout << stringify(value) << '\n';
}

void Interpreter::visit(const BlockStmt& stmt)
{
}

void Interpreter::visit(const ClassStmt& stmt)
{
}

void Interpreter::visit(const FnStmt& stmt)
{
}

void Interpreter::visit(const IfStmt& stmt)
{
}

void Interpreter::visit(const ReturnStmt& stmt)
{
}

void Interpreter::visit(const BreakStmt& stmt)
{
}

void Interpreter::visit(const VarStmt& stmt)
{
    std::any value;
    if (stmt.initializer != nullptr)
    {
        value = evaluate(*stmt.initializer);
    }

    environment.define(stmt.identifier.lexeme, value);
}

void Interpreter::visit(const WhileStmt& stmt)
{
}

void Interpreter::visit(const ForStmt& stmt)
{
}

std::any Interpreter::visit(const BinaryExpr& expr)
{
    using enum TokenType;
    std::any left = evaluate(*expr.left);
    std::any right = evaluate(*expr.right);
    switch (expr.op.type)
    {
    case MINUS:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) - std::any_cast<double>(right);

    case SLASH:
        checkNumberOperands(expr.op, left, right);
        if (std::any_cast<double>(right) == 0)
        {
            throw RuntimeError(expr.op, "Division by 0.");
        }
        return std::any_cast<double>(left) / std::any_cast<double>(right);

    case STAR:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) * std::any_cast<double>(right);

    case GREATER:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) > std::any_cast<double>(right);

    case GREATER_EQUAL:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) >= std::any_cast<double>(right);

    case LESS:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) < std::any_cast<double>(right);

    case LESS_EQUAL:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) <= std::any_cast<double>(right);

    case EQUAL_EQUAL:
        return isEqual(left, right);

    case EXCLAMATION_EQUAL:
        return !isEqual(left, right);

    case PLUS:
        if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
        }
        else if (left.type() == typeid(double) && right.type() == typeid(double))
        {
            return std::any_cast<double>(left) + std::any_cast<double>(right);
        }
        throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");

    default:
        return {};
    }
}

std::any Interpreter::visit(const UnaryExpr& expr)
{
    std::any right = evaluate(*expr.right);
    switch (expr.op.type)
    {
    case TokenType::MINUS:
        checkNumberOperand(expr.op, right);
        return -(std::any_cast<double>(right));
    case TokenType::EXCLAMATION:
        return !isTruthy(right);
    default:
        return std::any{};
    }
}

std::any Interpreter::visit(const VarExpr& expr)
{
    return environment.lookup(expr.identifier);
}

std::any Interpreter::visit(const GroupingExpr& expr)
{
    return evaluate(*expr.expression);
}

std::any Interpreter::visit(const LiteralExpr& expr)
{
    return expr.literal;
}

std::any Interpreter::visit(const AssignExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const CallExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const GetExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const SetExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const SuperExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const LogicalExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const ThisExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const ListExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const IncrementExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const DecrementExpr& expr)
{
    return std::any{};
}
