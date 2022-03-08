#ifndef DATA_MEMBER_H
#define DATA_MEMBER_H

#include "TypeDescriptor.hpp"
#include "Any.hpp"
#include <string>

namespace Speculo
{
    class DataMember
    {
    public:
        std::string GetName() const { return m_Name; }
        const TypeDescriptor* GetParent() const { return m_Parent; }
        const TypeDescriptor* GetType() const { return m_Type; }

        virtual void Set(AnyRef objectRef, const Any value) = 0;
        virtual Any Get(Any object) = 0;

    protected:
        DataMember(const std::string& name, const TypeDescriptor* type, const TypeDescriptor* parent) : m_Name(name), m_Type(type), m_Parent(parent) { }

    private:
        std::string m_Name;
        const TypeDescriptor* m_Type;       // Type of Data Member
        const TypeDescriptor* m_Parent;     // Type of Data Member's Class
    };

    template <typename Class, typename Type>
    class DataMemberPointer : public DataMember
    {
    public:
        DataMemberPointer(Type Class::*dataMemberPointer, const std::string name) : DataMember(name, Details::Resolve<Type>(), Details::Resolve<Class>()), m_DataMemberPointer(dataMemberPointer)
        { }

        void Set(AnyRef objectRef, const Any value) override
        {
            SetImplementation(objectRef, value, std::is_const<Type>());
        }

        Any Get(Any object) override
        {
            Class* object = object.TryCast<Class>();

            if (!object)
            {
                throw BadCastException(Details::Resolve<Class>()->GetName(), object.GetType()->GetName());
            }

            return object->*m_DataMemberPointer;
        }

    private:
        void SetImplementation(Any object, const Any value, std::false_type)
        {
            Class* classObject = object.TryCast<Class>();

            if (!classObject)
            {
                throw BadCastException(Details::Resolve<Class>()->GetName(), object.GetType()->GetName(), "object:");
            }

            const Type* casted = nullptr;
            Any val;
            if (casted = value.TryCast<Type>(); !casted)
            {
                val = value.TryConvert<Type>();
                casted = val.TryCast<Type>();
            }

            if (!casted)
            {
                throw BadCastException(Details::Resolve<Type>()->GetName(), value.GetType()->GetName(), "value:");
            }

            classObject->*m_DataMemberPointer = *casted;
        }

        void SetImplementation(Any object, const Any value, std::true_type)
        {
            // Assert that const members cannot be set.
        }

    private:
        Type Class::*m_DataMemberPointer;
    };

    // Helper meta function to get info about functions passed as auto non type parameters (C++ 17).
    template <typename>
    struct FunctionHelper;

    template <typename Return, typename ...Args>
    struct FunctionHelper<Return(Args...)>
    {
        using ReturnType = Return;
        using ParameterTypes = std::tuple<Args...>;
    };

    template <typename Class, typename Return, typename ...Args>
    FunctionHelper<Return(Args...)> ToFunctionHelper(Return(Class::*)(Args...));

    template <typename Class, typename Return, typename ...Args>
    FunctionHelper<Return(Args...)> ToFunctionHelper(Return(Class::*)(Args...) const);

    template <typename Return, typename ...Args>
    FunctionHelper<Return(Args...)> ToFunctionHelper(Return(*)(Args...));

    template <auto Setter, auto Getter, typename Class>
    class SetGetDataMember : public DataMember
    {
    private:
        using MemberType = Details::RawType<typename decltype(ToFunctionHelper(Getter))::ReturnType>;

    public:
        SetGetDataMember(const std::string& name) : DataMember(name, Details::Resolve<MemberType>(), Details::Resolve<Class>()) { }

        void Set(AnyRef objectRef, const Any value) override
        {
            Any a = objectRef;
            Class* classObject = a.TryCast<Class>();

            if (!classObject)
            {
                throw BadCastException(Details::Resolve<Class>()->GetName(), Any(objectRef).GetType()->GetName(), "object:");
            }

            const MemberType* casted = nullptr;
            Any val;

            if (casted = value.TryCast<MemberType>(); !casted)
            {
                val = value.TryConvert<MemberType>();
                casted = val.TryCast<MemberType>();
            }

            if (!casted)
            {
                throw BadCastException(Details::Resolve<MemberType>()->GetName(), value.GetType()->GetName(), "value:");
            }

            if constexpr (std::is_member_function_pointer_v<decltype(Setter)>)
            {
                (classObject->*Setter)(*casted);
            }
            else
            {
                static_assert(std::is_function_v<std::remove_pointer_t<decltype(Setter)>>);
                Setter(*classObject, *casted);
            }
        }

        Any Get(Any object) override
        {
            Class* classObject = object.TryCast<Class>(); // Ensure that the object can be casted to our data member's class.

            if (!classObject)
            {
                throw BadCastException(Details::Resolve<Class>()->GetName(), object.GetType()->GetName());
            }

            if constexpr (std::is_member_function_pointer_v<decltype(Getter)>)
            {
                return (classObject->*Getter)(); // Get!
            }
            else
            {
                static_assert(std::is_function_v<std::remove_pointer_t<decltype(Getter)>>);
                return Getter(*classObject);
            }
        }
    };
}

#endif 