#include "object.hpp"

#include <cstdio>
#include <cstring>

namespace Lux {

    String *Object::asString()
    {
        return dynamic_cast<String*>(this);
    }

    const String *Object::asString() const
    {
        return dynamic_cast<const String*>(this);
    }

    bool Object::operator==(const Object &rhs) const
    {
        if (m_type != rhs.m_type) return false;

        switch (m_type)
        {
        case Type::String: return *asString() == *rhs.asString();
        }

        return false;
    }

    void printObject(Object *object)
    {
        switch (object->getType())
        {
        case Object::Type::String:
            std::printf("%s", object->asString()->cstr());
            break;
        }
    }

    static uint32_t hashString(const char* key, size_t length) {
        uint32_t hash = 2166136261u;
        for (size_t i = 0; i < length; i++) {
            hash ^= static_cast<uint8_t>(key[i]);
            hash *= 16777619;
        }
        return hash;
    }

    String::String(const char *str, size_t length) :
        Object{ Type::String }
    {
        m_size = length + 1;
        m_buffer = new char[m_size];
        std::memcpy(m_buffer, str, length);
        m_buffer[length] = '\0';
        m_hash = hashString(m_buffer, m_size);
    }

    String::~String()
    {
        delete[] m_buffer;
    }

    bool String::operator==(const String &rhs) const
    {
        return m_hash == rhs.m_hash &&
               m_size == rhs.m_size &&
               std::memcmp(m_buffer, rhs.m_buffer, m_size) == 0;
    }

    String &String::operator+=(const String &rhs)
    {
        size_t newSize = m_size + rhs.m_size - 1;
        char *newBuffer = new char[newSize];
        std::memcpy(newBuffer, m_buffer, m_size - 1);
        std::memcpy(newBuffer + m_size - 1, rhs.m_buffer, rhs.m_size);
        delete[] m_buffer;
        m_size = newSize;
        m_buffer = newBuffer;
        m_hash = hashString(m_buffer, m_size);
        return *this;
    }

} // namespace Lux
