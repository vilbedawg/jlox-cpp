#ifndef LOGGER_HPP
#define LOGGER_HPP

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
    };
    void report() noexcept;
    void add(const unsigned int line, const std::string where, const std::string message) noexcept;
    void clear() noexcept;
    extern bool hadError;
    extern bool hadRuntimeError;
    extern std::vector<ErrorInfo> exceptionList;
};

#endif