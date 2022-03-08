#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include <vector>
#include <tuple>
#include "TypeDescriptor.hpp"
#include "Any.hpp"
#include "Conversion.hpp"
#include "Base.hpp"

namespace Speculo
{
    inline bool CanCastOrConvert(const TypeDescriptor* from, const TypeDescriptor* to)
    {
        if (from == to)
        {
            return true;
        }

        // If To is a base class type of From.
        for (auto* base : from->GetBases())
        {
            if (base->GetType() == to)
            {
                return true;
            }
        }

        // If To is a derived class of From. 
        for (auto* conversion : from->GetConversions())
        {
            if (conversion->GetToType() == to)
            {
                return true;
            }
        }

        return false;
    }
    
    class Constructor
    {
    public:
        Any NewInstance(std::vector<Any>& args)
        {
            if (args.size() == m_ParameterTypes.size())
            {
                return NewInstanceImplementation(args);
            }

            return Any();
        }

        template <typename ...Args>
        Any NewInstance(Args... args) const
        {
            if (sizeof...(Args) == m_ParameterTypes.size())
            {
                std::vector<Any> argsAny = std::vector<Any>({ Any(std::forward<Args>(args))... });
                return NewInstanceImplementation(argsAny);
            }

            return Any();
        }

        const TypeDescriptor* GetParent() const
        {
            return m_Parent;
        }

        const TypeDescriptor* GetParameterType(size_t index) const
        {
            return m_ParameterTypes[index];
        }

        size_t GetParameterCount() const
        {
            return m_ParameterTypes.size();
        }

        template <typename ...Args, size_t ...Indices>
        bool CanConstruct(std::index_sequence<Indices...> indexSequence = std::index_sequence_for<Args...>()) const
        {
            return GetParameterCount() == sizeof...(Args) && ((Speculo::CanCastOrConvert(Details::Resolve<Args>(), GetParameterType(Indices))) && ...);
        }

    protected:
        Constructor(TypeDescriptor* parent, const std::vector<const TypeDescriptor*> parameterTypes) : m_Parent(parent), m_ParameterTypes(parameterTypes) {}

    private:
        virtual Any NewInstanceImplementation(std::vector<Any>& args) const = 0;

    private:
        TypeDescriptor* m_Parent;
        std::vector<const TypeDescriptor*> m_ParameterTypes;
    };

    // One for each constructor type.
    template <typename Type, typename ...Args>
    class ConstructorImplementation : public Constructor
    {
    public:
        ConstructorImplementation() : Constructor(Details::Resolve<Details::RawType<Type>>(), { Details::Resolve<Details::RawType<Args>>()... }) { }

    private:
        template <size_t ...Indices>
        Any NewInstanceImplementation(std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ (std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::remove_reference_t<Args>>>())... };
            argsTuple = std::make_tuple(convertedArgs[indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if ((std::get<Indices>(argsTuple) && ...))
            {
                return Type(*std::get<Indices>(argsTuple)...);
            }

            return Any();
        }

        Any NewInstanceImplementation(std::vector<Any>& args) const override
        {
            return NewInstanceImplementation(args, std::make_index_sequence<sizeof...(Args)>());
        }
    };

    template <typename Type, typename ...Args>
    class FreeFunctionConstructor : public Constructor
    {
    private:
        typedef Type(*ConstructorFunction)(Args...);

    public:
        FreeFunctionConstructor(ConstructorFunction constructorFunction) : 
                                Constructor(Details::Resolve<Details::RawType<T>>(), { Details::Resolve<Details::RawType<Args>>()... }), m_ConstructorFunction(constructorFunction)
        { }

    private:
        template <size_t ...Indices>
        Any NewInstanceImplementation(std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ (std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::remove_reference_t<Args>>>())... };
            argsTuple = std::make_tuple(convertedArgs[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if ((std::get<Indices>(argsTuple) && ...)) // Fold
            {
                return m_ConstructorFunction(*std::get<Indices>(argsTuple)...);
            }

            return Any();
        }

        Any NewInstanceImplementation(std::vector<Any>& args) const override
        {
            return NewInstanceImplementation(args, std::make_index_sequence<sizeof...(Args)>());
        }

    private:
        ConstructorFunction m_ConstructorFunction;
    };
}

#endif