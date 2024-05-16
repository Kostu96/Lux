#include "vm.hpp"
#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, const char* argv[])
{
    Lux::VM vm;

    Lux::Chunk chunk;
    chunk.writeConstant(1.2, 1);
    chunk.writeConstant(3.4, 1);
    chunk.write((uint8_t)Lux::OpCode::Add, 1);
    chunk.writeConstant(5.6, 1);
    chunk.write((uint8_t)Lux::OpCode::Divide, 1);
    chunk.write((uint8_t)Lux::OpCode::Negate, 1);
    chunk.write((uint8_t)Lux::OpCode::Return, 1);

    Lux::disassembleChunk(chunk, "test chunk");
    
    std::printf("\nInterpreting test chunk\n");
    vm.interpret(chunk);

    return 0;
}
