#include "compiler.hpp"
#include "chunk.hpp"
#include "types/string.hpp"

#ifdef DEBUG_PRINT_CODE
#include "debug.hpp"
#endif

namespace Lux {

    bool Compiler::compile(const char *source, Chunk &chunk)
    {
        reset(source, chunk);

        advance();
        while (!match(Token::Type::EndOfFile)) declaration();
        
        emitByte(static_cast<uint8_t>(OpCode::Return));

#ifdef DEBUG_PRINT_CODE
        if (!m_hadError) disassembleChunk(currentChunk(), "code");
#endif

        return !m_hadError;
    }

    void Compiler::reset(const char* source, Chunk& chunk)
    {
        m_scanner = std::make_unique<Scanner>(source);
        m_currentChunk = &chunk;
        m_hadError = false;
        m_panicMode = false;

        m_scopeDepth = 0;
        m_localCount = 0;
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

    bool Compiler::match(Token::Type type)
    {
        if (m_current.type != type) return false;

        advance();
        return true;
    }

    void Compiler::declaration()
    {
        if (match(Token::Type::Var))
            varDeclaration();
        else
            statement();

        if (m_panicMode) synchronize();
    }

    void Compiler::varDeclaration()
    {
        // Parse variable name:
        consume(Token::Type::Identifier, "Expect variable name.");

        String* str = nullptr;
        if (m_scopeDepth == 0) { // Define global
            // TODO: memory leak
            str = String::create(m_previous.start, m_previous.length);
        }
        else { // Declare local
            if (m_localCount == 256) {
                error("Too many local variables in function.");
                return;
            }

            for (int i = m_localCount - 1; i >= 0; i--) {
                Local& local = m_locals[i];
                if (local.depth != -1 && local.depth < m_scopeDepth) {
                    break;
                }

                if (m_previous == local.name) {
                    error("Variable with this name is alredy defined in this scope.");
                }
            }

            m_locals[m_localCount].name = m_previous;
            m_locals[m_localCount].depth = -1; // mark variable as not ready for use...
            m_localCount++;
        }

        match(Token::Type::Equal) ? expression() : emitByte(static_cast<uint8_t>(OpCode::Nil));
        consume(Token::Type::Semicolon, "Expect ';' after variable declaration.");

        m_locals[m_localCount - 1].depth = m_scopeDepth; // ...and after initiazlization expression mark as ready

        if (str) emitDefGlobal(Value::makeObject(str));
    }

    void Compiler::statement()
    {
        if (match(Token::Type::Print))
            printStatement();
        else if (match(Token::Type::LeftBrace)) {
            // begin scope
            m_scopeDepth++;
            
            block();

            // end scope
            m_scopeDepth--;
            while (m_localCount > 0 && m_locals[m_localCount - 1].depth > m_scopeDepth) {
                emitByte(static_cast<uint8_t>(OpCode::Pop)); // TODO: add PopN to optimize when >1 pop
                m_localCount--;
            }
        }
        else
            expressionStatement();
    }

    void Compiler::block()
    {
        while (m_current.type != Token::Type::RightBrace && m_current.type != Token::Type::EndOfFile) {
            declaration();
        }

        consume(Token::Type::RightBrace, "Expect '}' after block.");
    }

    void Compiler::printStatement()
    {
        expression();
        consume(Token::Type::Semicolon, "Expect ';' after an expression.");
        emitByte(static_cast<uint8_t>(OpCode::Print));
    }

    void Compiler::expressionStatement()
    {
        expression();
        consume(Token::Type::Semicolon, "Expect ';' after an expression.");
        emitByte(static_cast<uint8_t>(OpCode::Pop));
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

        bool canAssign = precedence <= Precedence::Assignment;
        prefixRule(*this, canAssign);

        while (precedence <= getRule(m_current.type).precedence) {
            advance();
            ParseRule::ParseFn infixRule = getRule(m_previous.type).infix;
            infixRule(*this, canAssign);
        }

        if (canAssign && match(Token::Type::Equal)) {
            error("Invalid assignment target.");
        }
    }

    void Compiler::number(Compiler &c, bool canAssign)
    {
        double number = std::strtod(c.m_previous.start, nullptr);
        c.emitConstant(Value::makeNumber(number));
    }

    void Compiler::literal(Compiler &c, bool canAssign)
    {
        switch (c.m_previous.type) {
        case Token::Type::False: c.emitByte(static_cast<uint8_t>(OpCode::False)); break;
        case Token::Type::Nil: c.emitByte(static_cast<uint8_t>(OpCode::Nil)); break;
        case Token::Type::True: c.emitByte(static_cast<uint8_t>(OpCode::True)); break;
        }
    }

    void Compiler::string(Compiler &c, bool canAssign)
    {
        // TODO: memory leak
        String *str = String::create(c.m_previous.start + 1, c.m_previous.length - 2);
        c.emitConstant(Value::makeObject(str));
    }

    void Compiler::variable(Compiler& c, bool canAssign)
    {
        int i;
        bool isLocal = false;
        for (i = c.m_localCount - 1; i >= 0; i--) {
            if (c.m_locals[i].name == c.m_previous) {
                if (c.m_locals[i].depth == -1) {
                    c.error("Can't read local variable in its own initializer.");
                }
                isLocal = true;
                break;
            }
        }

        String* str = nullptr;
        if (!isLocal) {
            // TODO: memory leak
            str = String::create(c.m_previous.start, c.m_previous.length);
        }

        if (canAssign && c.match(Token::Type::Equal)) {
            c.expression();
            str ? c.emitSetGlobal(Value::makeObject(str)) : c.emitSetLocal(i);
        } 
        else
            str ? c.emitGetGlobal(Value::makeObject(str)) : c.emitGetLocal(i);
    }

    void Compiler::grouping(Compiler &c, bool canAssign)
    {
        c.expression();
        c.consume(Token::Type::RightParen, "Expect ')' after expression.");
    }

    void Compiler::unary(Compiler &c, bool canAssign)
    {
        Token::Type operatorType = c.m_previous.type;

        // Compile the operand.
        c.parsePrecedence(Precedence::Unary);

        switch (operatorType) {
        case Token::Type::Minus: c.emitByte(static_cast<uint8_t>(OpCode::Negate)); break;
        case Token::Type::Bang: c.emitByte(static_cast<uint8_t>(OpCode::Not)); break;
        }
    }

    void Compiler::binary(Compiler &c, bool canAssign)
    {
        Token::Type operatorType = c.m_previous.type;
        ParseRule& rule = getRule(operatorType);
        c.parsePrecedence(static_cast<Precedence>((static_cast<int>(rule.precedence) + 1)));

        switch (operatorType) {
        case Token::Type::BangEqual:    c.emitByte(static_cast<uint8_t>(OpCode::NotEqual));     break;
        case Token::Type::EqualEqual:   c.emitByte(static_cast<uint8_t>(OpCode::Equal));        break;
        case Token::Type::Greater:      c.emitByte(static_cast<uint8_t>(OpCode::Greater));      break;
        case Token::Type::GreaterEqual: c.emitByte(static_cast<uint8_t>(OpCode::GreaterEqual)); break;
        case Token::Type::Less:         c.emitByte(static_cast<uint8_t>(OpCode::Less));         break;
        case Token::Type::LessEqual:    c.emitByte(static_cast<uint8_t>(OpCode::LessEqual));    break;
        case Token::Type::Plus:         c.emitByte(static_cast<uint8_t>(OpCode::Add));          break;
        case Token::Type::Minus:        c.emitByte(static_cast<uint8_t>(OpCode::Subtract));     break;
        case Token::Type::Star:         c.emitByte(static_cast<uint8_t>(OpCode::Multiply));     break;
        case Token::Type::Slash:        c.emitByte(static_cast<uint8_t>(OpCode::Divide));       break;
        }
    }

    // TODO: make emiting opcodes easier
    void Compiler::emitByte(uint8_t byte)
    {
        currentChunk().write(byte, m_previous.line);
    }

    void Compiler::emitConstant(Value constant)
    {
        currentChunk().writeConstant(constant, m_previous.line, OpCode::Constant, OpCode::ConstantLong);
    }

    void Compiler::emitDefGlobal(Value global)
    {
        currentChunk().writeConstant(global, m_previous.line, OpCode::DefGlobal, OpCode::DefGlobalLong);
    }

    void Compiler::emitGetGlobal(Value global)
    {
        currentChunk().writeConstant(global, m_previous.line, OpCode::GetGlobal, OpCode::GetGlobalLong);
    }

    void Compiler::emitSetGlobal(Value global)
    {
        currentChunk().writeConstant(global, m_previous.line, OpCode::SetGlobal, OpCode::SetGlobalLong);
    }

    void Compiler::emitGetLocal(uint8_t index)
    {
        emitByte(static_cast<uint8_t>(OpCode::GetLocal));
        emitByte(index);
    }

    void Compiler::emitSetLocal(uint8_t index)
    {
        emitByte(static_cast<uint8_t>(OpCode::SetLocal));
        emitByte(index);
    }

    void Compiler::errorAt(const Token &token, const char *message)
    {
        if (m_panicMode) return;
        m_panicMode = true;
        std::fprintf(stderr, "[line %zu | col %zu] Error", token.line, token.col);

        if (token.type == Token::Type::EndOfFile)
            std::fprintf(stderr, " at end");
        else if (token.type != Token::Type::Error)
            std::fprintf(stderr, " at '%.*s'", (int)token.length, token.start);

        std::fprintf(stderr, ": %s\n", message);
        m_hadError = true;
    }

    void Compiler::synchronize()
    {
        m_panicMode = false;

        while (m_current.type != Token::Type::EndOfFile) {
            if (m_previous.type == Token::Type::Semicolon) return;
            switch (m_current.type) {
            case Token::Type::Class:
            case Token::Type::Fun:
            case Token::Type::Var:
            case Token::Type::For:
            case Token::Type::If:
            case Token::Type::While:
            case Token::Type::Print:
            case Token::Type::Return:
                return;
            }

            advance();
        }
    }

    Compiler::ParseRule &Compiler::getRule(Token::Type type)
    {
        return s_rules[static_cast<size_t>(type)];
    }

    Compiler::ParseRule Compiler::s_rules[] = {
        { &grouping, nullptr, Precedence::None },       // LeftParen
        { nullptr,   nullptr, Precedence::None },       // RightParen
        { nullptr,   nullptr, Precedence::None },       // LeftBrace
        { nullptr,   nullptr, Precedence::None },       // RightBrace
        { nullptr,   nullptr, Precedence::None },       // Comma
        { nullptr,   nullptr, Precedence::None },       // Dot
        { &unary,    &binary, Precedence::Term },       // Minus
        { nullptr,   &binary, Precedence::Term} ,       // Plus
        { nullptr,   nullptr, Precedence::None },       // Semicolon
        { nullptr,   &binary, Precedence::Factor },     // Slash
        { nullptr,   &binary, Precedence::Factor },     // Star
        { &unary,    nullptr, Precedence::None },       // Bang
        { nullptr,   &binary, Precedence::Equality },   // BangEqual
        { nullptr,   nullptr, Precedence::None },       // Equal
        { nullptr,   &binary, Precedence::Equality },   // EqualEqual
        { nullptr,   &binary, Precedence::Comparison }, // Greater
        { nullptr,   &binary, Precedence::Comparison }, // GreaterEqual
        { nullptr,   &binary, Precedence::Comparison }, // Less
        { nullptr,   &binary, Precedence::Comparison }, // LessEqual
        { &variable, nullptr, Precedence::None },       // Identifier
        { &string,   nullptr, Precedence::None },       // String
        { &number,   nullptr, Precedence::None },       // Number
        { nullptr,   nullptr, Precedence::None },       // And
        { nullptr,   nullptr, Precedence::None },       // Class
        { nullptr,   nullptr, Precedence::None },       // Else
        { &literal,  nullptr, Precedence::None },       // False
        { nullptr,   nullptr, Precedence::None },       // For
        { nullptr,   nullptr, Precedence::None },       // Fun
        { nullptr,   nullptr, Precedence::None },       // If
        { &literal,  nullptr, Precedence::None },       // Nil
        { nullptr,   nullptr, Precedence::None },       // Or
        { nullptr,   nullptr, Precedence::None },       // Print
        { nullptr,   nullptr, Precedence::None },       // Return
        { nullptr,   nullptr, Precedence::None },       // Super
        { nullptr,   nullptr, Precedence::None },       // This
        { &literal,  nullptr, Precedence::None },       // True
        { nullptr,   nullptr, Precedence::None },       // Var
        { nullptr,   nullptr, Precedence::None },       // While
        { nullptr,   nullptr, Precedence::None },       // Error
        { nullptr,   nullptr, Precedence::None }        // EndOfFile
    };

} // namespace Lux
