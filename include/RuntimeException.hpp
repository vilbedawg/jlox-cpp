
#ifndef RUNTIME_EXCEPTION_HPP
#define RUNTIME_EXCEPTION_HPP

#include "RuntimeError.hpp"
#include "Token.hpp"

class BreakException : public RuntimeError
{
public:
    explicit BreakException(const Token& token) noexcept
        : RuntimeError(token, "Cannot break outside of a loop."){};
};

class ContinueException : public RuntimeError
{
public:
    explicit ContinueException(const Token& token) noexcept
        : RuntimeError(token, "Cannot continue outside of a loop."){};
};

#endif // RUNTIME_EXCEPTION_HPP