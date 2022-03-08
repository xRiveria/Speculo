#ifndef TYPE_DESCRIPTOR_INL
#define TYPE_DESCRIPTOR_INL

#include "TypeDescriptor.hpp"
#include "DataMember.hpp"
#include "Function.hpp"
#include "Constructor.hpp"
#include "Base.hpp"
#include "Conversion.hpp"

namespace Speculo
{
    template <typename Type, typename ...Args>
    void TypeDescriptor::AddConstructor()
    {
        Constructor* constructor = new ConstructorImplementation<Type, Args...>();

        m_Constructors.push_back(constructor);
    }

    template <typename Type, typename ...Args>
    void TypeDescriptor::AddConstructor(Type(*constructorFunction)(Args...))
    {
        Constructor* constructor = new FreeFunctionConstructor<Type, Args...>(constructorFunction);

        m_Constructors.push_back(constructor);
    }

    template <typename B, typename T>
    void TypeDescriptor::AddBase()
    {
        Base* base = new BaseImplementation<B, T>;

        m_Bases.push_back(base);
    }

    template <typename C, typename T>
    void TypeDescriptor::AddDataMember(T C::*dataMemberPtr, const std::string& name)
    {
        DataMember* dataMember = new DataMemberPointer<C, T>(dataMemberPtr, name);

        m_DataMembers.push_back(dataMember);
    }

    template <auto Setter, auto Getter, typename Type>
    void TypeDescriptor::AddDataMember(const std::string& name)
    {
        DataMember* dataMember = new SetGetDataMember<Setter, Getter, Type>(name);

        m_DataMembers.push_back(dataMember);
    }

    template <typename Return, typename ...Args>
    void TypeDescriptor::AddMemberFunction(Return freeFunction(Args...), const std::string& name)
    {
        Function* memberFunction = new FreeFunction<Return, Args...>(freeFunction, name);

        m_MemberFunctions.push_back(memberFunction);
    }

    template <typename C, typename Return, typename ...Args>
    void TypeDescriptor::AddMemberFunction(Return(C::*memberFunction)(Args...), const std::string& name)
    {
        Function* memberFunction = new MemberFunction<C, Return, Args...>(memberFunction, name);

        m_MemberFunctions.push_back(memberFunction);
    }

    template <typename C, typename Return, typename ...Args>
    void TypeDescriptor::AddMemberFunction(Return(C::* memberFunction)(Args...) const, const std::string& name)
    {
        Function* memberFunction = new ConstMemberFunction<C, Return, Args...>(memberFunction, name);

        m_MemberFunctions.push_back(memberFunction);
    }

    template <typename From, typename To>
    void TypeDescriptor::AddConversion()
    {
        Conversion* conversion = new ConversionImplementation<From, To>;

        m_Conversions.push_back(conversion);
    }

    inline const std::string& TypeDescriptor::GetName() const
    {
        return m_Name;
    }

    inline std::vector<Constructor*> TypeDescriptor::GetConstructors() const
    {
        return m_Constructors;
    }

    template <typename ...Args>
    const Constructor* TypeDescriptor::GetConstructor() const
    {
        for (auto* constructor : m_Constructors)
        {
            if (constructor->CanConstruct<Args...>(std::index_sequence_for<Args...>()))
            {
                return constructor;
            }
        }

        return nullptr;
    }

    inline std::vector<Base*> TypeDescriptor::GetBases() const
    {
        return m_Bases;
    }

    template <typename B>
    Base* TypeDescriptor::GetBase() const
    {
        for (auto base : m_Bases)
        {
            if (base->GetType() == Details::Resolve<B>)
            {
                return base;
            }
        }

        return nullptr;
    }

    inline std::vector<DataMember*> TypeDescriptor::GetDataMembers() const
    {
        std::vector<DataMember*> dataMembers(m_DataMembers);

        for (auto* base : m_Bases) // Get members in base classes as well.
        {
            for (auto dataMember : base->GetType()->GetDataMembers())
            {
                dataMembers.push_back(dataMember);
            }
        }

        return dataMembers;
    }

    inline DataMember* TypeDescriptor::GetDataMember(const std::string& name) const
    {
        for (auto* dataMember : m_DataMembers)
        {
            if (dataMember->GetName() == name)
            {
                return dataMember;
            }
        }

        for (auto* base : m_Bases)
        {
            if (auto* baseDataMember = base->GetType()->GetDataMember(name))
            {
                return baseDataMember;
            }
        }

        return nullptr;
    }

    inline std::vector<Function*> TypeDescriptor::GetMemberFunctions() const
    {
        std::vector<Function*> memberFunctions(m_MemberFunctions);

        for (auto* base : m_Bases)
        {
            for (auto memberFunction : base->GetType()->GetMemberFunctions())
            {
                memberFunctions.push_back(memberFunction);
            }
        }

        return memberFunctions;
    }

    inline const Function* TypeDescriptor::GetMemberFunction(const std::string& name) const
    {
        for (auto* memberFunction : m_MemberFunctions)
        {
            if (memberFunction->GetName() == name)
            {
                return memberFunction;
            }
        }

        for (auto* base : m_Bases)
        {
            if (auto* memberFunction = base->GetType()->GetMemberFunction(name))
            {
                return memberFunction;
            }
        }

        return nullptr;
    }

    inline std::vector<Conversion*> TypeDescriptor::GetConversions() const
    {
        return m_Conversions;
    }

    template <typename To>
    Conversion* TypeDescriptor::GetConversion() const
    {
        for (auto conversion : m_Conversions)
        {
            if (conversion->GetToType() == Details::Resolve<To>())
            {
                return conversion;
            }
        }

        return nullptr;
    }
}


#endif