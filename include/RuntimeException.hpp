
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

class ReturnException : public std::runtime_error
{
public:
    explicit ReturnException(std::any value) : std::runtime_error{""}, value{std::move(value)} {};

    const std::any& getReturnValue() const { return value; }

private:
    std::any value;
};

#endif // RUNTIME_EXCEPTION_HPP