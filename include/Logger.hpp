#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Token.hpp"

#include <iostream>
#include <string>
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
    void add(const unsigned int line, const std::string where, const std::string message) noexcept;
    void add(const Token& token, std::string message) noexcept;
    void clear() noexcept;
    extern bool hadError;
    extern bool hadRuntimeError;
    extern std::vector<ErrorInfo> exceptionList;
};

#endif // LOGGER_HPP