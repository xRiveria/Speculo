#ifndef CONNECTION_H
#define CONNECTION_H

namespace Speculo
{
    template <typename Signature>
    class Signal;

    template <typename Signature>
    class CallableWrapper;

    class Connection
    {
    public:
        Connection() : m_Signal(nullptr), m_CallableWrapper(nullptr), m_DisconnectFunction(nullptr) { }

        template <typename Return, typename ...Args>
        Connection(Signal<Return(Args...)>* signal, CallableWrapper<Return(Args...)>* callableWrapper) 
                  : m_Signal(signal), m_CallableWrapper(callableWrapper), m_DisconnectFunction(&DisconnectFunction<Return, Args...>) { }

        void Disconnect()
        {
            if (m_DisconnectFunction)
            {
                m_DisconnectFunction(m_Signal, m_CallableWrapper);
            }
        }

    private:
        template <typename Return, typename ...Args>
        static void DisconnectFunction(void* signal, void* callableWrapper)
        {
            static_cast<Signal<Return(Args...)>*>(signal)->Unbind(static_cast<CallableWrapper<Return(Args...)>*>(callableWrapper));
        }
        
    private:
        void (*m_DisconnectFunction)(void*, void*);
        void* m_Signal;
        void* m_CallableWrapper;
    };
}

#endif      // CONNECTION_H