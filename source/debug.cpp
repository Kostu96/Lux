#include "debug.hpp"
#include "chunk.hpp"

namespace Lux {

    static int simpleInstruction(const char* name, int offset)
    {
        printf("%s\n", name);
        return offset + 1;
    }

    void disassembleChunk(const Chunk& chunk, const char* name)
    {
        std::printf("== %s ==\n", name);
        for (size_t i = 0; i < chunk.size(); i = disassembleInstruction(chunk, i));
    }

    int disassembleInstruction(const Chunk& chunk, int offset)
    {
        printf("%04d ", offset);

        OpCode instruction = chunk[offset];
        switch (instruction)
        {
        case OpCode::Return:
            return simpleInstruction("OP_RETURN", offset);
        default:
            std::printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }

} // namespace Lux
