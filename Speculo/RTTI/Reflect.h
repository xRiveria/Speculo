#pragma once
#include <std::string>

namespace Reflect
{
    template <typename T>
    TypeFactory<T> g_TypeFactory;

    template <typename T>
    TypeFactory<T>& Reflect(const std::string& name)
    {
        return g_TypeFactory<T>.ReflectType(name);
    }
}