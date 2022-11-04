#ifndef BIS_LEXER
#define BIS_LEXER

#include <vector>
#include <string>
#include <iostream>
#include "../Types/Token.h"

namespace bis
{
    using Token_vector = std::vector<Token::TokenType>;

    class Lexer
    {
    private:
        const std::string m_source;
        std::vector<Token::TokenType> m_tokens;

    public:
        // TODO
        Lexer(std::string source) : m_source{std::move(source)}
        {
            std::cout << "Move constructor init" << '\n';
        }
        Lexer(std::string_view source) : m_source{source}
        {
            std::cout << "Copy constructor init" << '\n';
        }
        ~Lexer()
        {
            std::cout << "destructor" << '\n';
        }
    };

}

#endif