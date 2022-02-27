#pragma once
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <type_traits>
#include <utility>

#include <string>
#include <string_view>

// Macros can be redefined to override default ones to change system integral values.

namespace Reflect_Enum
{

// The underlying type of enums are implementation defined unless explicitly stated (strongly typed enumerations). 
// Otherwise, the compiler gets to choose. In most implementations, it is signed unless there is a presence of a negative number.
// We will hereby set our range to be between -128 and 128 inclusive.

#if !defined(ENUM_RANGE_MIN)
#define ENUM_RANGE_MIN -128
#endif

#if !defined(ENUM_RANGE_MAX)
#define ENUM_RANGE_MAX 128
#endif

    namespace Customize
    {
        // Our enum range must be between the ranges ENUM_RANGE_MIN and ENUM_RANGE_MAX inclusive, which defaults to -128 to 128. Specializations can be added for specific types.
        template <typename Enum>
        struct Enum_Range
        {
            static_assert(std::is_enum_v<Enum>, "Reflect_Enum::Customize::Enum_Range requires an enum type to be specified.");
            static constexpr int m_Min = ENUM_RANGE_MIN;
            static constexpr int m_Max = ENUM_RANGE_MAX;
            static_assert(m_Max > m_Min, "Reflect_Enum::Customize::Enum_Range requires condition: Max > Min to be met.");
        };

        static_assert(ENUM_RANGE_MAX > ENUM_RANGE_MIN, "ENUM_RANGE_MAX must be greater than ENUM_RANGE_MIN");
        static_assert((ENUM_RANGE_MAX - ENUM_RANGE_MIN) < std::numeric_limits<std::uint16_t>::max(), "ENUM_RANGE must be lesser than uint16_t::max()."); 

        // If we need custom names for specific enum types, add specialization Enum_Name as required.
        template <typename Enum>
        constexpr std::string_view Enum_Name(Enum) noexcept
        {
            static_assert(std::is_enum_v<Enum>, "Reflect::Enum::Customize::Enum_Name requires an enum type to be specified.");
            /// Specialization as needed.
            return {};
        }
    } // Reflect_Enum::Customize

    namespace Detail
    {
        template <typename T, typename = void>
        struct Has_Is_Flags : std::false_type {};

        template <typename T>
        struct Has_Is_Flags<T, std::void_t<decltype(Customize::Enum_Range<T>::is_flags)>> : std::bool_constant<std::is_same_v<bool, std::decay<decltype(Customize::Enum_Range<T>::is_flags)>>> {};

        template <typename T, typename = void>
        struct Range_Min : std::integral_constant<int, ENUM_RANGE_MIN> {};

        template <typename T>
        struct Range_Min<T, std::void_t<decltype(Customize::Enum_Range<T>::m_Min)>> : std::integral_constant<decltype(Customize::Enum_Range<T>::m_Min), Customize::Enum_Range<T>::m_Min> {};
    }
}