#include "scanner.hpp"

#include <cstring>

namespace Lux {

    Scanner::Scanner(const char *source) :
        m_start{ source },
        m_current{ source },
        m_line{ 1 } {}

    Token Scanner::getToken()
    {
        skipWhitespace();
        m_start = m_current;
        if (*m_current == '\0') return makeToken(Token::Type::EndOfFile);

        char c = advance();
        if (isAlpha(c)) return identifier();
        if (isDigit(c)) return number();
        switch (c) {
        case '(': return makeToken(Token::Type::LeftParen);
        case ')': return makeToken(Token::Type::RightParen);
        case '{': return makeToken(Token::Type::LeftBrace);
        case '}': return makeToken(Token::Type::RightBrace);
        case ';': return makeToken(Token::Type::Semicolon);
        case ',': return makeToken(Token::Type::Comma);
        case '.': return makeToken(Token::Type::Dot);
        case '-': return makeToken(Token::Type::Minus);
        case '+': return makeToken(Token::Type::Plus);
        case '/': return makeToken(Token::Type::Slash);
        case '*': return makeToken(Token::Type::Star);
        case '!': return makeToken(match('=') ? Token::Type::BangEqual : Token::Type::Bang);
        case '=': return makeToken(match('=') ? Token::Type::EqualEqual : Token::Type::Equal);
        case '<': return makeToken(match('=') ? Token::Type::LessEqual : Token::Type::Less);
        case '>': return makeToken(match('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
        case '"': return string();
        }

        return errorToken("Unexpected character.");
    }

    char Scanner::advance()
    { 
        return *m_current++;
    }

    bool Scanner::match(char expected)
    {
        if (*m_current == '\0' || *m_current != expected) return false;
        m_current++;
        return true;
    }

    char Scanner::peekNext()
    {
        if (*m_current == '\0') return '\0';
        return m_current[1];
    }

    void Scanner::skipWhitespace() {
        while (true)
            switch (*m_current)
            {
            case '\n':
                m_line++;
            case ' ':
            case '\r':
            case '\t':
                m_current++;
                break;
            case '/':
                if (peekNext() == '/') while (*m_current != '\n' && *m_current != '\0') m_current++;
                else return;
                break;
            default:
                return;
            }
    }

    bool Scanner::isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool Scanner::isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    Token Scanner::makeToken(Token::Type type)
    {
        Token token;
        token.type = type;
        token.start = m_start;
        token.length = m_current - m_start;
        token.line = m_line;
        return token;
    }

    Token Scanner::errorToken(const char *message)
    {
        Token token;
        token.type = Token::Type::Error;
        token.start = message;
        token.length = std::strlen(message);
        return token;
    }

    Token Scanner::string()
    {
        while (*m_current != '"' && *m_current != '\0')
        {
            if (*m_current == '\n') m_line++;
            advance();
        }

        if (*m_current == '\0')
            return errorToken("Unterminated string.");

        // The closing quote.
        advance();
        return makeToken(Token::Type::String);
    }

    Token Scanner::number()
    {
        while (isDigit(*m_current)) m_current++;

        if (*m_current == '.' && isDigit(peekNext())) {
            m_current++;

            while (isDigit(*m_current)) m_current++;
        }

        return makeToken(Token::Type::Number);
    }

    Token Scanner::identifier()
    {
        while (isAlpha(*m_current) || isDigit(*m_current)) m_current++;
        return makeToken(identifierType());
    }

    Token::Type Scanner::identifierType()
    {
        switch (*m_start) {
            case 'a': return checkKeyword(1, 2, "nd", Token::Type::And);
            case 'c': return checkKeyword(1, 4, "lass", Token::Type::Class);
            case 'e': return checkKeyword(1, 3, "lse", Token::Type::Else);
            case 'f':
                if (m_current - m_start > 1) {
                    switch (m_start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", Token::Type::False);
                    case 'o': return checkKeyword(2, 1, "r", Token::Type::For);
                    case 'u': return checkKeyword(2, 1, "n", Token::Type::Fun);
                    }
                }
                break;
            case 'i': return checkKeyword(1, 1, "f", Token::Type::If);
            case 'n': return checkKeyword(1, 2, "il", Token::Type::Nil);
            case 'o': return checkKeyword(1, 1, "r", Token::Type::Or);
            case 'p': return checkKeyword(1, 4, "rint", Token::Type::Print);
            case 'r': return checkKeyword(1, 5, "eturn", Token::Type::Return);
            case 's': return checkKeyword(1, 4, "uper", Token::Type::Super);
            case 't':
                if (m_current - m_start > 1) {
                    switch (m_start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", Token::Type::This);
                    case 'r': return checkKeyword(2, 2, "ue", Token::Type::True);
                    }
                }
                break;
            case 'v': return checkKeyword(1, 2, "ar", Token::Type::Var);
            case 'w': return checkKeyword(1, 4, "hile", Token::Type::While);
        }

        return Token::Type::Identifier;
    }

    Token::Type Scanner::checkKeyword(size_t start, size_t length, const char *rest, Token::Type type)
    {
        if (m_current - m_start == start + length && memcmp(m_start + start, rest, length) == 0)
            return type;

        return Token::Type::Identifier;
    }

} // namespace Lux
