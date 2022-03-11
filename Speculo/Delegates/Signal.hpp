#pragma once
#ifndef SIGNAL_H
#define SIGNAL_H

#include "Delegate.hpp"
#include <queue>
#include <type_traits>

namespace Speculo
{ 
    #define SIGNAL(SignalType)                                                  typedef Signal<void()> SignalType
    #define SIGNAL_ONE_PARAM(SignalType, Param0)                                typedef Signal<void(Param0)> SignalType
    #define SIGNAL_TWO_PARAM(SignalType, Param0, Param1)                        typedef Signal<void(Param0, Param1)> SignalType
    
    #define SIGNAL_RETURN(SignalType, Return)                                   typedef Signal<Return()> SignalType
    #define SIGNAL_RETURN_ONE_PARAM(SignalType, Return, Param0)                 typedef Signal<Return(Param0)> SignalType
    #define SIGNAL_RETURN_TWO_PARAM(SignalType, Return, Param0, Param1)         typedef Signal<Return(Param0, Param1)> SignalType

    // Signal Primary Class Template
    template <typename Signature>
    class Signal;

    // Signal Partial Class Template Specialization for Function Types
    template <typename Return, typename ...Args>
    class Signal<Return(Args...)>
    {
        friend class Connection;

    public:
        template <typename T, typename PtrToMemberFunction>
        std::enable_if_t<std::is_member_function_pointer_v<PtrToMemberFunction>, Connection> Bind(T& instance, PtrToMemberFunction ptrToMemberFunction, unsigned int priority = -1);

        template <typename T>
        Connection Bind(T&& functionObject, unsigned int priority = -1);

        explicit operator bool() const { return !m_Delegates.empty(); }

        void operator()(Args... args);

        void Invoke(Args... args);

        // Call all delegates until F doesn't return true.
        template <typename F>
        void operator()(const F& function, Args... args);

        // Call all delegates until F doesn't return true.
        template <typename F>
        void Invoke(const F& function, Args... args);

    private:
        void Unbind(CallableWrapper<Return(Args...)>* callableWrapper);

    private:
        std::priority_queue<Delegate<Return(Args...)>> m_Delegates;
    };

    template <typename Return, typename ...Args>
    template <typename T, typename PtrToMemberFunction>
    std::enable_if_t<std::is_member_function_pointer_v<PtrToMemberFunction>, Connection> Signal<Return(Args...)>::Bind(T& instance, PtrToMemberFunction ptrToMemberFunction, unsigned int priority)
    {
        Delegate<Return(Args...)> delegate;
        delegate.Bind(instance, ptrToMemberFunction, priority);
        CallableWrapper<Return(Args...)>* callable = delegate.m_CallableWrapper;
        m_Delegates.push(std::move(delegate));

        return Connection(this, callable);
    }

    template <typename Return, typename ...Args>
    template <typename T>
    Connection Signal<Return(Args...)>::Bind(T&& functionObject, unsigned int priority)
    {
        Delegate<Return(Args...)> delegate;
        delegate.Bind(std::forward<T>(functionObject), priority);
        CallableWrapper<Return(Args...)>* callable = delegate.m_CallableWrapper;
        m_Delegates.push(std::move(delegate));

        return Connection(this, callable);
    }

    // Unbind specific functions.
    template <typename Return, typename ...Args>
    void Signal<Return(Args...)>::Unbind(CallableWrapper<Return(Args...)>* callableWrapper)
    {
        std::priority_queue<Delegate<Return(Args...)>> queue;

        while (!m_Delegates.empty())
        {
            if (m_Delegates.top().m_CallableWrapper == callableWrapper)
            {
                m_Delegates.pop();
            }
            else
            {
                queue.push(std::move(const_cast<Delegate<Return(Args...)>&>(m_Delegates.top())));
                m_Delegates.pop();
            }
        }

        while (!queue.empty())
        {
            m_Delegates.push(std::move(const_cast<Delegate<Return(Args...)>&>(queue.top())));
            queue.pop();
        }
    }

    template <typename Return, typename ...Args>
    void Signal<Return(Args...)>::operator()(Args... args)
    {
        Invoke(std::forward<Args>(args)...);
    }

    template <typename Return, typename ...Args>
    void Signal<Return(Args...)>::Invoke(Args... args)
    {
        std::priority_queue<Delegate<Return(Args...)>> queue;

        while (!m_Delegates.empty())
        {
            m_Delegates.top()(std::forward<Args>(args)...);

            queue.push(std::move(const_cast<Delegate<Return(Args...)>&>(m_Delegates.top())));
            m_Delegates.pop();
        }   

        while (!queue.empty())
        {
            m_Delegates.push(std::move(const_cast<Delegate<Return(Args...)>&>(queue.top())));
            queue.pop();
        }
    }

    template <typename Return, typename ...Args>
    template <typename F>
    void Signal<Return(Args...)>::operator()(const F& function, Args... args)
    {
        Invoke(function, std::forward<Args>(args)...);
    }

    template <typename Return, typename ...Args>
    template <typename F>
    void Signal<Return(Args...)>::Invoke(const F& function, Args... args)
    {
        std::priority_queue<Delegate<Return(Args...)>> queue;

        while (!m_Delegates.empty())
        {
            if (function(m_Delegates.top()(std::forward<Args>(args)...)))
            {
                queue.push(std::move(const_cast<Delegate<Return(Args...)>&>(m_Delegates.top())));
                m_Delegates.pop();

                break;
            }
            else
            {
                queue.push(std::move(const_cast<Delegate<Return(Args...)>&>(m_Delegates.top())));
                m_Delegates.pop();
            }
        }

        while (!queue.empty())
        {
            m_Delegates.push(std::move(const_cast<Delegate<Return(Args...)>&>(queue.top())));
            queue.pop();
        }
    }
}

#endif



/* Old API

    // A signal object contains a vector of delegates, and can bind to many callables that share the same signature.

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

*/