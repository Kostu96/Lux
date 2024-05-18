#include "compiler.hpp"
#include "chunk.hpp"

#ifdef DEBUG_PRINT_CODE
#include "debug.hpp"
#endif

namespace Lux {

    bool Compiler::compile(const char *source, Chunk &chunk)
    {
        m_scanner = std::make_unique<Scanner>(source);
        m_currentChunk = &chunk;
        m_hadError = false;
        m_panicMode = false;

        advance();
        expression();
        consume(Token::Type::EndOfFile, "Expect end of expression.");
        
        emitByte(static_cast<uint8_t>(OpCode::Return));

#ifdef DEBUG_PRINT_CODE
        if (!m_hadError) disassembleChunk(currentChunk(), "code");
#endif

        return !m_hadError;
    }

    void Compiler::advance()
    {
        m_previous = m_current;

        while (true)
        {
            m_current = m_scanner->getToken();
            if (m_current.type != Token::Type::Error) break;

            errorAtCurrent(m_current.start);
        }
    }

    void Compiler::consume(Token::Type type, const char *message)
    {
        if (m_current.type == type) {
            advance();
            return;
        }

        errorAtCurrent(message);
    }

    void Compiler::expression()
    {
        parsePrecedence(Precedence::Assignment);
    }

    void Compiler::parsePrecedence(Precedence precedence)
    {
        advance();
        ParseRule::ParseFn prefixRule = getRule(m_previous.type).prefix;
        if (!prefixRule) {
            error("Expect expression.");
            return;
        }
        prefixRule(*this);

        while (precedence <= getRule(m_current.type).precedence) {
            advance();
            ParseRule::ParseFn infixRule = getRule(m_previous.type).infix;
            infixRule(*this);
        }
    }

    void Compiler::number(Compiler &c)
    {
        double value = std::strtod(c.m_previous.start, nullptr);
        c.emitConstant(value);
    }

    void Compiler::grouping(Compiler &c)
    {
        c.expression();
        c.consume(Token::Type::RightParen, "Expect ')' after expression.");
    }

    void Compiler::unary(Compiler &c)
    {
        Token::Type operatorType = c.m_previous.type;

        // Compile the operand.
        c.parsePrecedence(Precedence::Unary);

        switch (operatorType) {
        case Token::Type::Minus: c.emitByte(static_cast<uint8_t>(OpCode::Negate)); break;
        }
    }

    void Compiler::binary(Compiler &c)
    {
        Token::Type operatorType = c.m_previous.type;
        ParseRule& rule = getRule(operatorType);
        c.parsePrecedence(static_cast<Precedence>((static_cast<int>(rule.precedence) + 1)));

        switch (operatorType) {
        case Token::Type::Plus:  c.emitByte(static_cast<uint8_t>(OpCode::Add)); break;
        case Token::Type::Minus: c.emitByte(static_cast<uint8_t>(OpCode::Subtract)); break;
        case Token::Type::Star:  c.emitByte(static_cast<uint8_t>(OpCode::Multiply)); break;
        case Token::Type::Slash: c.emitByte(static_cast<uint8_t>(OpCode::Divide)); break;
        }
    }

    // TODO: make emiting opcodes easier
    void Compiler::emitByte(uint8_t byte)
    {
        currentChunk().write(byte, m_previous.line);
    }

    void Compiler::emitConstant(Value constant)
    {
        currentChunk().writeConstant(constant, m_previous.line);
    }

    void Compiler::errorAt(const Token &token, const char *message)
    {
        if (m_panicMode) return;
        m_panicMode = true;
        std::printf("[line %zu] Error", token.line);

        if (token.type == Token::Type::EndOfFile)
            std::printf(" at end");
        else if (token.type != Token::Type::Error)
            std::printf(" at '%.*s'", (int)token.length, token.start);

        std::printf(": %s\n", message);
        m_hadError = true;
    }

    Compiler::ParseRule &Compiler::getRule(Token::Type type)
    {
        return s_rules[static_cast<size_t>(type)];
    }

    Compiler::ParseRule Compiler::s_rules[] = {
        { &grouping, nullptr, Precedence::None },   // LeftParen
        { nullptr,  nullptr, Precedence::None },   // RightParen
        { nullptr,  nullptr, Precedence::None },   // LeftBrace
        { nullptr,  nullptr, Precedence::None },   // RightBrace
        { nullptr,  nullptr, Precedence::None },   // Comma
        { nullptr,  nullptr, Precedence::None },   // Dot
        { &unary,   &binary,  Precedence::Term },   // Minus
        { nullptr,  &binary,  Precedence::Term} ,   // Plus
        { nullptr,  nullptr, Precedence::None },   // Semicolon
        { nullptr,  &binary,  Precedence::Factor }, // Slash
        { nullptr,  &binary,  Precedence::Factor }, // Star
        { nullptr,  nullptr, Precedence::None },   // Bang
        { nullptr,  nullptr, Precedence::None },   // BangEqual
        { nullptr,  nullptr, Precedence::None },   // Equal
        { nullptr,  nullptr, Precedence::None },   // EqualEqual
        { nullptr,  nullptr, Precedence::None },   // Greater
        { nullptr,  nullptr, Precedence::None },   // GreaterEqual
        { nullptr,  nullptr, Precedence::None },   // Less
        { nullptr,  nullptr, Precedence::None },   // LessEqual
        { nullptr,  nullptr, Precedence::None },   // Identifier
        { nullptr,  nullptr, Precedence::None },   // String
        { &number,  nullptr, Precedence::None },   // Number
        { nullptr,  nullptr, Precedence::None },   // And
        { nullptr,  nullptr, Precedence::None },   // Class
        { nullptr,  nullptr, Precedence::None },   // Else
        { nullptr,  nullptr, Precedence::None },   // False
        { nullptr,  nullptr, Precedence::None },   // For
        { nullptr,  nullptr, Precedence::None },   // Fun
        { nullptr,  nullptr, Precedence::None },   // If
        { nullptr,  nullptr, Precedence::None },   // Nil
        { nullptr,  nullptr, Precedence::None },   // Or
        { nullptr,  nullptr, Precedence::None },   // Print
        { nullptr,  nullptr, Precedence::None },   // Return
        { nullptr,  nullptr, Precedence::None },   // Super
        { nullptr,  nullptr, Precedence::None },   // This
        { nullptr,  nullptr, Precedence::None },   // True
        { nullptr,  nullptr, Precedence::None },   // Var
        { nullptr,  nullptr, Precedence::None },   // While
        { nullptr,  nullptr, Precedence::None },   // Error
        { nullptr,  nullptr, Precedence::None }    // EndOfFile
    };

} // namespace Lux
