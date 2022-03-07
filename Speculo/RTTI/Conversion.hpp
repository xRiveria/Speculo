#pragma once
#ifndef CONVERSION_H
#define CONVERSION_H

#include "TypeDescriptor.hpp"
#include "Any.hpp"

namespace Speculo
{
    class Conversion
    {
    public:
        const TypeDescriptor* GetFromType() const { return m_FromType; }
        const TypeDescriptor* GetToType() const { return m_ToType; }

        virtual Any Convert(const void* object) const = 0;

    protected:
        Conversion(const TypeDescriptor* from, const TypeDescriptor* to) : m_FromType(from), m_ToType(to) { }

    private:
        const TypeDescriptor* m_FromType; // Type to Convert From
        const TypeDescriptor* m_ToType;   // Type to Convert To
    };

    // Instantiates per conversion pair accordingly.
    template <typename From, typename To>
    class ConversionImplementation : public Conversion
    {
    public:
        ConversionImplementation() : Conversion(Details::Resolve<From>(), Details::Resolve<To>()) { }

        Any Convert(const void* object) const override
        {
            return static_cast<To>(*static_cast<const From*>(object));
        }
    };
}

#endif // CONVERSION_H