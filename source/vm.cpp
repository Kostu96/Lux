#include "vm.hpp"
#include "chunk.hpp"
#include "debug.hpp"
#include "scanner.hpp"

#define DEBUG_TRACE_EXECUTION

namespace Lux {

    void compile(const char *source)
    {
        // TODO: this functions is temp
        Scanner scanner{ source };
        int line = -1;
        while(true)
        {
            Token token = scanner.getToken();
            if (token.line != line) {
                std::printf("%4zu ", token.line);
                line = token.line;
            } else {
                std::printf("   | ");
            }
            std::printf("%2d '%.*s'\n", token.type, static_cast<int>(token.length), token.start); 

            if (token.type == Token::Type::EndOfFile) break;
        }
    }

    InterpretResult VM::interpret(const char *source)
    {
        compile(source);
        return InterpretResult::Success;
    }

    InterpretResult VM::run(const Chunk &chunk)
    {
        m_IP = chunk.getCodeRawPtr();

#define READ_BYTE() (*m_IP++)
#define READ_CONSTANT() (chunk.getConstant(READ_BYTE()))
// TODO: READ_CONSTANT_LONG
#define BINARY_OP(op) do { Value b = pop(); *(m_stack.end() - 1) op= b; } while(false)

        while(true)
        {
#ifdef DEBUG_TRACE_EXECUTION
            std::printf("          ");
            for (auto slot : m_stack) {
                std::printf("[ ");
                printValue(slot);
                std::printf(" ]");
            }
            std::printf("\n");
            disassembleInstruction(chunk, m_IP - chunk.getCodeRawPtr());
#endif
            OpCode opcode = (OpCode)READ_BYTE();
            switch (opcode)
            {
            case OpCode::Constant:
                push(READ_CONSTANT());
                break;
            case OpCode::Negate: *(m_stack.end() - 1) = -*(m_stack.end() - 1); break;
            case OpCode::Add: BINARY_OP(+); break;
            case OpCode::Subtract: BINARY_OP(-); break;
            case OpCode::Multiply: BINARY_OP(*); break;
            case OpCode::Divide: BINARY_OP(/); break;
            case OpCode::Return:
                printValue(pop());
                std::printf("\n");
                return InterpretResult::Success;
            }
        }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
    }

    void VM::push(Value value)
    {
        m_stack.push_back(value);
    }

    Value VM::pop()
    {
        Value val = *(m_stack.end() - 1);
        m_stack.pop_back();
        return val;
    }

} // namespace Lux
