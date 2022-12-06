#ifndef BIS_RUNTIME_ERROR_HPP
#define BIS_RUNTIME_ERROR_HPP
#include "Token.hpp"
#include <stdexcept>

class RuntimeError : public std::runtime_error
{
private:
    Token token;

public:
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error{message}, token{token}
    {
    }

    const Token& getToken() const { return token; }
};

#endif // BIS_RUNTIME_ERROR_HPP