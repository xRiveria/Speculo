#pragma once
#ifndef SIGNAL_H
#define SIGNAL_H

#include "Delegate.hpp"
#include <vector>

namespace Speculo
{ 
    // A signal object contains a vector of delegates, and can bind to many callables that share the same signature.
    #define SIGNAL(SignalType)                                                  typedef Signal<void()> SignalType
    #define SIGNAL_ONE_PARAM(SignalType, Param0)                                typedef Signal<void(Param0)> SignalType
    #define SIGNAL_TWO_PARAM(SignalType, Param0, Param1)                        typedef Signal<void(Param0, Param1)> SignalType
    
    #define SIGNAL_RETURN(SignalType, Return)                                   typedef Signal<Return()> SignalType
    #define SIGNAL_RETURN_ONE_PARAM(SignalType, Return, Param0)                 typedef Signal<Return(Param0)> SignalType
    #define SIGNAL_RETURN_TWO_PARAM(SignalType, Return, Param0, Param1)         typedef Signal<Return(Param0, Param1)> SignalType

    // Signal Primary Class Template
    template <typename Signature>
    class Signal;

    // Signal Partial Class Template for Function Types
    template <typename Return, typename ...Args>
    class Signal<Return(Args...)>
    {
    public:
        template <Return(*FreeFunction)(Args...)>
        void Bind();

        template <typename Type, Return(Type::*PtrToMemberFunction)(Args...)>
        void Bind(Type& instance);

        template <typename Type, Return(Type::*PtrToConstMemberFunction)(Args...) const>
        void Bind(Type& instance);

        template <typename Type>
        void Bind(Type&& functionObject);

        explicit operator bool() const { return !m_Delegates.empty(); }

        void operator()(Args... args)
        {
            for (auto& delegate : m_Delegates)
            {
                delegate(std::forward<Args...>(args)...);
            }
        }

        void Invoke(Args... args)
        {
            for (auto& delegate : m_Delegates)
            {
                delegate.Invoke(std::forward<Args>(args)...);
            }
        }

    private:
        std::vector<Delegate<Return(Args...)>> m_Delegates;
    };

    template <typename Return, typename ...Args>
    template <Return(*FreeFunction)(Args...)>
    void Signal<Return(Args...)>::Bind()
    {
        Delegate<Return(Args...)> delegate;
        delegate.template Bind<FreeFunction>();
        m_Delegates.push_back(delegate);
    }

    template <typename Return, typename ...Args>
    template <typename Type, Return(Type::*PtrToMemberFunction)(Args...)>
    void Signal<Return(Args...)>::Bind(Type& instance)
    {
        Delegate<Return(Args...)> delegate;
        delegate.template Bind<Type, PtrToMemberFunction>(instance);
        m_Delegates.push_back(delegate);
    }

    template <typename Return, typename ...Args>
    template <typename Type, Return(Type::*PtrToConstMemberFunction)(Args...) const>
    void Signal<Return(Args...)>::Bind(Type& instance)
    {
        Delegate<Return(Args...)> delegate;
        delegate.template Bind<Type, PtrToConstMemberFunction>(instance);
        m_Delegates.push_back(delegate);
    }

    template <typename Return, typename ...Args>
    template <typename Type>
    void Signal<Return(Args...)>::Bind(Type&& functionObject)
    {
        Delegate<Return(Args...)> delegate;
        delegate.Bind(std::forward<Type>(functionObject));
        m_Delegates.push_back(delegate);
    }
}

#endif