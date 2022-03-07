#pragma once
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
        const TypeDescriptor* m_Parent; // Derived
        const TypeDescriptor* m_Type;   // Base 
    };

    template <typename B, typename D>
    class BaseImplementation : public Base
    {
    public:
        BaseImplementation() : Base(Details::Resolve<B>(), Details::Resolve<D>()) {}

        void* Cast(void* object) override
        {
            return static_cast<B*>(object);
        }
    };
}

#endif // BASE_H