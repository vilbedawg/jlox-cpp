#ifndef BIS_BREAK_HPP
#define BIS_BREAK_HPP

#include "RuntimeError.hpp"
#include "Token.hpp"
#include <stdexcept>

class BreakStatement : public RuntimeError
{
public:
    explicit BreakStatement(const Token& token) noexcept
        : RuntimeError(token, "Cannot break outside of a loop."){};
};

#endif // BIS_BREAK_HPP