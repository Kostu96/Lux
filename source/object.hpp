#pragma once

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

    // TODO: implement Strings that doesn't own buffer
    class String : public Object
    {
    public:
        String() = default;
        String(const char *str, size_t length);
        ~String();

        static String *create(const char *str, size_t length) { return new String(str, length); }

        const char *cstr() const { return m_buffer; }

        bool operator==(const String &rhs) const;
        String& operator+=(const String &rhs);

        String(String &) = delete;
        String &operator=(String &) = delete;
    private:
        size_t m_size = 0;
        char *m_buffer = nullptr;
    };

    void printObject(Object *object);

} // namespace Lux
