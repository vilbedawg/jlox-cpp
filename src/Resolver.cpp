#include "../include/Resolver.hpp"
#include "../include/Logger.hpp"

Resolver::Resolver(Interpreter& interpreter) : interpreter{interpreter}
{
    func_stack.push(FuncType::NONE);
}

void Resolver::resolve(const std::vector<unique_stmt_ptr>& statements)
{
    for (const auto& stmt : statements)
    {
        assert(stmt);
        resolve(*stmt);
    }
}

void Resolver::resolve(const Stmt& stmt)
{
    stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr)
{
    expr.accept(*this);
}

void Resolver::resolveLocal(const Expr* expr, const Token& identifier)
{
    if (scopes.empty())
        return;
    // Look for a variable starting from the innermost scope.
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
    {
        // If variable is found, then we resolve it.
        if (scope->contains(identifier.lexeme))
        {
            interpreter.resolve(*expr, std::distance(scopes.rbegin(), scope));
            return;
        }
    }
    // ... If never found, we can assume that the variable is global.
}

void Resolver::resolveFuntion(const FnStmt& stmt, FuncType type)
{
    // Push the current function type onto the function stack.
    func_stack.push(type);

    // Start a new scope for the function.
    beginScope();

    // Bind each param as variable in the function scope.
    for (const auto& param : stmt.params)
    {
        declare(param);
        define(param);
    }

    // Resolve the statements inside the function body.
    resolve(stmt.body);

    // End the current scope.
    endScope();

    // Pop the current function type from the function stack.
    func_stack.pop();
}

void Resolver::beginScope()
{
    scopes.emplace_back();
}

void Resolver::endScope()
{
    scopes.pop_back();
}

void Resolver::declare(const Token& identifier)
{
    if (scopes.empty())
        return;

    // Get the innermost scope.
    Scope& scope = scopes.back();

    // Dont allow the same variable declaration more than once.
    if (scope.contains(identifier.lexeme))
    {
        Error::addError(identifier, "Variable with the name '" + identifier.lexeme +
                                        "' already exists in this scope");
    }

    // The value associated with a key in the scope map represents whether or not we have completed
    // resolving the initializer for that variable.
    scope.try_emplace(identifier.lexeme, false);
}

void Resolver::define(const Token& identifier)
{
    if (scopes.empty())
        return;

    // Indicates that the variable has been fully initialized by setting the value to true.
    scopes.back()[identifier.lexeme] = true;
}

std::any Resolver::visit(const BinaryExpr& expr)
{
    resolve(*expr.left);
    resolve(*expr.right);
    return {};
}

std::any Resolver::visit(const UnaryExpr& expr)
{
    resolve(*expr.right);
    return {};
}

std::any Resolver::visit(const GroupingExpr& expr)
{
    resolve(*expr.expression);
    return {};
}

std::any Resolver::visit(const LiteralExpr& expr)
{
    // No need to resolve literals.
    return {};
}

std::any Resolver::visit(const AssignExpr& expr)
{
    // Resolve the value assigned to the variable.
    resolve(*expr.value);
    // Resolve the variable being assigned to.
    resolveLocal(&expr, expr.identifier);
    return {};
}

std::any Resolver::visit(const CallExpr& expr)
{
    // Resolve the callee of the call expression, e.g the function or class being called.
    resolve(*expr.callee);

    // Resolve each argument passed to the function/class.
    for (const auto& argument : expr.args)
    {
        resolve(*argument);
    }

    return {};
}

std::any Resolver::visit(const LogicalExpr& expr)
{
    resolve(*expr.left);
    resolve(*expr.right);
    return {};
}

std::any Resolver::visit(const SetExpr& expr)
{
    return {};
}

std::any Resolver::visit(const GetExpr& expr)
{
    return {};
}

std::any Resolver::visit(const SuperExpr& expr)
{
    return {};
}

std::any Resolver::visit(const ThisExpr& expr)
{
    return {};
}

std::any Resolver::visit(const VarExpr& expr)
{
    // Checks to see whether variable is being accessed inside its own initializer.
    if (!scopes.empty())
    {
        // Get the innermost scope.
        const Scope& scope = scopes.back();

        // If the variable exists and its value is false,
        // then it has been declared but not yet defined.
        if (scope.contains(expr.identifier.lexeme) && !scope.at(expr.identifier.lexeme))
        {
            Error::addError(expr.identifier, "Can't read local variable in its own initializer.");
        }
    }

    resolveLocal(&expr, expr.identifier);
    return {};
}

