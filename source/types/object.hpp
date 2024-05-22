#pragma once
#include "common.hpp"

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

} // namespace Lux
