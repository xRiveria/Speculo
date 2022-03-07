#pragma once
#ifndef META_ANY_H
#define META_ANY_H

#include "TypeDescriptor.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>
#include <string>
#include <exception>

namespace Speculo
{
    namespace Details
    {
        template <std::size_t Size, std::size_t Alignment = alignof(std::max_align_t)>
        struct AlignedStorage
        {
            static_assert(Size >= sizeof(void*), "Storage must be at least the size of a pointer.");

            struct Type
            {
                alignas(Alignment) unsigned char Storage[Size];
            };
        };

        template <std::size_t Size, std::size_t Alignment = alignof(std::max_align_t)>
        using AlignedStorageT = typename AlignedStorage<Size, Alignment>::Type;
    }

    class BadCastException : public std::exception
    {
    public:
        BadCastException(const std::string& retrieved, const std::string& contained, const std::string& message = "")
            : m_Message(message + " wrong type from Get: tried to get " + retrieved + ", contained " + contained) { }

        const char* what() const noexcept override
        {
            return m_Message.c_str();
        }

    private:
        std::string m_Message;
    };

    // Forward Declaration
    template <std::size_t>
    class BasicAny;

    using Any = BasicAny<sizeof(void*)>;

    // AnyRef is an object that contains a pointer to any object but does not manage its lifetime.
    class AnyRef
    {
        template <std::size_t> friend class BasicAny;

    public:
        AnyRef() : m_Instance(nullptr), m_Type(nullptr) {}

        template <typename T, typename U = std::remove_cv_t<T>, typename = std::enable_if_t<!std::is_same_v<U, AnyRef>>>
        AnyRef(T& object) : m_Instance(&object), m_Type(Details::Resolve<U>()) {}

        template <std::size_t Size>
        AnyRef(BasicAny<Size>& any) : m_Instance(any.m_Instance), m_Type(any.m_Type) { }

    private:
        void* m_Instance; // Object we are pointing to. 
        const TypeDescriptor* m_Type; // Its Type Information.
    };

    template <std::size_t Size>
    void swap(BasicAny<Size>& any1, BasicAny<Size>& any2)
    {
        any1.swap(any2);
    }

    // Any acts as a container of an object of any kind. It either allocates the object dynamically on the heap or uses an SBO optimization for objects whose size is less than Size.
    template <std::size_t Size>
    class BasicAny
    {
        friend class AnyRef;

    public:
        BasicAny();

        /// Just use decay_t?
        template <typename T, typename U = typename std::remove_cv<std::remove_reference_t<std::decay_t<T>>>::type, typename = typename std::enable_if<!std::is_same_v<U, BasicAny>>::type>
        BasicAny(T&& object);

        BasicAny(const BasicAny& other);
        BasicAny(BasicAny&& other);

        BasicAny(AnyRef handle);

        ~BasicAny();

        template <typename T, typename U = typename std::remove_cv<std::remove_reference_t<std::decay_t<T>>>::type, typename = typename std::enable_if<!std::is_same_v<U, BasicAny>>::type>
        BasicAny& operator=(T&& object);

        BasicAny& operator=(const BasicAny& other);
        BasicAny& operator=(BasicAny&& other);

        void Swap(BasicAny& other);

        explicit operator bool() const { return Get() != nullptr; }

        const TypeDescriptor* GetType() const;

        const void* Get() const;

        void* Get();

        template <typename T>
        const T* TryCast() const;

        template <typename T>
        T* TryCast();

        template <typename T>
        BasicAny TryConvert() const;

        bool IsReference() const { return m_Copy == nullptr; } // Check if its a AnyRef.

    private:
        void* m_Instance;
        Details::AlignedStorageT<Size> m_Storage;

        const TypeDescriptor* m_Type;

        typedef void* (*CopyFunction)(void*, const void*);
        typedef void* (*MoveFunction)(void*, void*);
        typedef void (*DestroyFunction)(void*);

        CopyFunction m_Copy;
        MoveFunction m_Move;
        DestroyFunction m_Destroy;

        // Explicit Allocations (Fails SSO)
        template <typename T, typename = std::void_t<>>
        struct TypeTraits
        {
            template <typename ...Args>
            static void* New(void* storage, Args&&... args)
            {
                T* instance = new T(std::forward<Args>(args)...);
                new(storage) T* (instance);

                return instance;
            }

            static void* Copy(void* to, const void* from)
            {
                T* instance = new T(*static_cast<const T*>(from));
                new(to) T* (instance);

                return instance;
            }

            static void* Move(void* to, void* from)
            {
                T* instance = static_cast<T*>(from);
                new(to) T* (instance);

                return instance;
            }

            static void Destroy(void* instance)
            {
                delete static_cast<T*>(instance);
            }
        };

        // SSO
        template <typename T>
        struct TypeTraits<T, typename std::enable_if<sizeof(T) <= Size>::type>
        {
            template <typename ...Args>
            static void* New(void* storage, Args&&... args)
            {
                new(storage) T(std::forward<Args>(args)...);

                return storage;
            }

            static void* Copy(void* to, const void* from)
            {
                new(to) T(*static_cast<const T*>(from));

                return to;
            }

            static void* Move(void* to, void* from)
            {
                T& instance = *static_cast<T*>(from);
                new(to) T(std::move(instance));
                instance.~T(); // Delete old location.

                return to;
            }

            static void Destroy(void* instance)
            {
                static_cast<T*>(instance)->~T();
            }
        };
    };

