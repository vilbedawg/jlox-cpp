#include "../include/Logger.hpp"

namespace Error
{
    std::vector<ErrorInfo> exceptionList{};
    bool hadError = false;
    bool hadRuntimeError = false;

    void addRuntimeError(const RuntimeError& error) noexcept
    {
        exceptionList.emplace_back(error.getToken().line, "", error.what());
        hadRuntimeError = true;
    }

    void addError(const unsigned int line, std::string where, std::string message) noexcept
    {
        exceptionList.emplace_back(line, std::move(where), std::move(message));
        hadError = true;
    }

    void addError(const Token& token, std::string message) noexcept
    {
        if (token.type == TokenType::_EOF)
        {
            exceptionList.emplace_back(token.line, "at end", std::move(message));
        }
        else
        {
            exceptionList.emplace_back(token.line, "at '" + token.lexeme + "'", std::move(message));
        }

        hadError = true;
    }

    void report() noexcept
    {
        for (const auto& exception : exceptionList)
        {
            std::cerr << "[Line " + std::to_string(exception.line) + "] Error " + exception.where +

                             ": " + exception.message
                      << '\n';
        }
    }
};