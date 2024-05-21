#include "chunk.hpp"

namespace Lux {

    void Chunk::write(uint8_t byte, size_t line)
    {
        m_code.emplace_back(byte);

        if (!m_lines.empty() && m_lines[m_lines.size() - 1].line == line)
            m_lines[m_lines.size() - 1].indexOffset++;
        else
            m_lines.emplace_back(line, 1);
    }

    void Chunk::writeConstant(Value constant, size_t line)
    {
        size_t constantIndex = addConstant(constant);
        if (constantIndex >= 256)
        {
            write((uint8_t)OpCode::ConstantLong, line);
            write(constantIndex % 256, line);
            constantIndex /= 256;
            write(constantIndex % 256, line);
            constantIndex /= 256;
            write(constantIndex % 256, line);
        }
        else {
            write((uint8_t)OpCode::Constant, line);
            write((uint8_t)constantIndex, line);
        }
    }

    void Chunk::writeGlobal(Value global, size_t line)
    {
        size_t globalIndex = addConstant(global);
        if (globalIndex >= 256)
        {
            write((uint8_t)OpCode::DefGlobalLong, line);
            write(globalIndex % 256, line);
            globalIndex /= 256;
            write(globalIndex % 256, line);
            globalIndex /= 256;
            write(globalIndex % 256, line);
        }
        else {
            write((uint8_t)OpCode::DefGlobal, line);
            write((uint8_t)globalIndex, line);
        }
    }

    size_t Chunk::getLine(size_t index) const
    {
        size_t lastIndex = 0;
        size_t i = 0;
        for (; i < m_lines.size(); i++)
        {
            if (lastIndex + m_lines[i].indexOffset <= index)
                lastIndex += m_lines[i].indexOffset;
            else break;
        }

        return m_lines[i].line;
    }

    size_t Chunk::addConstant(Value value)
    {
        m_constants.emplace_back(value);
        return m_constants.size() - 1;
    }

} // namespace Lux
