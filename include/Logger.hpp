#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "RuntimeError.hpp"
#include "Token.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace Error
{
    struct ErrorInfo
    {
        const unsigned int line;
        const std::string where;
        const std::string message;

        ErrorInfo(unsigned int line, std::string where, std::string message)
            : line{line}, where{std::move(where)}, message{std::move(message)}
        {
        }
    };

    void report() noexcept;
    void addRuntimeError(const RuntimeError& error) noexcept;
    void addError(const unsigned int line, const std::string where,
                  const std::string message) noexcept;
    void addError(const Token& token, std::string message) noexcept;
    extern bool hadError;
    extern bool hadRuntimeError;
    extern std::vector<ErrorInfo> exceptionList;
};

#endif // LOGGER_HPP