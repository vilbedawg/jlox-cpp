#include "../include/Logger.hpp"

namespace Error
{
    std::vector<ErrorInfo> exceptionList{};
    bool hadError = false;
    bool hadRuntimeError = false;

    void add(const unsigned int line, std::string where, std::string message) noexcept
    {
        exceptionList.push_back({line, std::move(where), std::move(message)});
        hadError = true;
    }

    void add(const Token& token, std::string message) noexcept
    {
        if (token.type == TokenType::_EOF)
        {
            exceptionList.push_back({token.line, "at end", std::move(message)});
        }
        else
        {
            exceptionList.push_back({token.line, "at '" + token.lexeme + "'", std::move(message)});
        }

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