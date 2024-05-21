#pragma once
#include "common.hpp"

#include <xhash>

namespace Lux {

    class String;

    class Object
    {
    public:
        enum class Type {
            String
        };

        explicit Object(Type type) : m_type{ type } {}
        virtual ~Object() = default;

        Type getType() const { return m_type; }

        bool isString() const { return m_type == Type::String; }
        String *asString();
        const String *asString() const;

        bool operator==(const Object &rhs) const;
    private:
        Type m_type;
    };

    void printObject(Object *object);

    // TODO: implement Strings that doesn't own buffer
    // TODO: remember that const string can be easly interned
    class String : public Object
    {
    public:
        String() = default;
        String(const char *str, size_t length);
        ~String();

        static String *create(const char *str, size_t length) { return new String(str, length); }

        const char *cstr() const { return m_buffer; }
        size_t hash() const { return m_hash; }

        bool operator==(const String &rhs) const;
        String& operator+=(const String &rhs);

        String(String &) = delete;
        String &operator=(String &) = delete;
    private:
        size_t m_size = 0;
        uint32_t m_hash;
        char *m_buffer = nullptr;
    };

} // namespace Lux

template <>
struct std::hash<Lux::String>
{
    size_t operator()(const Lux::String& str) const noexcept { return str.hash(); }
};
