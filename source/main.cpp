#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, const char* argv[])
{
    Lux::Chunk chunk;
    chunk.push_back(Lux::OpCode::Return);

    Lux::disassembleChunk(chunk, "test chunk");

    return 0;
}