std::any Resolver::visit(const ListExpr& expr)
{
    // Resolve each item in contained in the list.
    for (const auto& item : expr.items)
    {
        resolve(*item);
    }

    return {};
}

std::any Resolver::visit(const SubscriptExpr& expr)
{
    // Resolve the index of the subscript.
    resolve(*expr.index);

    // If there's a value associated with the subscript, then it is also resolved.
    if (expr.value)
    {
        resolve(*expr.value);
    }

    // Resolve the variable being accessed.
    resolveLocal(&expr, expr.identifier);
    return {};
}

std::any Resolver::visit(const IncrementExpr& expr)
{
    // Resolve the variable being incremented.
    resolveLocal(&expr, expr.identifier);
    return {};
}

std::any Resolver::visit(const DecrementExpr& expr)
{
    // Resolve the variable being decremented.
    resolveLocal(&expr, expr.identifier);
    return {};
}

void Resolver::visit(const BlockStmt& stmt)
{
    // Enter a new scope to keep track of variables defined within the block statement.
    beginScope();

    // Resolve all statements within the block statement.
    resolve(stmt.statements);

    // End the scope, discarding any variables defined within the block statement.
    endScope();
}

void Resolver::visit(const ClassStmt& stmt)
{
    // Todo
}

void Resolver::visit(const ExprStmt& stmt)
{
    resolve(*stmt.expression);
}

void Resolver::visit(const FnStmt& stmt)
{
    declare(stmt.identifier);
    define(stmt.identifier);
    resolveFuntion(stmt, FuncType::FUNCTION);
}

void Resolver::visit(const IfStmt& stmt)
{
    // Resolve the condition and statement of the main branch.
    resolve(*stmt.main_branch.condition);
    resolve(*stmt.main_branch.statement);

    // Resolve the conditions and statements of all elif branches
    for (const auto& elif : stmt.elif_branches)
    {
        resolve(*elif.condition);
        resolve(*elif.statement);
    }

    // Resolve the else branch, if present.
    if (stmt.else_branch)
    {
        resolve(*stmt.else_branch);
    }
}

void Resolver::visit(const PrintStmt& stmt)
{
    resolve(*stmt.expression);
}

void Resolver::visit(const ReturnStmt& stmt)
{
    // Add error if not inside a function.
    if (func_stack.top() == FuncType::NONE)
    {
        Error::addError(stmt.keyword, "Can't return from a top-level code.");
    }
    // If return value is not void, resolve it.
    if (stmt.expression)
    {
        resolve(*stmt.expression);
    }
}

void Resolver::visit(const BreakStmt& stmt)
{
    // If not in a nested loop, add a new error.
    if (loop_nesting_level == 0)
    {
        Error::addError(stmt.keyword, "Can't break outside of a loop.");
    }
}

void Resolver::visit(const ContinueStmt& stmt)
{
    // If not in a nested loop, add a new error.
    if (loop_nesting_level == 0)
    {
        Error::addError(stmt.keyword, "Can't continue outside of a loop.");
    }
}

void Resolver::visit(const VarStmt& stmt)
{
    declare(stmt.identifier);
    if (stmt.initializer)
    {
        resolve(*stmt.initializer);
    }

    define(stmt.identifier);
}

void Resolver::visit(const WhileStmt& stmt)
{
    ++loop_nesting_level;
    // Resolve the while loop's condition and body.
    resolve(*stmt.condition);
    resolve(*stmt.body);
    --loop_nesting_level;
}

void Resolver::visit(const ForStmt& stmt)
{
    ++loop_nesting_level;
    // Begin a new scope for the for loop.
    beginScope();

    // Resolve the initializer, condition, and the increment statement, if present.
    if (stmt.initializer)
        resolve(*stmt.initializer);
    if (stmt.condition)
        resolve(*stmt.condition);
    if (stmt.increment)
        resolve(*stmt.increment);

    // Resolve the body statement
    resolve(*stmt.body);

    // End the current scope, discarding any variables declared inside the for loop.
    endScope();
    --loop_nesting_level;
}