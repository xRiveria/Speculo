#pragma once
#include "Reflect.h"

namespace Speculo
{
    /*
        The type descriptor of every primitive "built-in" type, whether its an int, double, std::string or something else, is found using the 
        GetPrimitiveDescriptor<T>() function template.
    */

    struct TypeDescriptor_Double : TypeDescriptor
    {
        TypeDescriptor_Double() : TypeDescriptor("double", sizeof(double)) { }
        virtual void Dump(const void* typeObject, int /* Unused */) const override
        {
            std::cout << "double{" << *(const double*)typeObject << "}";
        }
    };

    struct TypeDescriptor_Int : TypeDescriptor
    {
        TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int)) { }
        virtual void Dump(const void* typeObject, int /* Unused */) const override
        {
            std::cout << "int{" << *(const int*)typeObject << "}";
        }
    };

    struct TypeDescriptor_StdString : TypeDescriptor
    {
        TypeDescriptor_StdString() : TypeDescriptor("std::string", sizeof(std::string)) { }
        virtual void Dump(const void* typeObject, int /* Unused */) const override
        {
            std::cout << "std::string{\"" << *(const std::string*)typeObject << "\"}";
        }
    };

    template<>
    TypeDescriptor* GetPrimitiveDescriptor<double>()
    {
        static TypeDescriptor_Double m_TypeDescriptor;
        return &m_TypeDescriptor;
    }

    template <>
    TypeDescriptor* GetPrimitiveDescriptor<int>()
    {
        static TypeDescriptor_Int m_TypeDescriptor;
        return &m_TypeDescriptor;
    }

    template <>
    TypeDescriptor* GetPrimitiveDescriptor<std::string>()
    {
        static TypeDescriptor_StdString m_TypeDescriptor;
        return &m_TypeDescriptor;
    }

}