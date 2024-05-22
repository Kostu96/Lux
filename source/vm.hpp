#pragma once
#include "common.hpp"
#include "types/value.hpp"
#include "types/hash_table.hpp"

#include <cstdarg>
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
        InterpretResult interpret(const char *source);
    private:
        InterpretResult run();

        static bool isFalsey(Value value);

        void runtimeError(const char* format, ...);
        
        void push(Value value);
        Value pop();
        Value& peek(size_t distance = 0);

        const Chunk *m_currentChunk = nullptr;
        const uint8_t *m_IP;
        std::vector<Value> m_stack;
        HashTable m_globals;
    };

} // namespace Lux
