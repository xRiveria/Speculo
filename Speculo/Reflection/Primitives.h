#pragma once
#include "Reflect.h"

namespace Speculo
{
    template <typename T, std::enable_if<std::is_integral<T>::value, int>::type = 0>
    T ReturnItem(T value)
    {
        T thing;
        return T;
    }

    void Derp()
    {
        ReturnItem(5);
    }


    struct TypeDescriptor_Int : TypeDescriptor
    {
        TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int)) { }
        virtual void Dump(const void* typeObject, int /* Unused */) const override
        {
            std::cout << "int{" << *(const int*)typeObject << "}";
        }
    };

    template <>
    TypeDescriptor* GetPrimitiveDescriptor<int>()
    {
        static TypeDescriptor_Int m_TypeDescriptor;
        return &m_TypeDescriptor;
    }

    struct TypeDescriptor_StdString : TypeDescriptor
    {
        TypeDescriptor_StdString() : TypeDescriptor("std::string", sizeof(std::string)) { }
        virtual void Dump(const void* typeObject, int /* Unused */) const override
        {
            std::cout << "std::string{\"" << *(const std::string*)typeObject << "\"}";
        }
    };

    template <>
    TypeDescriptor* GetPrimitiveDescriptor<std::string>()
    {
        static TypeDescriptor_StdString m_TypeDescriptor;
        return &m_TypeDescriptor;
    }
}