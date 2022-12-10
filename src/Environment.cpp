#include "../include/Environment.hpp"

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

    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}

void Environment::assign(const Token& identifier, const std::any& value)
{
    if (!values.contains(identifier.lexeme))
    {
        throw RuntimeError(identifier, "Undefined identifier '" + identifier.lexeme + "' .");
    }

    values.at(identifier.lexeme) = value;
}