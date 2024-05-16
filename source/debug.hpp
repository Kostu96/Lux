#pragma once

namespace Lux {

    class Chunk;

    void disassembleChunk(const Chunk& chunk, const char* name);
    size_t disassembleInstruction(const Chunk& chunk, size_t offset);

} // namespace Lux
