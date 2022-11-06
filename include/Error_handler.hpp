#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>
#include <vector>
class ErrorHandler
{
public:
    struct ErrorInfo
    {
        int line;
        std::string m_where;
        std::string m_message;
    };
    static void report();
    static void add(size_t line, std::string_view where, std::string_view message);
    static void clear();
    static bool hadError;
    static bool hadRuntimeError;
    static std::vector<ErrorInfo> exceptionList;
};

#endif