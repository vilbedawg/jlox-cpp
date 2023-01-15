#include "../include/Interpreter.hpp"
#include "../include/BuiltIn.hpp"
#include "../include/Logger.hpp"
#include "../include/RuntimeException.hpp"

Interpreter::Interpreter() : global_environment{globals.get()}
{
    globals->define("clock", ClockCallable{});
    globals->define("print", PrintCallable{});
    environment = std::move(globals);
}

void Interpreter::interpret(const std::vector<unique_stmt_ptr>& statements)
{
    try
    {
        for (const auto& stmt : statements)
        {
            assert(stmt != nullptr);
            execute(*stmt);
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
    // Enter a new environment.
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
    // Throws a runtime error if either the left-hand side or the right-hand side operand is not of
    // type double.
    if (lhs.type() != typeid(double) || rhs.type() != typeid(double))
    {
        throw RuntimeError(op, "Operands must be numbers.");
    }
}

bool Interpreter::isTruthy(const std::any& object) const
{
    // Checks if the passed in object is considered "truthy".

    // This case would indicate a null value.
    if (!object.has_value())
    {
        return false;
    }

    // If the object is of type bool, return the cast value.
    if (object.type() == typeid(bool))
    {
        return std::any_cast<bool>(object);
    }

    // Object has value and is not a false boolean, Therefore it is considered truthy.
    return true;
}

bool Interpreter::isEqual(const std::any& lhs, const std::any& rhs) const
{
    // Checks if two `std::any` objects are equal.
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

    // If the type is not bool, double, or std::string, return false
    return false;
}

void Interpreter::resolve(const Expr& expr_ptr, size_t distance)
{
    locals.try_emplace(&expr_ptr, distance);
}

shared_ptr_any Interpreter::lookUpVariable(const Token& identifier, const Expr* expr_ptr) const
{
    if (locals.contains(expr_ptr))
    {
        size_t distance = locals.at(expr_ptr);
        return environment->getAt(distance, identifier.lexeme);
    }

    return global_environment->lookup(identifier);
}

void Interpreter::assignVariable(const Expr* expr_ptr, const Token& identifier,
                                 const std::any& value)
{
    // Check if the variable is defined in the local scope.
    if (locals.contains(expr_ptr))
    {
        // If so, assign the value to the variable at the specific environment.
        size_t distance = locals.at(expr_ptr);
        environment->assignAt(distance, identifier, value);
    }
    // If the variable is not defined in the local scope.
    else
    {
        // Assign the value to the variable in the global scope.
        global_environment->assign(identifier, value);
    }
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
    executeBlock(stmt.statements, std::make_shared<Environment>(environment));
}

void Interpreter::visit(const ClassStmt& stmt)
{
}

void Interpreter::visit(const FnStmt& stmt)
{
    environment->define(stmt.identifier.lexeme, FunctionType(&stmt, environment));
}

void Interpreter::visit(const IfStmt& stmt)
{
    // Check if the main branch condition is truthy
    if (isTruthy(evaluate(*stmt.main_branch.condition)))
    {
        // If it is, execute the statement
        execute(*stmt.main_branch.statement);
        return;
    }

    // Check each elif branch
    for (const auto& elif : stmt.elif_branches)
    {
        // If any elif branch's condition is truthy, execute its statement and return
        if (isTruthy(evaluate(*elif.condition)))
        {
            execute(*elif.statement);
            return;
        }
    }

    // If none of the conditions are true and there is an else branch, execute it.
    if (stmt.else_branch)
    {
        execute(*stmt.else_branch);
    }
}

void Interpreter::visit(const ReturnStmt& stmt)
{
    std::any value;
    // If the return statement is not void, evaluate the expression.
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
    // If the variable has an initializer, evaluate the initializer.
    if (stmt.initializer)
    {
        value = evaluate(*stmt.initializer);
    }

    // Define the variable in the current environment with the given identifier and value
    environment->define(stmt.identifier.lexeme, value);
}

void Interpreter::visit(const WhileStmt& stmt)
{
    // While the while loop condition is truthy.
    while (isTruthy(evaluate(*stmt.condition)))
    {
        try
        {
            // Execute the loop's body.
            execute(*stmt.body);
        }
        // If a break statement is encountered, exit the loop.
        catch (const ContinueException&)
        {
            // Do nothing.
        }
        // If a continue statement is encountered, continue to the next iteration.
        catch (const BreakException&)
        {
            return;
        }
    }
}

void Interpreter::visit(const ForStmt& stmt)
{
    // Enter a new environment.
    EnvironmentGuard environment_guard{*this, std::make_shared<Environment>(environment)};

    // If the for loop has an initializer, we execute it.
    if (stmt.initializer)
    {
        execute(*stmt.initializer);
    }

    // No condition can be interpreted as 'while true'.
    bool no_condition = stmt.condition == nullptr;

    // While the for loop condition is truthy.
    while (no_condition || isTruthy(evaluate(*stmt.condition)))
    {
        try
        {
            // Execute the for loop's body.
            execute(*stmt.body);
            // If the for loop has an increment, execute it.
            if (stmt.increment)
            {
                evaluate(*stmt.increment);
            }
        }
        // If a break statement is encountered, exit the loop.
        catch (const BreakException&)
        {
            return;
        }
        // If a continue statement is encountered, continue to the next iteration.
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
    // Evaluate the left-hand side and right-hand side operands of the binary expression
    auto left = evaluate(*expr.left);
    auto right = evaluate(*expr.right);

    using enum TokenType;
    // Check the type of the operator.
    switch (expr.op.type)
    {
    case MINUS:
        checkNumberOperands(expr.op, left, right);
        return std::any_cast<double>(left) - std::any_cast<double>(right);

    case SLASH:
        checkNumberOperands(expr.op, left, right);

        // Throw error if right operand is 0.
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
            // Remove trailing zeroes.
            std::string num_as_string = std::to_string(std::any_cast<double>(left));
            num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
            num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
            return num_as_string + std::any_cast<std::string>(right);
        }
        else if (left.type() == typeid(std::string) && right.type() == typeid(double))
        {
            // Remove trailing zeroes.
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
    // Evaluate the right-hand side operand of the unary expression.
    auto right = evaluate(*expr.right);

    // Check the type of the operator
    switch (expr.op.type)
    {
    case TokenType::MINUS:
        // Ensure that the right-hand side operand is a number.
        checkNumberOperand(expr.op, right);
        // Return the negation of the right-hand side operand.
        return -(std::any_cast<double>(right));

    case TokenType::EXCLAMATION:
        // Return the negation of the truthiness of the right-hand side operand.
        return !isTruthy(right);

    default:
        return {};
    }
}

std::any Interpreter::visit(const VarExpr& expr)
{
    // Retrieve the value associated with the identifier.
    auto value = lookUpVariable(expr.identifier, &expr);

    // If the value is of type list or string,
    // return a reference to the object instead.
    if (const auto& value_type = std::any_cast<shared_ptr_any>(value)->type();
        value_type == typeid(List) || value_type == typeid(std::string))
    {
        return value;
    }

    // Else return the objects value.
    return *(std::any_cast<shared_ptr_any>(value));
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
    // Evaluate the assigned value.
    auto value = evaluate(*expr.value);

    // Assign the new value to the variable.
    assignVariable(&expr, expr.identifier, value);

    return value;
}

std::any Interpreter::visit(const CallExpr& expr)
{
    // Evaluate the callee (the function or class being called).
    auto callee = evaluate(*expr.callee);

    // Collect the arguments passed to the function or class.
    std::vector<std::any> arguments;
    arguments.reserve(expr.args.size());
    for (const auto& arg : expr.args)
    {
        arguments.emplace_back(evaluate(*arg));
    }

    // Prevent calling objects which are not of callable type.
    std::unique_ptr<Callable> function;
    if (callee.type() == typeid(FunctionType))
    {
        function = std::make_unique<FunctionType>(std::any_cast<FunctionType>(callee));
    }
    else if (callee.type() == typeid(ClockCallable))
    {
        function = std::make_unique<ClockCallable>();
    }
    else if (callee.type() == typeid(PrintCallable))
    {
        function = std::make_unique<PrintCallable>(arguments.size());
    }
    else
    {
        // Throw an error if the callee is not callable (a function or class).
        throw RuntimeError(expr.paren,
                           expr.paren.lexeme +
                               " is not callable. Callable object must be a function or a class.");
    }

    // Check that the number of arguments passed to the function or class
    // matches the expected number
    if (arguments.size() != function->getArity())
    {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(function->getArity()) +
                                           " arguments but got " +
                                           std::to_string(arguments.size()) + " .");
    }

    // Return by calling the function.
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
    // Evaluate the left operand of the logical expression.
    auto left = evaluate(*expr.left);

    // If the operator is OR and the left operand is truthy, return the left operand.
    if (expr.op.type == TokenType::OR)
    {
        if (isTruthy(left))
        {
            return left;
        }
    }
    // If the operator is 'AND' and the left operand is falsy, return the left operand.
    else if (!isTruthy(left))
    {
        return left;
    }

    // If the left operand didn't short-circuit the evaluation, evaluate the right operand and
    // return it.
    return evaluate(*expr.right);
}

std::any Interpreter::visit(const ListExpr& expr)
{
    List list;
    // Evaluate each item contained in the list.
    for (const auto& item : expr.items)
    {
        assert(item);
        list.append(evaluate((*item)));
    }

    return list;
}

std::any Interpreter::visit(const SubscriptExpr& stmt)
{
    // Get the pointer for the list object.
    auto value_ptr = lookUpVariable(stmt.identifier, &stmt);

    // Check if the pointers value is a list, if not throw a runtime error.
    if (value_ptr->type() != typeid(List))
    {
        throw RuntimeError(stmt.identifier,
                           "Object '" + stmt.identifier.lexeme + "' is not subscriptable.");
    }

    // Dereference the pointer to access the underlying objects items.
    auto items = *(std::any_cast<shared_ptr_any>(value_ptr));

    // Evaluate the index expression. Need to use multiple temporary variables for type safety.
    auto index = evaluate(*stmt.index);
    double index_cast = 0;

    // Refers to the size of the original list object.
    size_t object_size = 0u;

    // Anything else than numbers for indexes are permitted.
    if (index.type() == typeid(int))
    {
        index_cast = static_cast<double>(std::any_cast<int>(index));
    }
    else if (index.type() == typeid(double))
    {
        index_cast = std::any_cast<double>(index);
    }
    else
    {
        throw RuntimeError(stmt.identifier, "Indices must be integers.");
    }

    // Throw an error if index is not an integer.
    if (static_cast<int>(index_cast) != index_cast)
    {
        throw RuntimeError(stmt.identifier, "Indices must be integers.");
    }

    try
    {
        auto list = std::any_cast<List>(items);
        object_size = list.length();

        // Allows negative indexes for reverse order.
        if (index_cast < 0)
        {
            index_cast = static_cast<int>(list.length()) + index_cast;
        }

        // If value is associated with the subscript expression, new value will be assigned to the
        // corresponding index.
        if (stmt.value)
        {
            list.at(index_cast) = evaluate(*stmt.value);
        }

        // Update the pointed object with the new list.
        *value_ptr = list;

        // Return the value at index.
        return list.at(index_cast);
    }
    catch (const std::out_of_range&)
    {
        throw RuntimeError(stmt.identifier, "Index out of range. Index is " +
                                                std::to_string(static_cast<int>(index_cast)) +
                                                " but object size is " +
                                                std::to_string(object_size));
    }
}

std::any Interpreter::visit(const IncrementExpr& expr)
{
    // Get the current value of the variable that is being incremented.
    auto old_value = lookUpVariable(expr.identifier, &expr);

    if (old_value->type() != typeid(double))
    {
        throw RuntimeError(expr.identifier,
                           "Cannot increment a non integer type '" + expr.identifier.lexeme + "'.");
    }

    // Increment the value by 1.
    const double new_value = std::any_cast<double>(*old_value) + 1;

    // Update the object being pointed to by.
    *old_value = std::any_cast<double>(*old_value) + 1;

    // If the expression is a postfix increment, return the old value
    // otherwise return the new value.
    return expr.type == IncrementExpr::Type::POSTFIX ? new_value - 1 : new_value;
}

std::any Interpreter::visit(const DecrementExpr& expr)
{
    // Get the current value of the variable that is being incremented.
    auto old_value = lookUpVariable(expr.identifier, &expr);

    if (old_value->type() != typeid(double))
    {
        throw RuntimeError(expr.identifier,
                           "Cannot decrement a non integer type '" + expr.identifier.lexeme + "'.");
    }

    // Decrement the value by 1.
    const double new_value = std::any_cast<double>(*old_value) - 1;
    // Assign the new value to the variable.
    *old_value = std::any_cast<double>(*old_value) - 1;

    // If the expression is a postfix increment, return the old value
    // otherwise return the new value.
    return expr.type == DecrementExpr::Type::POSTFIX ? new_value + 1 : new_value;
}

// The EnvironmentGuard class is used to manage the interpreter's environment stack. It follows the
// RAII technique, which means that when an instance of the class is created, a copy of the current
// environment is stored, and the current environment is moved to the new one. If a runtime error is
// encountered and the interpreter needs to unwind the stack and return to the previous environment,
// the EnvironmentGuard class's destructor is called, which swaps the resources back to the previous
// environment.
Interpreter::EnvironmentGuard::EnvironmentGuard(Interpreter& interpreter,
                                                std::shared_ptr<Environment> enclosing_env)
    : interpreter{interpreter}, previous_env{interpreter.environment}
{
    interpreter.environment = std::move(enclosing_env);
}

Interpreter::EnvironmentGuard::~EnvironmentGuard()
{
    interpreter.environment = std::move(previous_env);
}