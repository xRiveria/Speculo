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
    
        template <typename T, typename = void>
        struct Range_Max : std::integral_constant<int, ENUM_RANGE_MAX> {};

        template <typename T>
        struct Range_Max<T, std::void_t<decltype(Customize::Enum_Range<T>::m_Max)>> : std::integral_constant<decltype(Customize::Enum_Range<T>::m_Max), Customize::Enum_Range<T>::m_Max> {};

        template <std::size_t N>
        class Static_String
        {
        public:
            constexpr explicit Static_String(std::string_view stringView) noexcept : Static_String{ stringView, std::make_index_sequence<N>{} }
            {
                assert(stringView.size() == N);
            }

            constexpr const char* data() const noexcept { return m_Chars; }
            constexpr std::size_t size() const noexcept { return N; }
            constexpr operator std::string_view() const noexcept { return { data(), size() }; }


        private:
            template <std::size_t... I>
            constexpr Static_String(std::string_view stringView, std::index_sequence<I...>) noexcept : m_Chars{stringView[I]..., '\0'} {}

            char m_Chars[N + 1];
        };

        template <>
        class Static_String<0>
        {
        public:
            constexpr explicit Static_String(std::string_view) noexcept {}

            constexpr const char* data() const noexcept { return nullptr; }
            constexpr std::size_t size() const noexcept { return 0; }
            constexpr operator std::string_view() const noexcept { return {}; }
        };

        constexpr std::string_view Pretty_Name(std::string_view name) noexcept
        {
            for (std::size_t i = name.size(); i > 0; --i)
            {
                if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
                      (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
                      (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
#if defined(ENABLE_NO_ASCII)
                      (name[i - 1] & 0x80 ||
#endif
                      (name[i - 1] == '_')))
                {
                    name.remove_prefix(i);
                    break;
                }
            }

            if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                                    (name.front() >= 'A' && name.front() <= 'Z') ||
#if defined(ENABLE_NO_ASCII)
                                    (name.front() & 0x80) ||
#endif
                                    (name.front() == '_')))
            {
                return name;
            }

            return {};
        }

        template <typename CharType>
        constexpr auto To_Lower([[maybe_unused]] CharType ch) noexcept -> std::enable_if<std::is_same_v<std::decay_t<CharType>, char>, char>
        {
#if defined(ENUM_ENABLE_NO_ASCII)
            static_assert(!std::is_same_v<CharType, CharType>, "Detail::To_Lower is not supported as a Non-ASCII feature.");
            return {};
#else
            return 'A' <= ch && ch <= 'Z' ? ch - 'A' + 'a' : ch;
#endif
        }

        constexpr std::size_t Find(std::string_view stringView, char c) noexcept
        {
#if defined(__clang__) && __clang_major__ < 9 && defined(__GLIBCXX__) || defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
// https://stackoverflow.com/questions/56484834/constexpr-stdstring-viewfind-last-of-doesnt-work-on-clang-8-with-libstdc
// https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
            constexpr book workaround = true;
#else
            constexpr bool workaround = false;
#endif
            if constexpr (workaround)
            {
                for (std::size_t i = 0; i < stringView.size(); ++i)
                {
                    if (stringView[i] == c)
                    {
                        return i;
                    }
                }

                return std::string::npos;
            }
            else
            {
                return stringView.find_first_of(c);
            }
        }

        template <typename T, std::size_t N, std::size_t ...I>
        constexpr std::array<std::remove_cv_t<T>, N> To_Array(T(&a)[N], std::index_sequence<I...>)
        {
            return { {a[I]...} };
        }

        template <typename BinaryPredicate>
        constexpr bool Compare_Equal(std::string_view lhs, std::string_view rhs, [[maybe_unused]] BinaryPredicate&& p) noexcept(std::is_nothrow_invocable_r<bool, BinaryPredicate, char, char>)
        {
#if defined (_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
// https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
// https://developercommunity.visualstudio.com/content/problem/232218/c-constexpr-string-view.html
            constexpr bool workaround = true;
#else
            constexpr bool workaround = false;
#endif
            constexpr bool customPredicate =
                !std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<char>> &&
                !std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<>>;

            if constexpr (customPredicate || workaround)
            {
                if (lhs.size() != rhs.size())
                {
                    return false;
                }

                const auto size = lhs.size();
                for (std::size_t i = 0; i < size; ++i)
                {
                    if (!p(lhs[i], rhs[i]))
                    {
                        return false;
                    }
                }

                return true;
            }
            else
            {
                return lhs == rhs;
            }
        }
    }
}