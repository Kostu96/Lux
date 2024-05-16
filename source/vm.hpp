#pragma once
#include "value.hpp"

#include <cstdint>
#include <vector>

namespace Lux {

    class Chunk;

    enum class InterpretResult {
        Success,
        CompilationError,
        RuntimeError
    };

    class VM
    {
    public:
        InterpretResult interpret(const Chunk& chunk);
    private:
        void push(Value value);
        Value pop();

        const uint8_t *m_IP;
        std::vector<Value> m_stack;
    };

} // namespace Lux
