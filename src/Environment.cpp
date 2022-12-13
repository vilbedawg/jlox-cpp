#include "../include/Environment.hpp"

Environment::Environment(Environment* parent_env) : parent_env{parent_env}
{
    assert(parent_env != nullptr);
}

Environment::Environment() : parent_env{nullptr}
{
}

void Environment::define(const std::string& identifier, const std::any& value)
{
    values.try_emplace(identifier, value);
}

const std::any& Environment::lookup(const Token& identifier)
{

    if (values.contains(identifier.lexeme))
    {
        return values.at(identifier.lexeme);
    }

    if (parent_env)
    {
        return parent_env->lookup(identifier);
    }

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
