#include "../include/Lexer.hpp"
#include "../include/Logger.hpp"

const std::unordered_map<std::string, TokenType> Lexer::keywords{
    {"and", TokenType::AND},       {"or", TokenType::OR},         {"class", TokenType::CLASS},
    {"if", TokenType::IF},         {"else", TokenType::ELSE},     {"false", TokenType::_FALSE},
    {"true", TokenType::_TRUE},    {"fn", TokenType::FN},         {"for", TokenType::FOR},
    {"while", TokenType::WHILE},   {"nil", TokenType::NIL},       {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},   {"this", TokenType::THIS},
    {"var", TokenType::VAR},       {"lambda", TokenType::LAMBDA}, {"break", TokenType::BREAK}};

Lexer ::Lexer(std ::string_view source) : start{0}, current{0}, line{1}, source{source}
{
}

std::vector<Token>& Lexer::scanTokens()
{
    while (!isEOF())
    {
        start = current;
        scanToken();
    }
    tokens.emplace_back(Token(TokenType::_EOF, "", line));
    return tokens;
}

void Lexer::scanToken()
{
    char c = peek();
    advance();
    switch (c)
    {

    // 1 character lexemes.
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case '[':
        addToken(TokenType::LEFT_BRACKET);
        break;
    case ']':
        addToken(TokenType::RIGHT_BRACKET);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '.':
        addToken(TokenType::DOT);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;

    // > 1 character lexemes.
    case '!':
        addToken(match('=') ? TokenType::EXCLAMATION_EQUAL : TokenType::EXCLAMATION);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '-':
        addToken(match('-') ? TokenType::MINUS_MINUS : TokenType::MINUS);
        break;
    case '+':
        addToken(match('+') ? TokenType::PLUS_PLUS : TokenType::PLUS);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '/':
        if (match('/'))
        {
            while (peek() != '\n' && !isEOF())
            {
                advance();
            }
        }
        else
        {
            addToken(TokenType::SLASH);
        }

    // ignore whitespace.
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line++;
        break;

    // Literals.
    case '"':
        string();
        break;
    default:
        if (isDigit(c))
        {
            number();
        }
        else if (isAlpha(c))
        {
            identifier();
        }
        else
        {
            Error::addError(line, "", std::string("Unexpected character: '") + c + "'.");
        }
    }
}

void Lexer::identifier()
{
    while (isAlphaNumeric(peek()))
    {
        advance();
    }

    const std::string text = source.substr(start, current - start);
    addToken(keywords.contains(text) ? keywords.at(text) : TokenType::IDENTIFIER);
}

void Lexer::number()
{
    while (isDigit(peek()))
    {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the "."
        advance();

        while (isDigit(peek()))
        {
            advance();
        }
    }
    addToken(TokenType::NUMBER);
}

void Lexer::string()
{
    while (peek() != '"' && !isEOF())
    {
        if (peek() == '\n')
        {
            line++;
        }
        advance();
    }
    if (isEOF())
    {
        Error::addError(line, "", "Unterminated string.");
        return;
    }

    // Consume the closing ".
    advance();

    // Trim the surrounding quotes.
    addToken(TokenType::STRING);
}

bool Lexer::match(const char expected)
{
    if (isEOF() || source.at(current) != expected)
    {
        return false;
    }

    current++;
    return true;
}

char Lexer::peek() const
{
    return isEOF() ? '\0' : source.at(current);
}

char Lexer::peekNext() const
{
    return (current + 1 >= source.length()) ? '\0' : source.at(current + 1);
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
    return current >= source.size();
}

void Lexer::advance()
{
    current++;
}

std::string Lexer::getLexeme(TokenType type) const
{
    return (type == TokenType::STRING) ? source.substr(start + 1, current - start - 2)
                                       : source.substr(start, current - start);
}

void Lexer::addToken(const TokenType type)
{
    tokens.emplace_back(type, std::move(getLexeme(type)), line);
}
