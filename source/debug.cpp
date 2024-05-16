#include "debug.hpp"
#include "chunk.hpp"

namespace Lux {

    static size_t simpleInstruction(const char* name, size_t offset)
    {
        std::printf("%s\n", name);
        return offset + 1;
    }

    static size_t constantInstruction(const char* name, const Chunk& chunk, size_t offset)
    {
        uint8_t constant = chunk.getByte(offset + 1);
        std::printf("%-16s %4d '", name, constant);
        printValue(chunk.getConstant(constant));
        std::printf("'\n");
        return offset + 2;
    }

    static size_t constantLongInstruction(const char* name, const Chunk& chunk, size_t offset)
    {
        uint32_t constant = chunk.getByte(offset + 1);
        constant |= chunk.getByte(offset + 2) << 8;
        constant |= chunk.getByte(offset + 3) << 16;
        std::printf("%-16s %4d '", name, constant);
        printValue(chunk.getConstant(constant));
        std::printf("'\n");
        return offset + 4;
    }

    void disassembleChunk(const Chunk& chunk, const char* name)
    {
        std::printf("== %s ==\n", name);
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
        case OpCode::Constant: return constantInstruction("OP_CONSTANT", chunk, offset);
        case OpCode::ConstantLong: return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
        case OpCode::Negate: return simpleInstruction("OP_NEGATE", offset);
        case OpCode::Add: return simpleInstruction("OP_ADD", offset);
        case OpCode::Subtract: return simpleInstruction("OP_SUBTRACT", offset);
        case OpCode::Multiply: return simpleInstruction("OP_MULTIPLY", offset);
        case OpCode::Divide: return simpleInstruction("OP_DIVIDE", offset);
        case OpCode::Return: return simpleInstruction("OP_RETURN", offset);
        default:
            std::printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }

} // namespace Lux
