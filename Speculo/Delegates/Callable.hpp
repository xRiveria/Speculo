#ifndef CALLABLE_WRAPPER_H
#define CALLABLE_WRAPPER_H

#include <type_traits>

namespace Speculo
{
    // Base Callable Wrapper Class
    template <typename Signature>
    class CallableWrapper;

    template <typename Return, typename ...Args>
    class CallableWrapper<Return(Args...)>
    {
    public:
        virtual ~CallableWrapper() = default;
        virtual Return Invoke(Args... args) = 0;

    protected:
        CallableWrapper() = default;
    };

    // Wrapper around a (possibly const) member function (this implementation provides automatic arguments and return type conversions).
    template <typename Signature, typename T, typename PtrToMemberFunction>
    class MemberFunctionCallableWrapper;

    template <typename Return, typename ...Args, typename T, typename PtrToMemberFunction>
    class MemberFunctionCallableWrapper<Return(Args...), T, PtrToMemberFunction> : public CallableWrapper<Return(Args...)>
    {
    public:
        MemberFunctionCallableWrapper(T& instance, PtrToMemberFunction ptrToMemberFunction) : m_Instance(instance), m_PtrToMemberFunction(ptrToMemberFunction) { }
        Return Invoke(Args... args) override
        {
            return (m_Instance.*m_PtrToMemberFunction)(std::forward<Args>(args)...);
        }

    private:
        T& m_Instance;
        PtrToMemberFunction m_PtrToMemberFunction;
    };

    // Wrapper around a function object/lambda.
    template <typename Signature, typename T>
    class FunctionObjectCallableWrapper;

    template <typename Return, typename ...Args, typename T>
    class FunctionObjectCallableWrapper<Return(Args...), T> : public CallableWrapper<Return(Args...)>
    {
    public:
        FunctionObjectCallableWrapper(T& functionObject)  : m_FunctionObject(&functionObject), m_Allocated(false) { }
        FunctionObjectCallableWrapper(T&& functionObject) : m_FunctionObject(new T(std::move(functionObject))), m_Allocated(true) { }

        ~FunctionObjectCallableWrapper() { Destroy(); }

        Return Invoke(Args... args) override
        {
            return (*m_FunctionObject)(std::forward<Args>(args)...);
        }

    private:
        template <typename U = T, typename = std::enable_if_t<std::is_function<U>::value>> // Dummy type parameter defaulted to T.
        void Destroy() {}

        template <typename U = T, typename = std::enable_if_t<!std::is_function<U>::value>, typename = T> // Dummy type parameter defaulted to T + Extra type parameter for overloading.
        void Destroy()
        {
            if (m_Allocated)
            {
                delete m_FunctionObject;
            }
        }

    private:
        T* m_FunctionObject;
        bool m_Allocated;
    };
}

#endif          // CALLABLE_WRAPPER_H