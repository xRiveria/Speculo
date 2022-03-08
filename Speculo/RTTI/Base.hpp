#ifndef BASE_H
#define BASE_H

#include "TypeDescriptor.hpp"

namespace Speculo
{
    class Base
    {
    public:
        const TypeDescriptor* GetType() const { return m_Type; }
        virtual void* Cast(void* object) = 0;

    protected:
        Base(const TypeDescriptor* type, const TypeDescriptor* parent) : m_Type(type), m_Parent(parent) {}

    private:
        const TypeDescriptor* m_Type;   // Base 
        const TypeDescriptor* m_Parent; // Derived
    };

    template <typename Base, typename Derived>
    class BaseImplementation : public Base
    {
    public:
        BaseImplementation() : Base(Details::Resolve<Base>(), Details::Resolve<Derived>()) {}

        void* Cast(void* object) override
        {
            return static_cast<Base*>(object);
        }
    };
}

#endif // BASE_H