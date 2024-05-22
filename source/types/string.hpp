#pragma once
#include "object.hpp"

namespace Lux {

    // TODO: implement Strings that doesn't own buffer
    // TODO: remember that const string can be easly interned
    class String : public Object
    {
    public:
        String() : Object(Object::Type::String) {}
        String(String&& other) noexcept;
        String(const String& other);
        String(const char* str, size_t length);
        ~String();

        static String* create(const char* str, size_t length) { return new String(str, length); }

        const char* cstr() const { return m_buffer; }
        size_t hash() const { return m_hash; }

        bool isNull() const { return m_buffer == nullptr; }

        bool operator==(const String& rhs) const;
        String& operator+=(const String& rhs);
        String& operator=(String&& other) noexcept;
        String& operator=(const String& other);
    private:
        size_t m_size = 0;
        uint32_t m_hash = 0;
        char* m_buffer = nullptr;
    };

} // namespace Lux
