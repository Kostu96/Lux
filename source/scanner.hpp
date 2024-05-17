#pragma once

namespace Lux {

    struct Token
    {
        enum class Type {
            // Single-character tokens.
            LeftParen, RightParen, LeftBrace, RightBrace,
            Comma, Dot, Minus, Plus, Semicolon, Slash, Star,
            // One or two character tokens.
            Bang, BangEqual, Equal, EqualEqual, Greater, GreaterEqual, Less, LessEqual,
            // Literals.
            Identifier, String, Number,
            // Keywords.
            And, Class, Else, False, For, Fun, If, Nil, Or,
            Print, Return, Super, This, True, Var, While,

            Error, EndOfFile
        };

        Type type;
        const char *start;
        size_t length;
        size_t line;
    };

    class Scanner
    {
    public:
        explicit Scanner(const char *source);

        Token getToken();
    private:
        char advance();
        bool match(char expected);
        char peekNext();
        void skipWhitespace();
        bool isDigit(char c);
        bool isAlpha(char c);
        Token makeToken(Token::Type type);
        Token errorToken(const char *message);
        Token string();
        Token number();
        Token identifier();
        Token::Type identifierType();
        Token::Type checkKeyword(size_t start, size_t length, const char* rest, Token::Type type);

        const char* m_start;
        const char* m_current;
        size_t m_line;
    };

} // namespace Lux
