#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>
#include <string>
#include <vector>
namespace Error
{
    struct ErrorInfo
    {
        const size_t line;
        const std::string where;
        const std::string message;
    };
    void report() noexcept;
    void add(const size_t line, const std::string where, const std::string message) noexcept;
    void clear() noexcept;
    extern bool hadError;
    extern bool hadRuntimeError;
    extern std::vector<ErrorInfo> exceptionList;
};

#endif