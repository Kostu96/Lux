#pragma once
#include "common.hpp"
#include "scanner.hpp"
#include "types/value.hpp"

#include <memory>

namespace Lux {

    class Chunk;

    class Compiler
    {
    public:
        bool compile(const char *source, Chunk &chunk);
    private:
        enum class Precedence {
            None,
            Assignment,  // =
            Or,          // or
            And,         // and
            Equality,    // == !=
            Comparison,  // < > <= >=
            Term,        // + -
            Factor,      // * /
            Unary,       // ! -
            Call,        // . ()
            Primary
        };

        struct ParseRule {
            using ParseFn = void(*)(Compiler &, bool);

            ParseFn prefix;
            ParseFn infix;
            Precedence precedence;
        };

        void advance();
        void consume(Token::Type type, const char* message);
        bool match(Token::Type type);
        
        void declaration();
        void varDeclaration();
        void statement();
        void printStatement();
        void expressionStatement();

        void expression();
        void parsePrecedence(Precedence precedence);
        static void number(Compiler &c, bool canAssign);
        static void literal(Compiler &c, bool canAssign);
        static void string(Compiler &c, bool canAssign);
        static void variable(Compiler& c, bool canAssign);
        static void grouping(Compiler &c, bool canAssign);
        static void unary(Compiler &c, bool canAssign);
        static void binary(Compiler &c, bool canAssign);

        Chunk& currentChunk() { return *m_currentChunk; }
        void emitByte(uint8_t byte);
        void emitConstant(Value constant);
        void emitDefGlobal(Value global);
        void emitGetGlobal(Value global);
        void emitSetGlobal(Value global);

        void errorAtCurrent(const char* message) { errorAt(m_current, message); }
        void error(const char* message) { errorAt(m_previous, message); }
        void errorAt(const Token &token, const char* message);
        void synchronize();

        std::unique_ptr<Scanner> m_scanner{};
        Chunk *m_currentChunk = nullptr;
        Token m_previous;
        Token m_current;
        bool m_hadError;
        bool m_panicMode;

        static ParseRule& getRule(Token::Type type);
        static ParseRule s_rules[];
    };

} // namespace Lux
