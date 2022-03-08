#ifndef MEMBER_FUNCTION_H
#define MEMBER_FUNCTION_H

#include <string>
#include <vector>
#include <tuple>
#include "TypeDescriptor.hpp"
#include "Any.hpp"

namespace Speculo
{
    class Function
    {
    public:
        std::string GetName() const { return m_Name; }
        const TypeDescriptor* GetParent() const { return m_Parent; }

        template <typename ...Args>
        Any Invoke(AnyRef object, Args&& ...args) const
        {
            if (sizeof...(Args) == m_ParameterTypes.size())
            {
                std::vector<Any> anyArgs{ Any(std::forward<Args>(args))... };
                return InvokeImplementation(object, anyArgs);
            }

            return Any();
        }

        const TypeDescriptor* GetReturnType() const
        {
            return m_ReturnType;
        }

        std::vector<const TypeDescriptor*> GetParameterTypes() const
        {
            return m_ParameterTypes;
        }

        const TypeDescriptor* GetParameterType(size_t index) const
        {
            return m_ParameterTypes[index];
        }

        std::size_t GetParameterCount() const
        {
            return m_ParameterTypes.size();
        }
        
    protected:
        Function(const std::string& name, const TypeDescriptor* parent, const TypeDescriptor* returnType, const std::vector<const TypeDescriptor*> parameterTypes)
            : m_Name(name), m_Parent(parent), m_ReturnType(returnType), m_ParameterTypes(parameterTypes) {}
            
        const TypeDescriptor* m_ReturnType;
        std::vector<const TypeDescriptor*> m_ParameterTypes;

    private:
        virtual Any InvokeImplementation(Any object, std::vector<Any>& args) const = 0;

        std::string m_Name;
        const TypeDescriptor* const m_Parent;
    };

    template <typename Return, typename ...Args>
    class FreeFunction : public Function
    {
    private:
        using FunctionPtr = Return(*)(Args...);

    public:
        FreeFunction(FunctionPtr freeFunctionPtr, const std::string& name) : Function(name, nullptr, Details::Resolve<Return>(),
                                                                           { Details::Resolve<std::remove_cv_t<std::remove_reference_t<Args>>>()... }),
                                                                           m_FreeFunctionPtr(freeFunctionPtr) { }

    private:
        template <size_t ...Indices>
        Any InvokeImplementation(std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ (std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::remove_reference_t<Args>>>())... };
            argsTuple = std::make_tuple(convertedArgs[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if ((std::get<Indices>(argsTuple) && ...)) // All arguments are valid.
            {
                if constexpr (std::is_reference_v<Return>)
                {
                    return AnyRef(m_FreeFunctionPtr(*std::get<Indices>(argsTuple)...));
                }
                else
                {
                    return m_FreeFunctionPtr(*std::get<Indices>(argsTuple)...);
                }
            }
            else
            {
                return Any();
            }
        }

        Any InvokeImplementation(Any, std::vector<Any>&args) const override
        {
            return InvokeImplementation(args, std::index_sequence_for<Args...>());
        }
        
        private:
            FunctionPtr m_FreeFunctionPtr;      
    };

    template <typename ...Args>
    class FreeFunction<void, Args...> : public Function
    {
    private:
        using FunctionPtr = void(*)(Args...);

    public:
        FreeFunction(FunctionPtr freeFunctionPtr, const std::string& name) : Function(name, nullptr, Details::Resolve<void>(), { Details::Resolve<std::remove_cv_t<std::remove_reference_t<Args>>>()... }),
                                                                             m_FreeFunctionPtr(freeFunctionPtr) { }

    private:
        template <size_t ...Indices>
        Any InvokeImplementation(std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::add_lvalue_reference_t<Args>>>()... };
            argsTuple = std::make_tuple(convertedArgs[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if ((std::get<Indices>(argsTuple) && ...)) // All arguments are valid.
            {
                m_FreeFunctionPtr(*std::get<Indices>(argsTuple)...);
            }

            return Any();
        }

        Any InvokeImplementation(Any, std::vector<Any>& args) const override
        {
            return InvokeImplementation(args, std::index_sequence_for<Args...>());
        }

    private:
        FunctionPtr m_FreeFunctionPtr;
    };

    template <typename C, typename Return, typename ...Args>
    class MemberFunction : public Function
    {
    private:
        using MemberFunctionPtr = Return(C::*)(Args...);

    public:
        MemberFunction(MemberFunctionPtr memberFunction, const std::string& name) : Function(name, Details::Resolve<C>(), Details::Resolve<Return>(),
                                                                                    { Details::Resolve<std::remove_cv_t<std::remove_reference_t<Args>>>()... }),
                                                                                    m_MemberFunctionPtr(memberFunction) { }

