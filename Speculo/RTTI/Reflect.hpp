#ifndef REFLECT_H
#define REFLECT_H

#include "TypeFactory.hpp"
#include <type_traits>
#include <utility>

namespace Speculo
{
    class TypeDescriptor;

    // Reflect takes a string which is the mapped name of the reflected type and returns a TypeFactory.
    // This can be used to add meta objects to the Type Descriptor.
    template <typename Type>
    TypeFactory<Type>& Reflect(const std::string& name)
    {
        return typeFactory<Type>.ReflectType(name);
    }

    // There are 3 ways to get the type descriptor of a type:
    // 1) With a template type parameter.
    // 2) With the name of the type itself (string).
    // 3) With an instance of the object.
    // Each function calls the corresponding internal resolve and returns a const pointer to the type descriptor.
    template <typename Type>
    const TypeDescriptor* Resolve()
    {
        return Details::Resolve<Type>();
    }

    template <typename T, typename = typename std::enable_if<!std::is_convertible<T, std::string>::value>::type>
    const TypeDescriptor* Resolve(T&& object)
    {
        return Details::Resolve(std::forward<T>(object));
    }

    inline const TypeDescriptor* Resolve(const std::string& name)
    {
        if (auto it = Details::GetTypeRegistry().find(name); it != Details::GetTypeRegistry().end())
        {
            return it->second;
        }

        return nullptr;
    }
}

#endif // REFLECT_H