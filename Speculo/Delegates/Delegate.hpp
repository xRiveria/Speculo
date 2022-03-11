#ifndef DELEGATE_H
#define DELEGATE_H

#include <type_traits>
#include <utility>
#include <exception>
#include "Callable.hpp"
#include "Connection.hpp"

namespace Speculo
{
    // Delegate Exceptions
    class DelegateNotBoundException : public std::exception
    {
    public:
        const char* what() const noexcept override
        {
            return "Delegate not bound.";
        }
    };

    class DelegateAlreadyBoundException : public std::exception
    {
    public:
        const char* what() const noexcept override
        {
            return "Delegate already bound.";
        }
    };

    class WrongCallableException : public std::exception
    {
    public:
        const char* what() const noexcept override
        {
            return "Trying to disconnect wrong callable.";
        }
    };

    // Forward declaration (for friend declaration inside Delegate).
    template <typename Signature>
    class Signal;

    // Delegate primary class template (not defined).
    template <typename Signature>
    class Delegate;

    // Namespace scope swap.
    template <typename Return, typename ...Args>
    void Swap(Delegate<Return(Args...)>& delegate1, Delegate<Return(Args...)>& delegate2)
    {
        delegate1.Swap(delegate2);
    }

    template <typename Return, typename ...Args>
    bool operator<(const Delegate<Return(Args...)>& delegate1, const Delegate<Return(Args...)>& delegate2)
    {
        return delegate1.m_Priority < delegate2.m_Priority;
    }

    // Delegate partial class template specialization for function types.
    template <typename Return, typename ...Args>
    class Delegate<Return(Args...)>
    {
        friend class Signal<Return(Args...)>;
        friend bool operator< <>(const Delegate&, const Delegate&);

    public:
        Delegate() : m_CallableWrapper(nullptr) { }
        Delegate(const Delegate& other) = delete;
        Delegate(Delegate&& other);
        ~Delegate();

        Delegate& operator=(const Delegate& other) = delete;
        Delegate& operator=(Delegate&& other);

        template <typename T, typename PtrToMemberFunction>
        std::enable_if_t<std::is_member_function_pointer_v<PtrToMemberFunction>> Bind(T& instance, PtrToMemberFunction ptrToMemberFunction, unsigned int priority = -1);

        template <typename T>
        void Bind(T&& functionObject, unsigned int priority = -1);

        void Swap(Delegate& other);

        explicit operator bool() const { return m_CallableWrapper != nullptr; }

        Return operator()(Args... args) const;

        Return Invoke(Args... args) const;

