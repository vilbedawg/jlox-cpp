#include "../include/Error_handler.hpp"

namespace Error
{
    std::vector<ErrorInfo> exceptionList{};
    bool hadError = false;
    bool hadRuntimeError = false;
    void add(const size_t line, const std::string where, const std::string message) noexcept
    {
        exceptionList.push_back({line, where, message});
        hadError = true;
    }

    void report() noexcept
    {
        for (const auto& exception : exceptionList)
        {
            std::cout << "[Line " + std::to_string(exception.line) + "] Error " + exception.where +
                             ": " + exception.message
                      << '\n';
        }
    }

    void clear() noexcept
    {
        exceptionList.clear();
    }
};