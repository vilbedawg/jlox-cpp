#ifndef RUNTIME_ERROR_HPP
#define RUNTIME_ERROR_HPP

#include "Token.hpp"
#include <stdexcept>

class RuntimeError : public std::runtime_error
{
public:
    RuntimeError() = default;

    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error{message}, token{token}
    {
    }

    const Token& getToken() const { return token; }

private:
    Token token;
};

#endif // RUNTIME_ERROR_HPP