    private:
        CallableWrapper<Return(Args...)>* m_CallableWrapper;
        unsigned int m_Priority;
    };

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>::Delegate(Delegate&& other) : m_CallableWrapper(other.m_CallableWrapper), m_Priority(other.m_Priority)
    {
        other.m_CallableWrapper = nullptr;
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>::~Delegate()
    {
        delete m_CallableWrapper;
        m_CallableWrapper = nullptr;
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>& Delegate<Return(Args...)>::operator=(Delegate&& other)
    {
        Delegate temporary(std::move(other));
        Swap(temporary);

        return *this;
    }

    template <typename Return, typename ...Args>
    template <typename T, typename PtrToMemberFunction>
    std::enable_if_t<std::is_member_function_pointer_v<PtrToMemberFunction>> Delegate<Return(Args...)>::Bind(T& instance, PtrToMemberFunction ptrToMemberFunction, unsigned int priority)
    {
        if (m_CallableWrapper)
        {
            throw DelegateAlreadyBoundException();
        }

        m_CallableWrapper = new MemberFunctionCallableWrapper<Return(Args...), T, PtrToMemberFunction>(instance, ptrToMemberFunction);
        m_Priority = priority;
    }

    template <typename Return, typename ...Args>
    template <typename T>
    void Delegate<Return(Args...)>::Bind(T&& functionObject, unsigned int priority)
    {
        if (m_CallableWrapper)
        {
            throw DelegateAlreadyBoundException();
        }

        m_CallableWrapper = new FunctionObjectCallableWrapper<Return(Args...), std::remove_reference_t<T>>(std::forward<T>(functionObject));
        m_Priority = priority;
    }

    template <typename Return, typename ...Args>
    void Delegate<Return(Args...)>::Swap(Delegate& other)
    {
        CallableWrapper<Return(Args...)>* callableTemporary = m_CallableWrapper;
        m_CallableWrapper = other.m_CallableWrapper;
        other.m_CallableWrapper = callableTemporary;

        unsigned int priorityTemporary = m_Priority;
        m_Priority = other.m_Priority;
        other.m_Priority = priorityTemporary;
    }

    template <typename Return, typename ...Args>
    Return Delegate<Return(Args...)>::operator()(Args... args) const
    {
        if (!m_CallableWrapper)
        {
            throw DelegateNotBoundException();
        }

        return m_CallableWrapper->Invoke(std::forward<Args>(args)...);
    }

    template <typename Return, typename ...Args>
    Return Delegate<Return(Args...)>::Invoke(Args... args) const
    {
        if (!m_CallableWrapper)
        {
            throw DelegateNotBoundException();
        }

        return m_CallableWrapper->Invoke(std::forward<Args>(args)...);
    }
}


#endif


/* Old API

// Primary Delegate Class Template
    template <typename Signature>
    class Delegate;

    // Partial Delegate Class Template for Function Types
    template <typename Return, typename ...Args>
    class Delegate<Return(Args...)>
    {
    public:
        Delegate();
        Delegate(const Delegate& other);
        Delegate(Delegate&& other);
        ~Delegate();

        Delegate& operator=(const Delegate& other);
        Delegate& operator=(Delegate&& other);

        // Bind free functions (not related to any classes).
        template <Return(*freeFunction)(Args...)>
        void Bind();

        // Bind a class function which is called through the attached class instance.
        template <typename Type, Return(Type::* PtrToMemberFunction)(Args...)>
        void Bind(Type& instance);

        // Bind a const class function.
        template <typename Type, Return(Type::* PtrToConstMemberFunction)(Args...) const>
        void Bind(Type& instance);

        template <typename Type>
        void Bind(Type& functionObject);

        template <typename Type>
        void Bind(Type&& functionObject);

        void Swap(Delegate& other);

        explicit operator bool() const { return m_Function != nullptr; }

        Return operator()(Args... args) { return m_Function(&m_Data, std::forward<Args>(args)...); }
        Return Invoke(Args... args) { return m_Function(&m_Data, std::forward<Args>(args)...); }

    private:
        using Function = Return(*)(void*, Args...);
        using Storage = std::aligned_storage_t<sizeof(void*), alignof(void*)>;

        Function m_Function;
        Storage m_Data;

        bool m_Stored = false;

        using DestroyStorageFunction = void(*)(Delegate*);
        using CopyStorageFunction = void(*)(const Delegate*, Delegate*);
        using MoveStorageFunction = void(*)(Delegate*, Delegate*);

        DestroyStorageFunction m_DestroyStorage = nullptr;
        CopyStorageFunction m_CopyStorage = nullptr;
        MoveStorageFunction m_MoveStorage = nullptr;

        // Helper function templates for special member functions.
        template <typename Type>
        static void DestroyStorage(Delegate* delegate)
        {
            reinterpret_cast<Type*>(&delegate->m_Data)->~Type();
        }

        template <typename Type>
        static void CopyStorage(const Delegate* source, Delegate& destination)
        {
            new(&destination->m_Data) Type(*reinterpret_cast<const Type*>(&source->m_Data));
        }

        template <typename Type>
        static void MoveStorage(Delegate* source, Delegate* destination)
        {
            new(&destination->m_Data) Type(std::move(*reinterpret_cast<Type*>(&source->m_Data)));
        }

        // Stub Functions
        template <Return(*FreeFunction)(Args...)>
        static Return Stub(void* data, Args... args)
        {
            return FreeFunction(std::forward<Args>(args)...);
        }

        template <typename Type, Return(Type::* PtrToMemberFunction)(Args...)>
        static Return Stub(void* data, Args... args)
        {
            Storage* storage = static_cast<Storage*>(data);
            Type* instance = *reinterpret_cast<Type**>(storage);

            return (instance->*PtrToMemberFunction)(std::forward<Args>(args)...);
        }

        template <typename Type, Return(Type::* PtrToConstMemberFunction)(Args...) const>
        static Return Stub(void* data, Args... args)
        {
            Storage* storage = static_cast<Storage*>(data);
            Type* instance = *reinterpret_cast<Type**>(storage);

            return (instance->*PtrToConstMemberFunction)(std::forward<Args>(args)...);
        }

        template <typename Type>
        static Return Stub(void* data, Args... args)
        {
            Storage* storage = static_cast<Storage*>(data);
            Type* instance = *reinterpret_cast<Type**>(storage);

            return (*instance)(std::forward<Args>(args)...);
        }

        template <typename Type, typename>
        static Return Stub(void* data, Args... args)
        {
            Storage* storage = static_cast<Storage*>(data);
            Type* instance = reinterpret_cast<Type*>(storage);

            return (*instance)(std::forward<Args>(args)...);
        }
    };

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>::Delegate()
    {
        new(&m_Data) std::nullptr_t(nullptr);
        m_Function = nullptr;
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>::Delegate(const Delegate& other)
    {
        if (other.m_Stored)
        {
            other.m_CopyStorage(&other, this);

            m_DestroyStorage = other.m_DestroyStorage;
            m_CopyStorage = other.m_CopyStorage;
            m_MoveStorage = other.m_MoveStorage;

            m_Stored = true;
        }
        else
        {
            m_Data = other.m_Data;
        }

        m_Function = other.m_Function;
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>::Delegate(Delegate&& other)
    {
        if (other.m_Stored)
        {
            other.m_MoveStorage(&other, this);

            m_DestroyStorage = other.m_DestroyStorage;
            m_CopyStorage = other.m_CopyStorage;
            m_MoveStorage = other.m_MoveStorage;

            m_Stored = true;
        }
        else
        {
            m_Data = other.m_Data;
        }

        m_Function = other.m_Function;
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>::~Delegate()
    {
        if (m_Stored)
        {
            m_DestroyStorage(this);
        }
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>& Delegate<Return(Args...)>::operator=(const Delegate& other)
    {
        Delegate temporary(other);
        Swap(temporary);

        return *this;
    }

    template <typename Return, typename ...Args>
    Delegate<Return(Args...)>& Delegate<Return(Args...)>::operator=(Delegate&& other)
    {
        Delegate temporary(std::move(other));
        Swap(temporary);

        return *this;
    }

    template <typename Return, typename ...Args>
    template <Return(*FreeFunction)(Args...)>
    void Delegate<Return(Args...)>::Bind()
    {
        new(&m_Data) std::nullptr_t(nullptr);
        m_Function = &Stub<FreeFunction>;
    }

    template <typename Return, typename ...Args>
    template <typename Type, Return(Type::*PtrToMemberFunction)(Args...)>
    void Delegate<Return(Args...)>::Bind(Type& instance)
    {
        new(&m_Data) Type* (&instance);
        m_Function = &Stub<Type, PtrToMemberFunction>;
    }

    template <typename Return, typename ...Args>
    template <typename Type, Return(Type::*PtrToConstMemberFunction)(Args...) const>
    void Delegate<Return(Args...)>::Bind(Type& instance)
    {
        new(&m_Data) Type* (&instance);
        m_Function = &Stub<Type, PtrToConstMemberFunction>;
    }

    template <typename Return, typename ...Args>
    template <typename Type>
    void Delegate<Return(Args...)>::Bind(Type& functionObject)
    {
        new(&m_Data) Type* (&functionObject);
        m_Function = &Stub<Type>;
    }

    template <typename Return, typename ...Args>
    template <typename Type>
    void Delegate<Return(Args...)>::Bind(Type&& functionObject)
    {
        static_assert(sizeof(Type) <= sizeof(void*));

        new(&m_Data) Type(std::move(functionObject));

        m_Function = &Stub<Type, Type>;

        m_DestroyStorage = &DestroyStorage<Type>;
        m_CopyStorage = &CopyStorage<Type>;
        m_MoveStorage = &MoveStorage<Type>;

        m_Stored = true;
    }

    template <typename Return, typename ...Args>
    void Delegate<Return(Args...)>::Swap(Delegate& other)
    {
        if (other.m_Stored)
        {
            other.m_CopyStorage(&other, this);
        }
        else
        {
            m_Data = other.m_Data;
        }

        Function temporaryFunction = m_Function;
        m_Function = other.m_Function;
        other.m_Function = temporaryFunction;

        bool temporaryStored = m_Stored;
        m_Stored = other.m_Stored;
        other.m_Stored = temporaryStored;

        DestroyStorageFunction temporaryDestroy = m_DestroyStorage;
        m_DestroyStorage = other.m_DestroyStorage;
        other.m_DestroyStorage = temporaryDestroy;

        CopyStorageFunction temporaryCopy = m_CopyStorage;
        m_CopyStorage = other.m_CopyStorage;
        other.m_CopyStorage = temporaryCopy;

        MoveStorageFunction temporaryMove = m_MoveStorage;
        m_MoveStorage = other.m_MoveStorage;
        other.m_MoveStorage = temporaryMove;
    }
*/