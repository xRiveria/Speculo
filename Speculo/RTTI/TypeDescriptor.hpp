#pragma once
#ifndef TYPE_DESCRIPTOR_H
#define TYPE_DESCRIPTOR_H

#include <string>
#include <vector>
#include <map>
#include <type_traits>

namespace Speculo
{
    // Forward Declarations (Type Information)
    class DataMember;           // Variables
    class Function;             // Functions
    class Constructor;          // Constructors
    class Base;                 // Base Classes
    class Conversion;           // Type Conversion Operators

    template <typename>
    class TypeFactory;

    class TypeDescriptor;

    // Forward Declarations (For Friend Declarations inside TypeDescriptor)
    namespace Details
    {
        template <typename Type>
        TypeDescriptor* Resolve();

        template <typename Type>
        TypeDescriptor* Resolve(Type&&);
    }

    class TypeDescriptor
    {
        template <typename> friend class TypeFactory;
        template <typename Type> friend TypeDescriptor* Details::Resolve();
        template <typename Type> friend TypeDescriptor* Details::Resolve(Type&&);

    public:
        template <typename Type, typename ...Args>
        void AddConstructor();

        template <typename Type, typename ...Args>
        void AddConstructor(Type(*)(Args...));

        template <typename B, typename T>
        void AddBase();

        template <typename C, typename T>
        void AddDataMember(T C::*dataMember, const std::string& name);

        template <auto Setter, auto Getter, typename Type>
        void AddDataMember(const std::string& name);

        template <typename Return, typename ...Args>
        void AddMemberFunction(Return freeFunction(Args...), const std::string& name);

        template <typename C, typename Return, typename ...Args>
        void AddMemberFunction(Return(C::*memberFunction)(Args...), const std::string& name);

        template <typename C, typename Return, typename ...Args>
        void AddMemberFunction(Return(C::*memberFunction)(Args...) const, const std::string& name);

        template <typename From, typename To>
        void AddConversion();

        // Retrievals
        const std::string& GetName() const;

        std::vector<Constructor*> GetConstructors() const;

        template <typename ...Args>
        const Constructor* GetConstructor() const;

        std::vector<Base*> GetBases() const;

        template <typename B>
        Base* GetBase() const;

        std::vector<DataMember*> GetDataMembers() const;

        DataMember* GetDataMember(const std::string& name) const;

        std::vector<Function*> GetMemberFunctions() const; 
         
        const Function* GetMemberFunction(const std::string& name) const;

        std::vector<Conversion*> GetConversions() const;

        template <typename To>
        Conversion* GetConversion() const;

    private:
        std::string m_Name;
        std::size_t m_Size;

        std::vector<Base*> m_Bases;
        std::vector<Conversion*> m_Conversions;
        std::vector<Constructor*> m_Constructors;
        std::vector<DataMember*> m_DataMembers;
        std::vector<Function*> m_MemberFunctions;

        // C++ Primary Type Categories
        bool m_IsVoid;
        bool m_IsIntegral;
        bool m_IsFloatingPoint;
        bool m_IsArray;
        bool m_IsPointer;
        bool m_IsPointerToDataMember;
        bool m_IsPointerToMemberFunction;
        bool m_IsNullPointer;
        bool m_IsClass;
        bool m_IsUnion;
        bool m_IsEnum;
        bool m_IsFunction;
    };

    namespace Details
    {
        /*
            All CV and reference qualifiers are stripped, but pointers are distinct types (i.e. int and int* have two distinct type descriptors).
        */
        template <typename T>
        using RawType = typename std::remove_cv<std::remove_reference_t<T>>::type; // Always strip reference before stripping const-volatile.

        template <typename T>
        TypeDescriptor& GetTypeDescriptor()
        {
            static TypeDescriptor typeDescriptor;   // Single instance of type descriptor per reflected type.
            return typeDescriptor;
        }

        template <typename Type>
        TypeDescriptor*& GetTypeDescriptorPointer()
        {
            static TypeDescriptor* typeDescriptorPointer = nullptr; // Single instance of type descriptor pointer per reflected type.
            return typeDescriptorPointer;
        }

        inline std::map<std::string, TypeDescriptor*>& GetTypeRegistry()
        {
            static std::map<std::string, TypeDescriptor*> typeRegistry;
            return typeRegistry;
        }

        template <typename Type>
        inline constexpr auto GetTypeSize() -> typename std::enable_if<!std::is_same<RawType<Type>, void>::value, std::size_t>::type
        {
            return sizeof(Type);
        }

        template <typename Type>
        inline constexpr auto GetTypeSize() -> typename std::enable_if<std::is_same<RawType<Type>, void>::value, std::size_t>::type
        {
            return 0U;
        }

        // Internal function template that returns a type descriptor by type.
        template <typename Type>
        TypeDescriptor* Resolve()
        {
            TypeDescriptor*& typeDescriptorPointer = GetTypeDescriptorPointer<RawType<Type>>();

            if (!typeDescriptorPointer) // Create a type descriptor if not present.
            {
                TypeDescriptor& typeDescriptor = GetTypeDescriptor<RawType<Type>>();
                typeDescriptorPointer = &typeDescriptor;

                typeDescriptor.m_Size = GetTypeSize<Type>();
                
                typeDescriptor.m_IsVoid = std::is_void_v<Type>;
                typeDescriptor.m_IsIntegral = std::is_integral_v<Type>;
                typeDescriptor.m_IsFloatingPoint = std::is_floating_point_v<Type>;
                typeDescriptor.m_IsArray = std::is_array_v<Type>;
                typeDescriptor.m_IsPointer = std::is_pointer_v<Type>;
                typeDescriptor.m_IsPointerToDataMember = std::is_member_object_pointer_v<Type>;
                typeDescriptor.m_IsPointerToMemberFunction = std::is_member_function_pointer_v<Type>;
                typeDescriptor.m_IsNullPointer = std::is_null_pointer_v<Type>;
                typeDescriptor.m_IsClass = std::is_class_v<std::remove_pointer_t<Type>>;
                typeDescriptor.m_IsUnion = std::is_union_v<Type>;
                typeDescriptor.m_IsEnum = std::is_enum_v<Type>;
                typeDescriptor.m_IsFunction = std::is_function_v<Type>;
            }

            return typeDescriptorPointer;
        }

        // Internal function template that returns a type descriptor by object.
        template <typename Type>
        TypeDescriptor* Resolve(Type&& object)
        {
            if (!GetTypeDescriptorPointer<RawType<Type>>())
            {
                Resolve<Type>();
            }

            return GetTypeDescriptorPointer<RawType<Type>>();
        }
    }
}

#include "TypeDescriptor.inl"

#endif // TYPE_DESCRIPTOR_H