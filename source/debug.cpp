#include "debug.hpp"
#include "chunk.hpp"

namespace Lux {

    static size_t simpleInstruction(const char* name, size_t offset)
    {
        std::printf("%s\n", name);
        return offset + 1;
    }

    static size_t byteInstruction(const char* name, const Chunk& chunk, size_t offset)
    {
        uint8_t index = chunk.getByte(offset + 1);
        printf("%-16s %4d\n", name, index);
        return offset + 2;
    }

#define PRINT_CONSTANT() do { \
    std::printf("%-16s %4d  '", name, constant); \
    printValue(chunk.getConstant(constant)); \
    std::printf("'\n"); \
} while (false)

    static size_t constantInstruction(const char* name, const Chunk& chunk, size_t offset)
    {
        uint8_t constant = chunk.getByte(offset + 1);
        PRINT_CONSTANT();
        return offset + 2;
    }

    static size_t constantLongInstruction(const char* name, const Chunk& chunk, size_t offset)
    {
        uint32_t constant = chunk.getByte(offset + 1);
        constant |= chunk.getByte(offset + 2) << 8;
        constant |= chunk.getByte(offset + 3) << 16;
        PRINT_CONSTANT();
        return offset + 4;
    }

#undef PRINT_CONSTANT
    
    void disassembleChunk(const Chunk& chunk, const char* name)
    {
        std::printf("== %s ==\n", name);
        std::printf("byte line opcode          index  value\n");
        for (size_t i = 0; i < chunk.getCodeSize(); i = disassembleInstruction(chunk, i));
    }

    size_t disassembleInstruction(const Chunk& chunk, size_t offset)
    {
        std::printf("%04zu ", offset);
        if (offset > 0 && chunk.getLine(offset) == chunk.getLine(offset - 1))
            std::printf("   | ");
        else
            std::printf("%4zu ", chunk.getLine(offset));

        OpCode instruction = static_cast<OpCode>(chunk.getByte(offset));
        switch (instruction)
        {
        case OpCode::Constant: return constantInstruction("CONSTANT", chunk, offset);
        case OpCode::ConstantLong: return constantLongInstruction("CONSTANT_LONG", chunk, offset);
        case OpCode::DefGlobal: return constantInstruction("DEF_GLOBAL", chunk, offset);
        case OpCode::DefGlobalLong: return constantLongInstruction("DEF_GLOBAL_LONG", chunk, offset);
        case OpCode::GetGlobal: return constantInstruction("GET_GLOBAL", chunk, offset);
        case OpCode::GetGlobalLong: return constantLongInstruction("GET_GLOBAL_LONG", chunk, offset);
        case OpCode::SetGlobal: return constantInstruction("SET_GLOBAL", chunk, offset);
        case OpCode::SetGlobalLong: return constantLongInstruction("SET_GLOBAL_LONG", chunk, offset);
        case OpCode::GetLocal: return byteInstruction("GET_LOCAL", chunk, offset);
        case OpCode::SetLocal: return byteInstruction("SET_LOCAL", chunk, offset);
        case OpCode::Nil: return simpleInstruction("NIL", offset);
        case OpCode::True: return simpleInstruction("TRUE", offset);
        case OpCode::False: return simpleInstruction("FALSE", offset);
        case OpCode::Negate: return simpleInstruction("NEGATE", offset);
        case OpCode::Add: return simpleInstruction("ADD", offset);
        case OpCode::Subtract: return simpleInstruction("SUBTRACT", offset);
        case OpCode::Multiply: return simpleInstruction("MULTIPLY", offset);
        case OpCode::Divide: return simpleInstruction("DIVIDE", offset);
        case OpCode::Not: return simpleInstruction("NOT", offset);
        case OpCode::Equal: return simpleInstruction("EQUAL", offset);
        case OpCode::NotEqual: return simpleInstruction("NOT_EQUAL", offset);
        case OpCode::Less: return simpleInstruction("LESS", offset);
        case OpCode::LessEqual: return simpleInstruction("LESS_EQUAL", offset);
        case OpCode::Greater: return simpleInstruction("GREATER", offset);
        case OpCode::GreaterEqual: return simpleInstruction("GREATER_EQUAL", offset);
        case OpCode::Print: return simpleInstruction("PRINT", offset);
        case OpCode::Pop: return simpleInstruction("POP", offset);
        case OpCode::Return: return simpleInstruction("RETURN", offset);
        default:
            std::printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }

} // namespace Lux
