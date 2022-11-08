#include "../include/Lexer.hpp"
#include "../include/Error_handler.hpp"

Lexer ::Lexer(std ::string_view source)
    : start{0}, current{0}, line{1}, m_source{source},
      keywords{
          {"and", TokenType::AND},       {"or", TokenType::OR},        {"class", TokenType::CLASS},
          {"if", TokenType::IF},         {"else", TokenType::ELSE},    {"false", TokenType::_FALSE},
          {"true", TokenType::_TRUE},    {"fn", TokenType::FN},        {"for", TokenType::FOR},
          {"while", TokenType::WHILE},   {"nil", TokenType::NIL},      {"print", TokenType::PRINT},
          {"return", TokenType::RETURN}, {"super", TokenType::SUPER},  {"this", TokenType::THIS},
          {"token", TokenType::VAR},     {"lambda", TokenType::LAMBDA}}
{
}

std::vector<Token>& Lexer::scanTokens()
{
    while (!isEOF())
    {
        start = current;
        scanToken();
    }
    m_tokens.emplace_back(Token(TokenType::_EOF, "", "", line));
    return m_tokens;
}

void Lexer::scanToken()
{
    char c = advance();
    switch (c)
    {
    // 1 character lexemes.
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '{': addToken(TokenType::LEFT_BRACE); break;
    case '}': addToken(TokenType::RIGHT_BRACE); break;
    case ',': addToken(TokenType::COMMA); break;
    case '.': addToken(TokenType::DOT); break;
    case '-': addToken(TokenType::MINUS); break;
    case '+': addToken(TokenType::PLUS); break;
    case ';': addToken(TokenType::SEMICOLON); break;
    case '*': addToken(TokenType::STAR); break;

    // > 1 character lexemes.
    case '!': addToken(match('=') ? TokenType::EXCLAMATION_EQUAL : TokenType::EXCLAMATION); break;
    case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
    case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
    case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
    case '/':
        if (match('/'))
            while (peek() != '\n' && !isEOF())
                static_cast<void>(advance());
        else
            addToken(TokenType::SLASH);

    // ignore whitespace.
    case ' ':
    case '\r':
    case '\t': break;
    case '\n': line++; break;

    // Literals.
    case '"': string(); break;
    default:
        if (isDigit(c))
            number();
        else if (isAlpha(c))
            identifier();
        else
            Error::add(line, "", std::string("Unexpected character: '") + c + "'.");
    }
}

void Lexer::identifier()
{
    while (isAlphaNumeric(peek()))
        static_cast<void>(advance());

    const std::string text{m_source.substr(start, current - start)};
    TokenType type{keywords.contains(text) ? keywords.at(text) : TokenType::IDENTIFIER};
    if (type == TokenType::_TRUE)
        addToken(type, true);
    else if (type == TokenType::_FALSE)
        addToken(type, false);
    else
        addToken(type);
}

void Lexer::number()
{
    while (isDigit(peek()))
        static_cast<void>(advance());

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        static_cast<void>(advance());
        while (isDigit(peek()))
            static_cast<void>(advance());
    }
    addToken(TokenType::NUMBER, std::stod(m_source.substr(start, current - start)));
}

void Lexer::string()
{
    while (peek() != '"' && !isEOF())
    {
        if (peek() == '\n')
            line++;
        static_cast<void>(advance());
    }
    if (isEOF())
    {
        Error::add(line, "", "Unterminated string.");
        return;
    }
    // The closing ".
    static_cast<void>(advance());

    // Trim the surrounding quotes.
    addToken(TokenType::STRING, m_source.substr(start + 1, current - start - 2));
}

bool Lexer::match(const char expected)
{
    if (isEOF() || m_source.at(current) != expected)
        return false;
    current++;
    return true;
}

char Lexer::peek() const
{
    return isEOF() ? '\0' : m_source.at(current);
}

char Lexer::peekNext() const
{
    return (current + 1 >= m_source.length()) ? '\0' : m_source.at(current + 1);
}

bool Lexer::isAlpha(const char c) const
{
    return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_');
}

bool Lexer::isAlphaNumeric(const char c) const
{
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isDigit(const char c) const
{
    return c >= '0' && c <= '9';
}

bool Lexer::isEOF() const
{
    return current >= m_source.size();
}

char Lexer::advance()
{
    return m_source.at(current++);
}

void Lexer::addToken(const TokenType type)
{
    addToken(type, "");
}

void Lexer::addToken(const TokenType type, const literalType literal)
{
    m_tokens.emplace_back(Token(type, m_source.substr(start, current - start), literal, line));
}