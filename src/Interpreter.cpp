#include "../include/Interpreter.hpp"
#include "../include/BisFunction.hpp"
#include "../include/BuiltIn.hpp"
#include "../include/Logger.hpp"
#include "../include/RuntimeException.hpp"

Interpreter::Interpreter() : global_environment{globals.get()}
{
    globals->define("clock", bis::ClockCallable{});
    globals->define("print", bis::PrintCallable{});
    environment = std::move(globals);
}

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
    catch (const ReturnException&)
    {
        // Do nothing.
    }

    Error::report();
}

std::any Interpreter::evaluate(const Expr& expr)
{
    return expr.accept(*this);
}

void Interpreter::execute(const Stmt& stmt)
{
    stmt.accept(*this);
}

void Interpreter::executeBlock(const std::vector<unique_stmt_ptr>& statements,
                               std::shared_ptr<Environment> enclosing_env)
{
    EnvironmentGuard environment_guard{*this, std::move(enclosing_env)};
    for (const auto& statement : statements)
    {
        assert(statement != nullptr);
        execute(*statement);
    }
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
    evaluate(*stmt.expression);
}

void Interpreter::visit(const BlockStmt& stmt)
{
    executeBlock(stmt.statements, environment);
}

void Interpreter::visit(const ClassStmt& stmt)
{
}

void Interpreter::visit(const FnStmt& stmt)
{
    environment->define(stmt.identifier.lexeme, BisFunction(&stmt, environment));
}

void Interpreter::visit(const IfStmt& stmt)
{
    if (isTruthy(evaluate(*stmt.main_branch.condition)))
    {
        execute(*stmt.main_branch.statement);
        return;
    }

    for (const auto& elif : stmt.elif_branches)
    {
        if (isTruthy(evaluate(*elif.condition)))
        {
            execute(*elif.statement);
            return;
        }
    }

    if (stmt.else_branch)
    {
        execute(*stmt.else_branch);
    }
}

void Interpreter::visit(const ReturnStmt& stmt)
{
    std::any value;
    if (stmt.expression)
    {
        value = evaluate(*stmt.expression);
    }

    throw ReturnException(value);
}

void Interpreter::visit(const BreakStmt& stmt)
{
    throw BreakException(stmt.keyword);
}

void Interpreter::visit(const ContinueStmt& stmt)
{
    throw ContinueException(stmt.keyword);
}

void Interpreter::visit(const VarStmt& stmt)
{
    std::any value;
    if (stmt.initializer)
    {
        value = evaluate(*stmt.initializer);
    }

    environment->define(stmt.identifier.lexeme, value);
}

void Interpreter::visit(const WhileStmt& stmt)
{
    while (isTruthy(evaluate(*stmt.condition)))
    {
        try
        {
            execute(*stmt.body);
        }
        catch (const ContinueException&)
        {
            // Do nothing
        }
        catch (const BreakException&)
        {
            return;
        }
    }
}

void Interpreter::visit(const ForStmt& stmt)
{
    EnvironmentGuard environment_guard{*this, environment};

    if (stmt.initializer)
    {
        execute(*stmt.initializer);
    }

    if (!stmt.condition)
    {
        return;
    }

    while (isTruthy(evaluate(*stmt.condition)))
    {
        try
        {
            execute(*stmt.body);
            if (stmt.increment)
            {
                evaluate(*stmt.increment);
            }
        }
        catch (const BreakException&)
        {
            return;
        }
        catch (const ContinueException&)
        {
            if (stmt.increment)
            {
                evaluate(*stmt.increment);
            }
        }
    }
}

std::any Interpreter::visit(const BinaryExpr& expr)
{
    using enum TokenType;
    auto left = evaluate(*expr.left);
    auto right = evaluate(*expr.right);
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
        else if (left.type() == typeid(double) && right.type() == typeid(std::string))
        {

            std::string num_as_string = std::to_string(std::any_cast<double>(left));
            num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
            num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
            return num_as_string + std::any_cast<std::string>(right);
        }
        else if (left.type() == typeid(std::string) && right.type() == typeid(double))
        {
            std::string num_as_string = std::to_string(std::any_cast<double>(right));
            num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
            num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
            return std::any_cast<std::string>(left) + num_as_string;
        }

        throw RuntimeError(expr.op, "Operands must be of type string or number.");

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
    return environment->lookup(expr.identifier);
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
    auto value = evaluate(*expr.value);
    environment->assign(expr.identifier, value);
    return value;
}

