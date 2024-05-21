#include "vm.hpp"
#include "chunk.hpp"
#include "debug.hpp"
#include "compiler.hpp"

namespace Lux {

    InterpretResult VM::interpret(const char *source)
    {
        Compiler compiler;
        Chunk chunk;
        if (!compiler.compile(source, chunk)) return InterpretResult::CompilationError;

        m_currentChunk = &chunk;
        m_IP = m_currentChunk->getCodeRawPtr();
        m_globals.clear();
        return run();
    }

    InterpretResult VM::run()
    {
#define READ_BYTE() (*m_IP++)
#define READ_CONSTANT() (m_currentChunk->getConstant(READ_BYTE()))
// TODO: READ_CONSTANT_LONG
#define BINARY_OP_N(op) do { \
    if (!peek(0).isNumber() || !peek(1).isNumber()) { \
        runtimeError("Operands must be numbers."); \
        return InterpretResult::RuntimeError; \
      } \
    Value b = pop(); \
    peek().number op= b.number; \
} while(false)
#define BINARY_OP_B(op) do { \
    if (!peek(0).isNumber() || !peek(1).isNumber()) { \
        runtimeError("Operands must be numbers."); \
        return InterpretResult::RuntimeError; \
      } \
    Value b = pop(); \
    push(Value::makeBool(pop().number op b.number)); \
} while(false)

        while(true)
        {
#ifdef DEBUG_TRACE_EXECUTION
            std::printf("stack:    ");
            for (auto slot : m_stack) {
                std::printf("[");
                printValue(slot);
                std::printf("]");
            }
            std::printf("\n");
            disassembleInstruction(*m_currentChunk, m_IP - m_currentChunk->getCodeRawPtr());
#endif
            OpCode opcode = (OpCode)READ_BYTE();
            switch (opcode)
            {
            case OpCode::Constant: push(READ_CONSTANT()); break;
            case OpCode::DefGlobal: {
                String* name = READ_CONSTANT().object->asString();
                if (m_globals.contains(*name)) {
                    runtimeError("Global variable with such name already exists.");
                    return InterpretResult::RuntimeError;
                }
                Value val = pop();
                m_globals.emplace(std::make_pair(*name, val));
            } break;
            case OpCode::Nil:      push(Value::makeNil());       break;
            case OpCode::True:     push(Value::makeBool(true));  break;
            case OpCode::False:    push(Value::makeBool(false)); break;
            case OpCode::Negate:
                if (!peek().isNumber()) {
                    runtimeError("Operand must be a number.");
                    return InterpretResult::RuntimeError;
                }
                peek().number = -peek().number;
                break;
            case OpCode::Add:
                // TODO: Add support for concatenating Strings with Values
                if (peek(0).isString() && peek(1).isString()) {
                    Value b = pop();
                    *peek().object->asString() += *b.object->asString();
                    //concatenate();
                } else if (peek(0).isNumber() && peek(1).isNumber()) {
                    Value b = pop();
                    peek().number += b.number;
                } else {
                    runtimeError( "Operands must be two numbers or two strings.");
                    return InterpretResult::RuntimeError;
                }
                break;
            case OpCode::Subtract: BINARY_OP_N(-); break;
            case OpCode::Multiply: BINARY_OP_N(*); break;
            case OpCode::Divide:   BINARY_OP_N(/); break;
            case OpCode::Not:
                peek().boolean = peek();
                break;
            case OpCode::Equal: {
                Value b = pop();
                push(Value::makeBool(pop() == b));
            } break;
            case OpCode::NotEqual: {
                Value b = pop();
                push(Value::makeBool(pop() != b));
            } break;
            case OpCode::Greater:      BINARY_OP_B(>);  break;
            case OpCode::GreaterEqual: BINARY_OP_B(>=); break;
            case OpCode::Less:         BINARY_OP_B(<);  break;
            case OpCode::LessEqual:    BINARY_OP_B(<=); break;
            case OpCode::Print:
                printValue(pop());
                std::printf("\n");
                break;
            case OpCode::Pop: pop(); break;
            case OpCode::Return:
                return InterpretResult::Success;
            }
        }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP_N
#undef BINARY_OP_B
    }

    bool VM::isFalsey(Value value)
    {
        return value.isNil() || (value.isBool() && !value.boolean);
    }

    void VM::runtimeError(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        std::vprintf(format, args);
        va_end(args);
        std::puts("\n");

        size_t instruction = m_IP - m_currentChunk->getCodeRawPtr() - 1;
        size_t line = m_currentChunk->getLine(instruction);
        std::printf("[line %zu] in script\n", line);
        m_stack.clear(); // TODO: implement proper stack
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

    Value &VM::peek(size_t distance)
    {
        return *(m_stack.end() - 1 - distance);
    }

} // namespace Lux
