#include "string.hpp"

#include <cstring>

namespace Lux {

    static uint32_t hashString(const char* key, size_t length) {
        uint32_t hash = 2166136261u;
        for (size_t i = 0; i < length; i++) {
            hash ^= static_cast<uint8_t>(key[i]);
            hash *= 16777619;
        }
        return hash;
    }

    String::String(String&& other) noexcept :
        Object{ Type::String },
        m_size{ other.m_size },
        m_hash{ other.m_hash },
        m_buffer{ other.m_buffer }
    {
        other.m_buffer = nullptr;
        other.m_hash = 0;
        other.m_size = 0;
    }

    String::String(const String& other) :
        Object{ Type::String },
        m_size{ other.m_size },
        m_hash{ other.m_hash },
        m_buffer{ new char[m_size] }
    {
        std::memcpy(m_buffer, other.m_buffer, m_size);
    }

    String::String(const char* str, size_t length) :
        Object{ Type::String },
        m_size{ length + 1},
        m_buffer{ new char[m_size] }
    {
        std::memcpy(m_buffer, str, length);
        m_buffer[length] = '\0';
        m_hash = hashString(m_buffer, m_size);
    }

    String::~String()
    {
        delete[] m_buffer;
    }

    bool String::operator==(const String& rhs) const
    {
        return m_hash == rhs.m_hash &&
               m_size == rhs.m_size &&
               std::memcmp(m_buffer, rhs.m_buffer, m_size) == 0;
    }

    String& String::operator+=(const String& rhs)
    {
        size_t newSize = m_size + rhs.m_size - 1;
        char* newBuffer = new char[newSize];
        std::memcpy(newBuffer, m_buffer, m_size - 1);
        std::memcpy(newBuffer + m_size - 1, rhs.m_buffer, rhs.m_size);
        delete[] m_buffer;
        m_size = newSize;
        m_buffer = newBuffer;
        m_hash = hashString(m_buffer, m_size);
        return *this;
    }

    String& String::operator=(String&& other) noexcept
    {
        if (this == &other) return *this;

        m_size = other.m_size;
        m_hash = other.m_hash;
        m_buffer = other.m_buffer;

        other.m_buffer = nullptr;
        other.m_hash = 0;
        other.m_size = 0;

        return *this;
    }

    String& String::operator=(const String& other)
    {
        if (this == &other) return *this;
        if (m_size != other.m_size)
        {
            m_size = other.m_size;
            delete[] m_buffer;
            m_buffer = new char[m_size];
        }
        m_hash = other.m_hash;
        std::memcpy(m_buffer, other.m_buffer, m_size);
        return *this;
    }

} // namespace Lux
