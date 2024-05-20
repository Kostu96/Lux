#pragma once
#include "object.hpp"

namespace Lux {

    struct Value {
        enum class Type {
            Nil,
            Bool,
            Number,
            Object
        };
        
        Type type;
        union {
            double number;
            bool boolean;
            Object *object;
        };

        bool isNil() const { return type == Type::Nil; }
        bool isBool() const { return type == Type::Bool; }
        bool isNumber() const { return type == Type::Number; }
        bool isObject() const { return type == Type::Object; }
        bool isString() const { return isObject() && object->isString(); }

        static Value makeNil();
        static Value makeBool(bool boolean);
        static Value makeNumber(double number);
        static Value makeObject(Object *object);

        operator bool() const { return !(isNil() || (isBool() && !boolean)); }
        bool operator==(Value rhs) const;
        bool operator!=(Value rhs) const { return !(*this == rhs); }
    };

    void printValue(Value value);

} // namespace Lux
