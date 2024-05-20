#pragma once
#include "value.hpp"

#include <cstdint>
#include <vector>

namespace Lux {

    enum class OpCode : uint8_t {
        Constant,
        ConstantLong,
        Nil,
        True,
        False,
        Negate,
        Add,
        Subtract,
        Multiply,
        Divide,
        Not,
        Equal,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
        Return
    };

    class Chunk
    {
    public:
        void write(uint8_t byte, size_t line);
        void writeConstant(Value constant, size_t line);

        const uint8_t* getCodeRawPtr() const { return m_code.data(); }
        size_t getCodeSize() const { return m_code.size(); }
        uint8_t getByte(size_t index) const { return m_code[index]; }
        size_t getLine(size_t index) const;

        size_t addConstant(Value value);
        Value getConstant(size_t index) const { return m_constants[index]; }
    private:
        struct LineInfo {
            size_t line;
            size_t indexOffset;
        };

        std::vector<uint8_t> m_code;
        std::vector<LineInfo> m_lines;
        std::vector<Value> m_constants;
    };

} // namespace Lux
