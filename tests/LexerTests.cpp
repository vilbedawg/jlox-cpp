#include "../include/Lexer.hpp"
#include <gtest/gtest.h>

void checkTokensEqual(const std::vector<Token>& lhs, const std::vector<Token>& rhs)
{
    ASSERT_EQ(lhs.size(), rhs.size());
    for (std::size_t i = 0; i < rhs.size(); i++)
    {
        EXPECT_EQ(lhs[i].type, rhs[i].type) << "Token types differ at index: " << i;
        EXPECT_EQ(lhs[i].lexeme, rhs[i].lexeme) << "Token lexemes differ at index: " << i;
        EXPECT_EQ(lhs[i].line, rhs[i].line) << "Token lines differ at index: " << i;
    }
}

TEST(LexerTests, BASIC)
{
    const auto test_script = R"(print "Hello, world" 123)";
    Lexer lexer{test_script};
    const auto& tokens = lexer.scanTokens();
    std::vector<Token> expected_tokens{
        {TokenType::PRINT, "print", 1},
        {TokenType::STRING, "Hello, world", 1},
        {TokenType::NUMBER, "123", 1},
        {TokenType::_EOF, "", 1},
    };

    checkTokensEqual(expected_tokens, tokens);
}

TEST(LexerTests, KEYWORDS)
{
    const std::vector<std::string> keywords{
        "and", "if",    "true",   "while", "return", "var", "or",    "else", "fn",
        "nil", "super", "lambda", "class", "false",  "for", "print", "this", "break",
    };
    const std::unordered_map<std::string_view, TokenType> tokentypes{
        {"and", TokenType::AND},       {"or", TokenType::OR},         {"class", TokenType::CLASS},
        {"if", TokenType::IF},         {"else", TokenType::ELSE},     {"false", TokenType::_FALSE},
        {"true", TokenType::_TRUE},    {"fn", TokenType::FN},         {"for", TokenType::FOR},
        {"while", TokenType::WHILE},   {"nil", TokenType::NIL},       {"print", TokenType::PRINT},
        {"return", TokenType::RETURN}, {"super", TokenType::SUPER},   {"this", TokenType::THIS},
        {"var", TokenType::VAR},       {"lambda", TokenType::LAMBDA}, {"break", TokenType::BREAK}};

    std::string test_script;
    for (auto& keyword : keywords)
    {
        test_script += keyword;
        test_script += " ";
    }
    Lexer lexer{test_script};
    auto tokens = lexer.scanTokens();
    std::vector<Token> expected_tokens;
    for (auto& lexeme : keywords)
    {
        expected_tokens.emplace_back(tokentypes.at(lexeme), lexeme, 1);
    }

    tokens.pop_back(); // remove EOF token
    checkTokensEqual(expected_tokens, tokens);
}

TEST(LexerTests, COMMENT)
{
    const auto test_script = R"(// this is a comment 
        print "Hello world!" // this one is a comment as well)";
    Lexer lexer{test_script};
    const auto& tokens = lexer.scanTokens();
    std::vector<Token> expected_tokens{
        {TokenType::PRINT, "print", 2},
        {TokenType::STRING, "Hello world!", 2},
        {TokenType::_EOF, "", 2},
    };

    checkTokensEqual(expected_tokens, tokens);
}

TEST(LexerTests, EXPRESSION)
{
    const auto test_script = R"(
        1 + 1 // line 2
        2 - 2 // line 3
        3 * 3 // line 4
        4 / 4 // line 5
        5 == 5 // line 6
        6 != 6 // line 7
        7 > 7 // line 8
        8 >= 8 // line 9
        9 < 9 // line 10
        10 <= 10 // line 11
    )";

    Lexer lexer{test_script};
    const auto& tokens = lexer.scanTokens();
    /* clang-format off */
    std::vector<Token> expected_tokens{
        {TokenType::NUMBER, "1", 2},   
        {TokenType::PLUS, "+", 2},
        {TokenType::NUMBER, "1", 2},

        {TokenType::NUMBER, "2", 3},   
        {TokenType::MINUS, "-", 3},
        {TokenType::NUMBER, "2", 3},

        {TokenType::NUMBER, "3", 4},   
        {TokenType::STAR, "*", 4},
        {TokenType::NUMBER, "3", 4},

        {TokenType::NUMBER, "4", 5},   
        {TokenType::SLASH, "/", 5},
        {TokenType::NUMBER, "4", 5},

        {TokenType::NUMBER, "5", 6},   
        {TokenType::EQUAL_EQUAL, "==", 6},
        {TokenType::NUMBER, "5", 6},

        {TokenType::NUMBER, "6", 7},   
        {TokenType::EXCLAMATION_EQUAL, "!=", 7},
        {TokenType::NUMBER, "6", 7},

        {TokenType::NUMBER, "7", 8},   
        {TokenType::GREATER, ">", 8},
        {TokenType::NUMBER, "7", 8},

        {TokenType::NUMBER, "8", 9},   
        {TokenType::GREATER_EQUAL, ">=", 9},
        {TokenType::NUMBER, "8", 9},

        {TokenType::NUMBER, "9", 10},  
        {TokenType::LESS, "<", 10},
        {TokenType::NUMBER, "9", 10},

        {TokenType::NUMBER, "10", 11}, 
        {TokenType::LESS_EQUAL, "<=", 11},
        {TokenType::NUMBER, "10", 11},

        {TokenType::_EOF, "", 12},
    };
    /* clang-format on */

    checkTokensEqual(expected_tokens, tokens);
}