#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <cstddef>

namespace Speculo
{
    // Base class of all type descriptors.
    struct TypeDescriptor
    {
        TypeDescriptor(const char* typeName, size_t typeSize) : m_Name(typeName), m_Size(typeSize) { }
        virtual ~TypeDescriptor() { }
        virtual std::string GetFullName() const { return m_Name; }
        virtual void Dump(const void* typeObject, int indentLevel = 0) const = 0;

        const char* m_Name;
        size_t m_Size;
    };

    // Primary template for finding primitive.
    template <typename T>
    TypeDescriptor* GetPrimitiveDescriptor(); // For primitive types.

    // Helper class to find TypeDescriptors in different ways.
    struct DefaultResolver
    {
        template <typename T>
        static char func(decltype(&T::Reflection));

        template <typename T>
        static int func(...);

        template <typename T>
        struct IsReflected
        {
            enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
        };

        // This version is called if T has a static member named "Reflection".
        template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
        static TypeDescriptor* Get()
        {
            return &T::Reflection;
        }

        // This version is called otherwise as it is a primitive type.
        template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
        static TypeDescriptor* Get()
        {
            return GetPrimitiveDescriptor<T>();
        }
    };

    // Primary class template for finding all TypeDescriptors.
    template <typename T>
    struct TypeResolver
    {
        static TypeDescriptor* Get()
        {
            return DefaultResolver::Get<T>();
        }
    };

    // User defined classes/structs.
    struct TypeDescriptor_Struct : TypeDescriptor
    {
        struct Member
        {
            const char* m_Name;
            size_t m_Offset;
            TypeDescriptor* m_Type;
        };

        TypeDescriptor_Struct(void (*Init)(TypeDescriptor_Struct*)) : TypeDescriptor(nullptr, 0)
        {
            Init(this);
        }

        TypeDescriptor_Struct(const char* typeName, size_t typeSize, const std::initializer_list<Member>& members) : TypeDescriptor(nullptr, 0), m_Members(members)
        {

        }

        virtual void Dump(const void* typeObject, int indentLevel) const override
        {
            std::cout << m_Name << " {" << std::endl;
            for (const Member& member : m_Members)
            {
                std::cout << std::string(4 * (indentLevel + 1), ' ') << member.m_Name << " = ";
                member.m_Type->Dump((char*)typeObject + member.m_Offset, indentLevel + 1);
                std::cout << std::endl;
            }

            std::cout << std::string(4 * indentLevel, ' ') << "}";
        }

        std::vector<Member> m_Members;
    };

#define REFLECT() \
    friend struct Speculo::DefaultResolver; \
    static Speculo::TypeDescriptor_Struct Reflection; \
    static void InitializeReflection(Speculo::TypeDescriptor_Struct*);

#define REFLECT_STRUCT_BEGIN(type) \
    Speculo::TypeDescriptor_Struct type::Reflection{type::InitializeReflection}; \
    void type::InitializeReflection(Speculo::TypeDescriptor_Struct* typeDescriptor) \
    {   \
        using T = type; \
        typeDescriptor->m_Name = #type; \
        typeDescriptor->m_Size = sizeof(T); \
        typeDescriptor->m_Members = {
    
#define REFLECT_STRUCT_MEMBER(name) \
        { #name, offsetof(T, name), Speculo::TypeResolver<decltype(T::name)>::Get() },

#define REFLECT_STRUCT_END() \
        };  \
    }

}