std::any Interpreter::visit(const CallExpr& expr)
{
    auto callee = evaluate(*expr.callee);

    std::vector<std::any> arguments;
    arguments.reserve(expr.args.size());
    for (const auto& arg : expr.args)
    {
        arguments.emplace_back(evaluate(*arg));
    }

    std::unique_ptr<Callable> function;
    if (callee.type() == typeid(BisFunction))
    {
        function = std::make_unique<BisFunction>(std::any_cast<BisFunction>(callee));
    }
    else if (callee.type() == typeid(bis::ClockCallable))
    {
        function = std::make_unique<bis::ClockCallable>();
    }
    else if (callee.type() == typeid(bis::PrintCallable))
    {
        function = std::make_unique<bis::PrintCallable>(arguments.size());
    }
    else
    {
        throw RuntimeError(expr.paren,
                           expr.paren.lexeme +
                               " is not callable. Callable object must be a function or a class.");
    }

    if (arguments.size() != function->getArity())
    {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(function->getArity()) +
                                           " arguments but got " +
                                           std::to_string(arguments.size()) + " .");
    }

    return function->call(*this, arguments);
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

std::any Interpreter::visit(const ThisExpr& expr)
{
    return std::any{};
}

std::any Interpreter::visit(const LogicalExpr& expr)
{
    auto left = evaluate(*expr.left);
    if (expr.op.type == TokenType::OR)
    {
        if (isTruthy(left))
        {
            return left;
        }
    }
    else if (!isTruthy(left))
    {
        return left;
    }

    return evaluate(*expr.right);
}

std::any Interpreter::visit(const ListExpr& expr)
{
    auto list = std::make_shared<List>();
    for (const auto& item : expr.items)
    {
        assert(item);
        list->append(evaluate((*item)));
    }

    return list;
}

std::any Interpreter::visit(const SubscriptExpr& stmt)
{
    auto items = environment->lookup(stmt.identifier);
    if (items.type() != typeid(std::shared_ptr<List>))
    {
        throw RuntimeError(stmt.identifier,
                           "Object '" + stmt.identifier.lexeme + " is not subscriptable.");
    }

    auto index = evaluate(*stmt.index);
    if (index.type() != typeid(double))
    {
        throw RuntimeError(stmt.identifier, "Indices must be integers.");
    }

    int casted_index = std::any_cast<double>(index);
    int object_size = 0;

    try
    {
        auto casted_list = std::any_cast<std::shared_ptr<List>>(items);
        object_size = casted_list->length();

        if (casted_index < 0)
        {
            casted_index = casted_list->length() + casted_index;
        }

        if (stmt.value)
        {
            casted_list->at(casted_index) = evaluate(*stmt.value);
        }

        return casted_list->at(casted_index);
    }
    catch (const std::out_of_range&)
    {
        throw RuntimeError(stmt.identifier,
                           "Index out of range. Index is " + std::to_string(casted_index) +
                               " but object size is " + std::to_string(object_size));
    }
}

std::any Interpreter::visit(const IncrementExpr& expr)
{
    const auto variable = environment->lookup(expr.identifier);

    if (variable.type() != typeid(double))
    {
        throw RuntimeError(expr.identifier,
                           "Cannot increment a non integer type '" + expr.identifier.lexeme + "'.");
    }

    const auto incremented_variable = std::any_cast<double>(variable) + 1;
    environment->assign(expr.identifier, incremented_variable);

    return expr.type == IncrementExpr::Type::POSTFIX ? variable : incremented_variable;
}

std::any Interpreter::visit(const DecrementExpr& expr)
{
    const auto variable = environment->lookup(expr.identifier);

    if (variable.type() != typeid(double))
    {
        throw RuntimeError(expr.identifier,
                           "Cannot decrement a non integer type '" + expr.identifier.lexeme + "'.");
    }

    const auto decremented_variable = std::any_cast<double>(variable) - 1;
    environment->assign(expr.identifier, decremented_variable);

    return expr.type == DecrementExpr::Type::POSTFIX ? variable : decremented_variable;
}

Environment& Interpreter::getGlobalEnvironment()
{
    assert(global_environment);
    return *global_environment;
}

Interpreter::EnvironmentGuard::EnvironmentGuard(Interpreter& interpreter,
                                                std::shared_ptr<Environment> enclosing_env)
    : interpreter{interpreter}, previous_env{interpreter.environment}
{
    interpreter.environment = std::make_shared<Environment>(std::move(enclosing_env));
}

Interpreter::EnvironmentGuard::~EnvironmentGuard()
{
    interpreter.environment = std::move(previous_env);
}