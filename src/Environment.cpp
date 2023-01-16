#include "../include/Environment.hpp"

Environment::Environment(std::shared_ptr<Environment> parent_env)
    : parent_env{std::move(parent_env)}
{
    assert(this->parent_env != nullptr);
}

Environment::Environment() : parent_env{nullptr}
{
}

void Environment::define(const std::string& identifier, const std::any& value)
{
    // Define a new identifier.
    values.try_emplace(identifier, std::make_shared<std::any>(value));
}

void Environment::define(const std::string& identifier, shared_ptr_any ptr_to_val)
{
    // Define a new identifier.
    values.try_emplace(identifier, ptr_to_val);
}

shared_ptr_any Environment::lookup(const Token& identifier)
{
    // Check if the current environment contains the identifier.
    if (values.contains(identifier.lexeme))
    {
        // If so, return the value associated with it.
        return values[identifier.lexeme];
    }

    // If the identifier is not in the current environment, check the parent environment until
    // global scope.
    if (parent_env)
    {
        return parent_env->lookup(identifier);
    }

    // If not in global scope, throw error.
    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}

shared_ptr_any Environment::getAt(size_t distance, const std::string& identifier)
{
    return ancestor(distance)->values[identifier];
}

void Environment::assign(const Token& identifier, const std::any& value)
{
    if (values.contains(identifier.lexeme))
    {
        auto& ptr_to_var = values[identifier.lexeme];
        *ptr_to_var = value;
        return;
    }

    if (parent_env)
    {
        parent_env->assign(identifier, value);
        return;
    }

    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}

void Environment::assignAt(size_t distance, const Token& identifier, const std::any& value)
{
    auto& ptr_to_var = ancestor(distance)->values[identifier.lexeme];
    *ptr_to_var = value;
}

Environment* Environment::ancestor(size_t distance)
{
    auto environment = this;
    for (size_t i = 0u; i < distance; ++i)
    {
        if (!environment->parent_env)
            break;

        environment = environment->parent_env.get();
    }

    return environment;
}