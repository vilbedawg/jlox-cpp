#include "../include/Environment.hpp"

Environment::Environment(std::shared_ptr<Environment> parent_env) : parent_env{parent_env}
{
    assert(parent_env != nullptr);
}

Environment::Environment() : parent_env{nullptr}
{
}

void Environment::define(const std::string& identifier, const std::any& value)
{
    // Define a new identifier.
    values.try_emplace(identifier, value);
}

const std::any& Environment::lookup(const Token& identifier)
{
    // Check if the current environment contains the identifier.
    if (values.contains(identifier.lexeme))
    {
        // If so, return the value associated with it.
        return values.at(identifier.lexeme);
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

void Environment::assign(const Token& identifier, const std::any& value)
{
    if (values.contains(identifier.lexeme))
    {
        values.at(identifier.lexeme) = value;
        return;
    }

    if (parent_env)
    {
        parent_env->assign(identifier, value);
        return;
    }

    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}

std::any Environment::getAt(size_t distance, const std::string& identifier)
{
    auto env = ancestor(distance);
    if (!env->values.contains(identifier))
    {
        throw std::runtime_error("Identifier '" + identifier +
                                 "' does not exist. Bug in resolver, should not be happening.");
    }
    return env->values.at(identifier);
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

void Environment::assignAt(size_t distance, const Token& identifier, const std::any& value)
{
    auto env = ancestor(distance);
    if (!env->values.contains(identifier.lexeme))
    {
        throw std::runtime_error("Identifier '" + identifier.lexeme +
                                 "' does not exist. Bug in resolver, should not be happening.");
    }

    env->values[identifier.lexeme] = value;
}