    private:
        template <size_t ...Indices>
        Any InvokeImplementation(Any object, std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ (std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::remove_reference_t<Args>>>())... };
            argsTuple = std::make_tuple(convertedArgs[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if (C* classObject = object.TryCast<C>(); (std::get<Indices>(argsTuple) && ...) && classObject) // Object can be casted and is valid + all arguments valid
            {
                if constexpr (std::is_reference_v<Return>)
                {
                    return AnyRef((classObject->*m_MemberFunctionPtr)(*std::get<Indices>(argsTuple)...));
                }
                else
                {
                    return (classObject->*m_MemberFunctionPtr)(*std::get<Indices>(argsTuple)...);
                }
            }
            else
            {
                return Any();
            }
        }

        Any InvokeImplementation(Any object, std::vector<Any>& args) const override
        {
            return InvokeImplementation(object, args, std::make_index_sequence<sizeof...(Args)>());
        }

    private:
        MemberFunctionPtr m_MemberFunctionPtr;
    };

    template <typename C, typename ...Args>
    class MemberFunction<C, void, Args...> : public Function
    {
    private:
        using MemberFunctionPtr = void(C::*)(Args...);

    public:
        MemberFunction(MemberFunctionPtr memberFunction, const std::string& name) : Function(name, Details::Resolve<C>(), Details::Resolve<void>(),
                                                                                   { Details::Resolve<std::remove_cv_t<std::remove_reference_t<Args>>>()... }), m_MemberFunctionPtr(memberFunction) { }

    private:
        template <size_t ...Indices>
        Any InvokeImplementation(Any object, std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ (std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::remove_reference_t<Args>>>())... };
            argsTuple = std::make_tuple(convertedArgs[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if (C* classObject = object.TryCast<C>(); (std::get<Indices>(argsTuple) && ...) && classObject)
            {
                (classObject->*m_MemberFunctionPtr)(*std::get<Indices>(argsTuple)...);
            }

            return Any();
        }

        Any InvokeImplementation(Any object, std::vector<Any>& args) const override
        {
            return InvokeImplementation(object, args, std::make_index_sequence<sizeof...(Args)>());
        }

    private:
        MemberFunctionPtr m_MemberFunctionPtr;
    };

    template <typename C, typename Return, typename ...Args>
    class ConstMemberFunction : public Function
    {
    private:
        using ConstMemberFunctionPtr = Return(C::*)(Args...) const;

    public:
        ConstMemberFunction(ConstMemberFunctionPtr constMemberFunction, const std::string& name)
                          : Function(name, Details::Resolve<C>(), Details::Resolve<Return>(), { Details::Resolve<std::remove_cv_t<std::remove_reference_t<Args>>>()...}),
                            m_ConstMemberFunctionPtr(constMemberFunction) { }

    private:
        template <size_t ...Indices>
        Any InvokeImplementation(Any object, std::vector<Any>& args, std::index_sequence<Indices...> indexSequence) const
        {
            std::tuple argsTuple = std::make_tuple(args[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            std::vector<Any> convertedArgs{ (std::get<Indices>(argsTuple) ? AnyRef(*std::get<Indices>(argsTuple)) : args[Indices].TryConvert<std::remove_cv_t<std::remove_reference_t<Args>>>())... };
            argsTuple = std::make_tuple(convertedArgs[Indices].TryCast<std::remove_cv_t<std::remove_reference_t<Args>>>()...);

            if (C*  = object.TryCast<C>(); classObject && (std::get<Indices>(argsTuple) && ...))
            {
                if constexpr (std::is_void<Return>::value)
                {
                    (classObject->*m_ConstMemberFunctionPtr)(*std::get<Indices>(argsTuple)...);
                    return Any();
                }
                else
                {
                    if constexpr (std::is_reference_v<Return>)
                    {
                        return AnyRef((classObject->m_ConstMemberFunctionPtr)(*std::get<Indices>(argsTuple)...));
                    }
                    else
                    {
                        return (classObject->*m_ConstMemberFunctionPtr)(*std::get<Indices>(argsTuple)...);
                    }
                }
            }
            else
            {
                return Any();
            }
        }

        Any InvokeImplementation(Any object, std::vector<Any>& args) const override
        {
            return InvokeImplementation(object, args, std::make_index_sequence<sizeof...(Args)>());
        }

    private:
        ConstMemberFunctionPtr m_ConstMemberFunctionPtr;

    };

}


#endif // MEMBER_FUNCTION_H