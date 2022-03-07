#pragma once
#ifndef TYPE_FACTORY_H
#define TYPE_FACTORY_H

#include <string>
#include "TypeDescriptor.hpp"

namespace Speculo
{
    // Forward Declaration
    template <typename Type>
    class TypeFactory;

    template <typename Type>
    TypeFactory<Type> typeFactory; // One type factory per type.

    template <typename Type>
    class TypeFactory
    {
    public:
        TypeFactory& ReflectType(const std::string& name)
        {
            TypeDescriptor* typeDescriptor = Details::Resolve<Type>();

            typeDescriptor->m_Name = name;
            Details::GetTypeRegistry()[name] = typeDescriptor;

            return *this;
        }

        template <typename ...Args>
        TypeFactory& AddConstructor()
        {
            Details::Resolve<Type>()->template AddConstructor<Type, ...Args>();

            return *this;
        }

        template <typename ...Args>
        TypeFactory& AddConstructor(Type(*ConstructorFunction)(Args...))
        {
            Details::Resolve<Type>()->template AddConstructor<Type, Args...>(ConstructorFunction);

            return *this;
        }

        template <typename Base>
        TypeFactory& AddBase()
        {
            static_Assert(std::is_base_of<Base, Type>::value); // Base must be a base class of Type.

            Details::Resolve<Type>()->template AddBase<Base, Type>();

            return *this;
        }

        template <typename T, typename U = Type> // Default template type parameter to allow for non-class types.
        TypeFactory& AddDataMember(T U::*dataMemberPointer, const std::string& name)
        {
            Details::Resolve<Type>()->template AddDataMember(dataMemberPointer, name);

            return *this;
        }

        template <auto Setter, auto Getter>
        TypeFactory& AddDataMember(const std::string& name)
        {
            Details::Resolve<Type>()->template AddDataMember<Setter, Getter, Type>(name);
            
            return *this;
        }

        template <typename Return, typename ...Args>
        TypeFactory& AddMemberFunction(Return(*freeFunction)(Args...), const std::string name)
        {
            Details::Resolve<Type>()->template AddMemberFunction(freeFunction, name);

            return *this;
        }

        template <typename Return, typename ...Args, typename U = Type>
        TypeFactory& AddMemberFunction(Return(*U::*memberFunction)(Args...), const std::string& name)
        {
            Details::Resolve<Type>()->template AddMemberFunction(memberFunction, name);

            return *this;
        }

        template <typename Return, typename ...Args, typename U = Type>
        TypeFactory& AddMemberFunction(Return(U::*constMemberFunction)(Args...) const, const std::string& name)
        {
            Details::Resolve<Type>()->template AddMemberFunction(constMemberFunction, name);

            return *this;
        }

        template <typename To>
        TypeFactory& AddConversion()
        {
            static_assert(std::is_convertible<Type, To>::value); // A conversion from Type -> To must exist.

            Details::Resolve<Type>()->template AddConversion<Type, To>();

            return *this;
        }
    };
}

#endif TYPE_FACTORY_H