#include "../include/Interpreter.hpp"
#include "../include/Logger.hpp"
#include <cassert>

void Interpreter::interpret(unique_expr_ptr expr)
{
    try
    {
        std::any value = evaluate(*expr);
        std::cout << stringify(value) << '\n';
    }
    catch (const RuntimeError& error)
    {
        Error::addRuntimeError(error);
    }
}

std::string Interpreter::stringify(std::any& object)
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

    return std::string();
}

std::any Interpreter::evaluate(const Expr& expr)
{
    return expr.accept(*this);
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

std::any Interpreter::visit(const BinaryExpr& expr)
{
    std::any left = evaluate(*expr.left);
    std::any right = evaluate(*expr.right);
    switch (expr.op.type)
    {
    case TokenType::MINUS:
        checkNumberOperands(expr.op, left, right);
        return left - right;
    case TokenType::SLASH:
        checkNumberOperands(expr.op, left, right);
        if (std::any_cast<double>(right) == 0)
        {
            throw RuntimeError(expr.op, "Division by 0.");
        }
        return left / right;
    case TokenType::STAR:
        checkNumberOperands(expr.op, left, right);
        return left * right;
    case TokenType::GREATER:
        checkNumberOperands(expr.op, left, right);
        return left > right;
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(expr.op, left, right);
        return left >= right;
    case TokenType::LESS:
        checkNumberOperands(expr.op, left, right);
        return left < right;
    case TokenType::LESS_EQUAL:
        checkNumberOperands(expr.op, left, right);
        return left <= right;
    case TokenType::EQUAL_EQUAL:
        return left == right;
    case TokenType::EXCLAMATION_EQUAL:
        return left != right;
    case TokenType::PLUS:
    {
        auto expression = left + right;
        if (expr.op.type == TokenType::PLUS)
        {
            if (!expression.has_value())
            {
                throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
            }

            return expression;
        }
    }
    default:
        return std::any{};
    }
}

std::any Interpreter::visit(const UnaryExpr& expr)
{
    std::any right = evaluate(*expr.right);
    switch (expr.op.type)
    {
    case TokenType::MINUS:
        checkNumberOperand(expr.op, right);
        return -right;
    case TokenType::EXCLAMATION:
        return !isTruthy(right);
    default:
        return std::any{};
    }
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

std::any Interpreter::visit(const VarExpr& expr)
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

double operator-(const std::any& operand)
{
    return -(std::any_cast<double>(operand));
}

double operator-(const std::any& lhs, const std::any& rhs)
{
    return std::any_cast<double>(lhs) - std::any_cast<double>(lhs);
}

std::any operator+(const std::any& lhs, const std::any& rhs)
{
    if (lhs.type() == typeid(std::string) && rhs.type() == typeid(std::string))
    {
        return std::any_cast<std::string>(lhs) + std::any_cast<std::string>(rhs);
    }
    else if (lhs.type() == typeid(double) && rhs.type() == typeid(double))
    {
        return std::any_cast<double>(lhs) + std::any_cast<double>(rhs);
    }

    return std::any{};
}

double operator/(const std::any& lhs, const std::any& rhs)
{
    return std::any_cast<double>(lhs) / std::any_cast<double>(rhs);
}

double operator*(const std::any& lhs, const std::any& rhs)
{
    return std::any_cast<double>(lhs) * std::any_cast<double>(rhs);
}

bool operator>(const std::any& lhs, const std::any& rhs)
{
    return std::any_cast<double>(lhs) > std::any_cast<double>(rhs);
};

bool operator>=(const std::any& lhs, const std::any& rhs)
{

    return std::any_cast<double>(lhs) >= std::any_cast<double>(rhs);
};

bool operator<(const std::any& lhs, const std::any& rhs)
{
    return std::any_cast<double>(lhs) < std::any_cast<double>(rhs);
};

bool operator<=(const std::any& lhs, const std::any& rhs)
{
    return std::any_cast<double>(lhs) <= std::any_cast<double>(rhs);
};

bool operator==(const std::any& lhs, const std::any& rhs)
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
};

bool operator!=(const std::any& lhs, const std::any& rhs)
{
    return !(lhs == rhs);
};
