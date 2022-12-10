#include "../include/Environment.hpp"

void Environment::define(std::string_view identifier, const std::any& value)
{
    values.try_emplace(identifier, value);
}

const std::any& Environment::lookup(const Token& identifier)
{

    if (values.contains(identifier.lexeme))
    {
        return values.at(identifier.lexeme);
    }

    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}