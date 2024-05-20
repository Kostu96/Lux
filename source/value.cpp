#include "value.hpp"

#include <cstdio>

namespace Lux {

    Value Value::makeNil()
    {
        Value value;
        value.type = Type::Nil;
        return value;
    }

    Value Value::makeBool(bool boolean)
    {
        Value value;
        value.type = Type::Bool;
        value.boolean = boolean;
        return value;
    }

    Value Value::makeNumber(double number)
    {
        Value value;
        value.type = Type::Number;
        value.number = number;
        return value;
    }

    Value Value::makeObject(Object *object)
    {
        Value value;
        value.type = Type::Object;
        value.object = object;
        return value;
    }

    bool Value::operator==(Value rhs) const
    {
        if (type != rhs.type) return false;

        switch (type)
        {
        case Type::Bool:   return boolean == rhs.boolean;
        case Type::Nil:    return true;
        case Type::Number: return number == rhs.number;
        case Type::Object: return *object == *rhs.object;
        }

        return false;
    }

    void printValue(Value value)
    {
        switch (value.type)
        {
        case Value::Type::Bool:
            std::printf(value.boolean ? "true" : "false");
            break;
        case Value::Type::Nil:
            std::printf("nil");
            break;
        case Value::Type::Number:
            std::printf("%g", value.number);
            break;
        case Value::Type::Object:
            printObject(value.object);
        }
        
    }

} // namespace Lux
