#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>
#include <vector>
namespace bis
{
    class Exception
    {
    public:
        struct ErrorInfo
        {
            int line;
            std::string m_where;
            std::string m_message;
        };
        Exception() = default;
        void report() const;
        void add(int line, std::string_view where, std::string_view message);
        void clear();
        bool hadError;
        bool hadRuntimeError;

    private:
        std::vector<ErrorInfo> exceptionList;
    };
}

#endif