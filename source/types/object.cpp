#include "object.hpp"
#include "string.hpp"

#include <cstdio>

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

} // namespace Lux