    template <std::size_t Size>
    BasicAny<Size>::BasicAny() : m_Instance(nullptr), m_Copy(nullptr), m_Move(nullptr), m_Destroy(nullptr), m_Type(nullptr)
    {
        new(&m_Storage) std::nullptr_t (nullptr);
    }

    template <std::size_t Size>
    template <typename T, typename U, typename>
    BasicAny<Size>::BasicAny(T&& object) : m_Copy(TypeTraits<U>::Copy), m_Move(TypeTraits<U>::Move), m_Destroy(TypeTraits<U>::Destroy), m_Type(Details::Resolve<U>())
    {
        m_Instance = TypeTraits<U>::New(&m_Storage, std::forward<T>(object));
    }

    template <std::size_t Size>
    BasicAny<Size>::BasicAny(const BasicAny& other) : m_Copy(other.m_Copy), m_Move(other.m_Move), m_Destroy(other.m_Destroy), m_Type(other.m_Type)
    {
        m_Instance = other.m_Copy ? other.m_Copy(&m_Storage, other.m_Instance) : other.m_Instance;
    }

    template <std::size_t Size>
    BasicAny<Size>::BasicAny(BasicAny&& other) : m_Copy(other.m_Copy), m_Move(other.m_Move), m_Destroy(other.m_Destroy), m_Type(other.m_Type)
    {
        if (other.m_Move)
        {
            m_Instance = other.m_Move(&m_Storage, other.m_Instance);
            other.m_Destroy = nullptr;
        }
        else
        {
            m_Instance = other.m_Instance;
        }
    }

    template <std::size_t Size>
    BasicAny<Size>::BasicAny(AnyRef handle) : BasicAny()
    {
        m_Instance = handle.m_Instance;
        m_Type = handle.m_Type;
    }

    template <std::size_t Size>
    BasicAny<Size>::~BasicAny()
    {
        if (m_Destroy)
        {
            m_Destroy(m_Instance);
        }
    }

    template <std::size_t Size>
    template <typename T, typename U, typename>
    BasicAny<Size>& BasicAny<Size>::operator=(T&& object)
    {
        return *this = BasicAny(std::forward<T>(object));
    }

    template <std::size_t Size>
    BasicAny<Size>& BasicAny<Size>::operator=(const BasicAny& other)
    {
        return *this = BasicAny(other);
    }

    template <std::size_t Size>
    BasicAny<Size>& BasicAny<Size>::operator=(BasicAny&& other)
    {
        BasicAny temporary(std::move(other));
        Swap(temporary);

        return *this;
    }

    template <std::size_t Size>
    void BasicAny<Size>::Swap(BasicAny& other)
    {
        if (m_Move && other.m_Move)
        {
            Details::AlignedStorageT<Size> temporaryStorage;
            void* temporaryInstance = m_Move(&temporaryStorage, m_Instance);
            m_Instance = other.m_Move(m_Storage, other.m_Instance);
            other.m_Instance = m_Move(&other.m_Storage, temporaryInstance);
        }
        else if (m_Move)
        {
            other.m_Instance = m_Move(&other.m_Storage, m_Instance);
        }
        else if (other.m_Move)
        {
            m_Instance = other.m_Move(&m_Storage, other.m_Instance);
        }
        else
        {
            m_Instance = other.m_Instance;
        }

        std::swap(m_Type, other.m_Type);
        std::swap(m_Copy, other.m_Copy);
        std::swap(m_Move, other.m_Move);
        std::swap(m_Destroy, other.m_Destroy);
    }

    template <std::size_t Size>
    const TypeDescriptor* BasicAny<Size>::GetType() const
    {
        return m_Type;
    }

    template <std::size_t Size>
    const void* BasicAny<Size>::Get() const
    {
        return m_Instance;
    }

    template <std::size_t Size>
    void* BasicAny<Size>::Get()
    {
        return const_cast<void*>(static_cast<const BasicAny&>(*this).Get());
    }

    template <std::size_t Size>
    template <typename T>
    const T* BasicAny<Size>::TryCast() const
    {
        const TypeDescriptor* typeDescriptor = typeDescriptor = Details::Resolve<T>();
        void* casted = nullptr;

        if (!*this) // If this Any instance isn't constructed, just cast immediately.
        {
            return static_cast<const T*>(casted);
        }

        if (typeDescriptor == m_Type) // If the types are already the same, nothing needs to be done.
        {
            casted = m_Instance;
        }
        else // Else, try to find the type within the typeDescriptor's list of base classes and see if a cast is possible. 
        {
            for (auto* base : m_Type->GetBases())
            {
                if (base->GetType() == typeDescriptor)
                {
                    casted = base->Cast(m_Instance);
                }
            }
        }

        return static_cast<const T*>(casted); // Finally, cast.
    }

    template <std::size_t Size>
    template <typename T>
    T* BasicAny<Size>::TryCast()
    {
        return const_cast<T*>(static_cast<const BasicAny&>(*this).TryCast<T>());
    }

    template <std::size_t Size>
    template <typename T>
    BasicAny<Size> BasicAny<Size>::TryConvert() const
    {
        BasicAny converted;

        if (!*this)
        {
            return converted;
        }

        if (const TypeDescriptor* typeDescriptor = Details::Resolve<T>(); typeDescriptor == m_Type)
        {
            converted = *this;
        }
        else
        {
            for (auto* conversion : m_Type->GetConversions())
            {
                if (conversion->GetToType() == typeDescriptor)
                {
                    converted = conversion->Convert(m_Instance);
                }
            }
        }

        return converted;
    }

}

#endif 