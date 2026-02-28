#pragma once

#pragma push_macro("BIZWEN_EXPORT")
#undef BIZWEN_EXPORT

#if !defined(BIZWEN_MODULE)
#define BIZWEN_EXPORT

#include <array>
#include <climits>
#include <compare>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <memory> // std::to_address
#include <ranges>
#include <type_traits> // std::remove_reference

#else

#define BIZWEN_EXPORT export

#endif

static_assert(CHAR_BIT == 8);

namespace bizwen
{

namespace detail
{
using cls_base_t = unsigned int;

}

BIZWEN_EXPORT enum class ascii_classification : detail::cls_base_t
{
    any = 1u << 0,
    digit = 1u << 1,
    bit = 1u << 2,
    octal_digit = 1u << 3,
    hex_digit = 1u << 4,
    lower = 1u << 5,
    upper = 1u << 6,
    alphabetic = 1u << 7,
    alphanumeric = 1u << 8,
    punctuation = 1u << 9,
    graphic = 1u << 10,
    printing = 1u << 11,
    horizontal_whitespace = 1u << 12,
    whitespace = 1u << 13,
    control = 1u << 14,
};

namespace detail
{
template <typename T>
inline constexpr auto to_address_const(T t)
{
    auto ptr = std::to_address(t);
    using const_pointer = std::add_const_t<std::remove_reference_t<decltype(*t)>> *;
    return const_pointer(ptr);
}

template <typename T>
inline constexpr bool is_supported_ascii_char_v =
    std::is_same_v<std::remove_cv_t<T>, char> || std::is_same_v<std::remove_cv_t<T>, wchar_t> ||
    std::is_same_v<std::remove_cv_t<T>, char8_t> || std::is_same_v<std::remove_cv_t<T>, char16_t> ||
    std::is_same_v<std::remove_cv_t<T>, char32_t>;

// ASCII classification and case-mapping are implemented via 256-entry lookup tables.
//
// For each possible byte value c in [0, 255]:
// lower_table[c] is the ASCII-lowercased byte (only 'A'..'Z' changes).
// upper_table[c] is the ASCII-uppercased byte (only 'a'..'z' changes).
// class_table[c] is a bitmask of precomputed "is X" properties.
//
// The primary ascii_find_* algorithms scan the input range and return an iterator to the first/last byte that is
// not in the requested classification:
// (class_table[byte] & required_mask) == 0
//
// ascii_is_* functions are thin wrappers equivalent to:
// ascii_find_first_not_of(mask, begin, end) == end

// Bit flags stored in class_table[c] (fits in 16 bits).
inline constexpr detail::cls_base_t ascii_class_any = 1u << 0;
inline constexpr detail::cls_base_t ascii_class_digit = 1u << 1;
inline constexpr detail::cls_base_t ascii_class_bit = 1u << 2;
inline constexpr detail::cls_base_t ascii_class_octal_digit = 1u << 3;
inline constexpr detail::cls_base_t ascii_class_hex_digit = 1u << 4;
inline constexpr detail::cls_base_t ascii_class_lower = 1u << 5;
inline constexpr detail::cls_base_t ascii_class_upper = 1u << 6;
inline constexpr detail::cls_base_t ascii_class_alphabetic = 1u << 7;
inline constexpr detail::cls_base_t ascii_class_alphanumeric = 1u << 8;
inline constexpr detail::cls_base_t ascii_class_punctuation = 1u << 9;
inline constexpr detail::cls_base_t ascii_class_graphic = 1u << 10;
inline constexpr detail::cls_base_t ascii_class_printing = 1u << 11;
inline constexpr detail::cls_base_t ascii_class_horizontal_whitespace = 1u << 12;
inline constexpr detail::cls_base_t ascii_class_whitespace = 1u << 13;
inline constexpr detail::cls_base_t ascii_class_control = 1u << 14;

#if defined(BIZWEN_PREDEFINED_LOOKUP_TABLE)
inline constexpr std::array<char8_t, 256> lower_table = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};

inline constexpr std::array<char8_t, 256> upper_table = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};

inline constexpr std::array<detail::cls_base_t, 256> class_table = {
    0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x7001, 0x6001, 0x6001, 0x6001, 0x6001, 0x4001, 0x4001,
    0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001, 0x4001,
    0x3801, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01,
    0x0D1F, 0x0D1F, 0x0D1B, 0x0D1B, 0x0D1B, 0x0D1B, 0x0D1B, 0x0D1B, 0x0D13, 0x0D13, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01,
    0x0E01, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1,
    0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0DC1, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x0E01,
    0x0E01, 0x0DB1, 0x0DB1, 0x0DB1, 0x0DB1, 0x0DB1, 0x0DB1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1,
    0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0DA1, 0x0E01, 0x0E01, 0x0E01, 0x0E01, 0x4001,
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,   
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0000};

#else
consteval std::array<char8_t, 256> make_lower_table()
{
    std::array<char8_t, 256> table{};
    for (std::size_t i{}; i != table.size(); ++i)
    {
        auto c = static_cast<char8_t>(i);
        if (c >= u8'A' && c <= u8'Z')
            table[i] = c + static_cast<char8_t>(32);
        else
            table[i] = c;
    }
    return table;
}

consteval std::array<char8_t, 256> make_upper_table()
{
    std::array<char8_t, 256> table{};
    for (std::size_t i{}; i != table.size(); ++i)
    {
        auto c = static_cast<char8_t>(i);
        if (c >= u8'a' && c <= u8'z')
            table[i] = c - static_cast<char8_t>(32);
        else
            table[i] = c;
    }
    return table;
}

consteval std::array<detail::cls_base_t, 256> make_class_table()
{
    std::array<detail::cls_base_t, 256> table{};
    for (std::size_t i{}; i != table.size(); ++i)
    {
        auto c = static_cast<char8_t>(i);
        detail::cls_base_t f{};

        if (c <= 0x7F)
            f |= ascii_class_any;
        if (c >= u8'0' && c <= u8'9')
            f |= ascii_class_digit;
        if (c == u8'0' || c == u8'1')
            f |= ascii_class_bit;
        if (c >= u8'0' && c <= u8'7')
            f |= ascii_class_octal_digit;
        if ((c >= u8'0' && c <= u8'9') || (c >= u8'a' && c <= u8'f') || (c >= u8'A' && c <= u8'F'))
            f |= ascii_class_hex_digit;
        if (c >= u8'a' && c <= u8'z')
            f |= ascii_class_lower;
        if (c >= u8'A' && c <= u8'Z')
            f |= ascii_class_upper;
        if ((f & (ascii_class_lower | ascii_class_upper)) != 0)
            f |= ascii_class_alphabetic;
        if ((f & (ascii_class_alphabetic | ascii_class_digit)) != 0)
            f |= ascii_class_alphanumeric;
        if ((c >= 0x21 && c <= 0x2F) || (c >= 0x3A && c <= 0x40) || (c >= 0x5B && c <= 0x60) ||
            (c >= 0x7B && c <= 0x7E))
            f |= ascii_class_punctuation;
        if (c >= 0x21 && c <= 0x7E)
            f |= ascii_class_graphic;
        if (c >= 0x20 && c <= 0x7E)
            f |= ascii_class_printing;
        if (c == 0x20 || c == 0x09)
            f |= ascii_class_horizontal_whitespace;
        if (c == 0x20 || c == 0x0C || c == 0x0A || c == 0x0D || c == 0x09 || c == 0x0B)
            f |= ascii_class_whitespace;
        if (c <= 0x1F || c == 0x7F)
            f |= ascii_class_control;

        table[i] = f;
    }
    return table;
}

inline constexpr auto lower_table = make_lower_table();
inline constexpr auto upper_table = make_upper_table();
inline constexpr auto class_table = make_class_table();

#endif

inline constexpr char8_t ascii_lower_u8(char8_t c) noexcept
{
    return lower_table[static_cast<std::size_t>(c)];
}

inline constexpr char8_t ascii_upper_u8(char8_t c) noexcept
{
    return upper_table[static_cast<std::size_t>(c)];
}

template <typename Char>
inline constexpr Char ascii_lower_char(Char c) noexcept
{
    auto v = static_cast<unsigned long>(c);
    if (v <= 0xFF)
        return static_cast<Char>(ascii_lower_u8(static_cast<char8_t>(v)));
    return c;
}

template <typename Char>
inline constexpr Char ascii_upper_char(Char c) noexcept
{
    auto v = static_cast<unsigned long>(c);
    if (v <= 0xFF)
        return static_cast<Char>(ascii_upper_u8(static_cast<char8_t>(v)));
    return c;
}

template <typename Char>
inline constexpr unsigned long ascii_lower_u32(Char c) noexcept
{
    auto v = static_cast<unsigned long>(c);
    if (v <= 0xFF)
        return static_cast<unsigned long>(ascii_lower_u8(static_cast<char8_t>(v)));
    return v;
}

// Convenience wrappers for classification (bit tests on `class_table`).
inline constexpr bool is_ascii_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_any) != 0;
}

inline constexpr bool is_digit_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_digit) != 0;
}

inline constexpr bool is_bit_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_bit) != 0;
}

inline constexpr bool is_octal_digit_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_octal_digit) != 0;
}

inline constexpr bool is_hex_digit_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_hex_digit) != 0;
}

inline constexpr bool is_lower_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_lower) != 0;
}

inline constexpr bool is_upper_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_upper) != 0;
}

inline constexpr bool is_alphabetic_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_alphabetic) != 0;
}

inline constexpr bool is_alphanumeric_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_alphanumeric) != 0;
}

inline constexpr bool is_punctuation_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_punctuation) != 0;
}

inline constexpr bool is_graphic_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_graphic) != 0;
}

inline constexpr bool is_printing_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_printing) != 0;
}

inline constexpr bool is_horizontal_whitespace_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_horizontal_whitespace) != 0;
}

inline constexpr bool is_whitespace_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_whitespace) != 0;
}

inline constexpr bool is_control_uc(char8_t c) noexcept
{
    return (class_table[static_cast<std::size_t>(c)] & ascii_class_control) != 0;
}

template <typename InChar>
inline constexpr InChar const *find_first_not_of_mask_ptr(InChar const *begin, InChar const *end, detail::cls_base_t required) noexcept
{
    using in_char = std::remove_cv_t<InChar>;

    if constexpr (sizeof(in_char) == 1)
    {
        for (auto p = begin; p != end; ++p)
        {
            auto idx = static_cast<std::size_t>(static_cast<char8_t>(*p));
            if ((class_table[idx] & required) == 0)
                return p;
        }
    }
    else
    {
        for (auto p = begin; p != end; ++p)
        {
            auto v = static_cast<unsigned long>(*p);
            if (v > 0xFF)
                return p;
            auto idx = static_cast<std::size_t>(v);
            if ((class_table[idx] & required) == 0)
                return p;
        }
    }

    return end;
}

template <typename InChar>
inline constexpr InChar const *find_last_not_of_mask_ptr(InChar const *begin, InChar const *end, detail::cls_base_t required) noexcept
{
    using in_char = std::remove_cv_t<InChar>;

    if constexpr (sizeof(in_char) == 1)
    {
        for (auto p = end; p != begin;)
        {
            --p;
            auto idx = static_cast<std::size_t>(static_cast<char8_t>(*p));
            if ((class_table[idx] & required) == 0)
                return p;
        }
    }
    else
    {
        for (auto p = end; p != begin;)
        {
            --p;
            auto v = static_cast<unsigned long>(*p);
            if (v > 0xFF)
                return p;
            auto idx = static_cast<std::size_t>(v);
            if ((class_table[idx] & required) == 0)
                return p;
        }
    }

    return end;
}

template <typename In>
inline constexpr In find_first_not_of_mask(In begin, In end, detail::cls_base_t required)
{
    using in_char = std::remove_cv_t<std::iter_value_t<In>>;

    static_assert(std::contiguous_iterator<In>);
    static_assert(is_supported_ascii_char_v<in_char>);

    auto b = detail::to_address_const(begin);
    auto e = detail::to_address_const(end);
    auto result_ptr = find_first_not_of_mask_ptr(b, e, required);
    return begin + (result_ptr - b);
}

template <typename In>
inline constexpr In find_last_not_of_mask(In begin, In end, detail::cls_base_t required)
{
    using in_char = std::remove_cv_t<std::iter_value_t<In>>;

    static_assert(std::contiguous_iterator<In>);
    static_assert(is_supported_ascii_char_v<in_char>);

    auto b = detail::to_address_const(begin);
    auto e = detail::to_address_const(end);
    auto result_ptr = find_last_not_of_mask_ptr(b, e, required);
    return begin + (result_ptr - b);
}

template <typename In>
inline constexpr bool all_of_flag(In begin, In end, detail::cls_base_t required)
{
    return find_first_not_of_mask(begin, end, required) == end;
}

template <typename InOut>
inline constexpr void static_assert_mutable_contiguous()
{
    using in_char = std::remove_cv_t<std::iter_value_t<InOut>>;
    using ref = std::iter_reference_t<InOut>;

    static_assert(std::contiguous_iterator<InOut>);
    static_assert(is_supported_ascii_char_v<in_char>);
    static_assert(!std::is_const_v<std::remove_reference_t<ref>>);
}

template <typename InChar, typename Out>
inline constexpr Out to_lower_ptr(InChar const *begin, InChar const *end, Out out)
{
    for (auto p = begin; p != end; ++p)
    {
        auto mapped = ascii_lower_char(*p);
        *out = mapped;
        ++out;
    }
    return out;
}

template <typename InChar>
inline constexpr void to_lower_inplace_ptr(InChar *begin, InChar *end)
{
    for (auto p = begin; p != end; ++p)
    {
        *p = ascii_lower_char(*p);
    }
}

template <typename InChar, typename Out>
inline constexpr Out to_upper_ptr(InChar const *begin, InChar const *end, Out out)
{
    for (auto p = begin; p != end; ++p)
    {
        auto mapped = ascii_upper_char(*p);
        *out = mapped;
        ++out;
    }
    return out;
}

template <typename InChar>
inline constexpr void to_upper_inplace_ptr(InChar *begin, InChar *end)
{
    for (auto p = begin; p != end; ++p)
    {
        *p = ascii_upper_char(*p);
    }
}

template <typename C1, typename C2>
inline constexpr std::strong_ordering case_insensitive_compare_ptr(C1 const *p1, C1 const *end1, C2 const *p2,
                                                                   C2 const *end2)
{
    for (; p1 != end1 && p2 != end2; ++p1, ++p2)
    {
        auto x = ascii_lower_u32(*p1);
        auto y = ascii_lower_u32(*p2);

        if (x < y)
            return std::strong_ordering::less;
        if (x > y)
            return std::strong_ordering::greater;
    }

    if (p1 == end1 && p2 == end2)
        return std::strong_ordering::equal;
    if (p1 == end1)
        return std::strong_ordering::less;
    return std::strong_ordering::greater;
}

} // namespace detail

namespace ascii_impl
{
struct ascii_find_first_not_of_fn
{
    template <typename In>
    static inline constexpr In operator()(In begin, In end, ascii_classification cls)
    {
        return detail::find_first_not_of_mask(begin, end, static_cast<detail::cls_base_t>(cls));
    }

    template <typename R>
        requires std::ranges::contiguous_range<R> && std::ranges::borrowed_range<R>
    static inline constexpr std::ranges::iterator_t<R> operator()(R &&r, ascii_classification cls)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), cls);
    }
};

struct ascii_find_last_not_of_fn
{
    template <typename In>
    static inline constexpr In operator()(In begin, In end, ascii_classification cls)
    {
        return detail::find_last_not_of_mask(begin, end, static_cast<detail::cls_base_t>(cls));
    }

    template <typename R>
        requires std::ranges::contiguous_range<R> && std::ranges::borrowed_range<R>
    static inline constexpr std::ranges::iterator_t<R> operator()(R &&r, ascii_classification cls)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), cls);
    }
};

struct ascii_is_any_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::any) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_digit_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::digit) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_bit_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::bit) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_octal_digit_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::octal_digit) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_hex_digit_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::hex_digit) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_lower_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::lower) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_upper_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::upper) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_alphabetic_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::alphabetic) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_alphanumeric_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::alphanumeric) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_punctuation_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::punctuation) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_graphic_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::graphic) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_printing_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::printing) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_horizontal_whitespace_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::horizontal_whitespace) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_whitespace_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::whitespace) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_is_control_fn
{
    template <typename In>
    static inline constexpr bool operator()(In begin, In end)
    {
        return ascii_find_first_not_of_fn::operator()(begin, end, ascii_classification::control) == end;
    }

    template <typename R>
    static inline constexpr bool operator()(R &&r)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_to_lower_copy_fn
{
    template <typename In, typename Out>
    static inline constexpr Out operator()(In begin, In end, Out out)
    {
        using in_char = std::remove_cv_t<std::iter_value_t<In>>;

        static_assert(std::contiguous_iterator<In>);
        static_assert(detail::is_supported_ascii_char_v<in_char>);

        auto p = detail::to_address_const(begin);
        auto e = detail::to_address_const(end);
        return detail::to_lower_ptr(p, e, out);
    }

    template <typename R, typename Out>
    static inline constexpr Out operator()(R &&r, Out out)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), out);
    }
};

struct ascii_to_lower_fn
{

    template <typename InOut>
    static inline constexpr void operator()(InOut begin, InOut end)
    {
        detail::static_assert_mutable_contiguous<InOut>();

        using in_char = std::remove_cv_t<std::iter_value_t<InOut>>;

        auto p = std::to_address(begin);
        auto e = std::to_address(end);
        detail::to_lower_inplace_ptr(p, e);
    }

    template <typename R>
    static inline constexpr void operator()(R &&r)
    {
        operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_to_upper_copy_fn
{
    template <typename In, typename Out>
    static inline constexpr Out operator()(In begin, In end, Out out)
    {
        using in_char = std::remove_cv_t<std::iter_value_t<In>>;

        static_assert(std::contiguous_iterator<In>);
        static_assert(detail::is_supported_ascii_char_v<in_char>);

        auto p = detail::to_address_const(begin);
        auto e = detail::to_address_const(end);
        return detail::to_upper_ptr(p, e, out);
    }

    template <typename R, typename Out>
    static inline constexpr Out operator()(R &&r, Out out)
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), out);
    }
};

struct ascii_to_upper_fn
{

    template <typename InOut>
    static inline constexpr void operator()(InOut begin, InOut end)
    {
        detail::static_assert_mutable_contiguous<InOut>();

        using in_char = std::remove_cv_t<std::iter_value_t<InOut>>;

        auto p = std::to_address(begin);
        auto e = std::to_address(end);
        detail::to_upper_inplace_ptr(p, e);
    }

    template <typename R>
    static inline constexpr void operator()(R &&r)
    {
        operator()(std::ranges::begin(r), std::ranges::end(r));
    }
};

struct ascii_case_insensitive_compare_fn
{
    template <typename In1, typename In2>
    static inline constexpr std::strong_ordering operator()(In1 b1, In1 e1, In2 b2, In2 e2)
    {
        using c1 = std::remove_cv_t<std::iter_value_t<In1>>;
        using c2 = std::remove_cv_t<std::iter_value_t<In2>>;

        static_assert(std::contiguous_iterator<In1>);
        static_assert(std::contiguous_iterator<In2>);
        static_assert(detail::is_supported_ascii_char_v<c1>);
        static_assert(detail::is_supported_ascii_char_v<c2>);

        auto p1 = detail::to_address_const(b1);
        auto end1 = detail::to_address_const(e1);
        auto p2 = detail::to_address_const(b2);
        auto end2 = detail::to_address_const(e2);
        return detail::case_insensitive_compare_ptr(p1, end1, p2, end2);
    }

    template <typename R1, typename R2>
    static inline constexpr std::strong_ordering operator()(R1 &&r1, R2 &&r2)
    {
        return operator()(std::ranges::begin(r1), std::ranges::end(r1), std::ranges::begin(r2), std::ranges::end(r2));
    }
};

struct ascii_case_insensitive_equals_fn
{
    template <typename In1, typename In2>
    static inline constexpr bool operator()(In1 b1, In1 e1, In2 b2, In2 e2)
    {
        static_assert(std::contiguous_iterator<In1>);
        static_assert(std::contiguous_iterator<In2>);

        if (e1 - b1 != e2 - b2)
            return false;

        return ascii_case_insensitive_compare_fn::operator()(b1, e1, b2, e2) == std::strong_ordering::equal;
    }

    template <typename R1, typename R2>
    static inline constexpr bool operator()(R1 &&r1, R2 &&r2)
    {
        return operator()(std::ranges::begin(r1), std::ranges::end(r1), std::ranges::begin(r2), std::ranges::end(r2));
    }
};

} // namespace ascii_impl

BIZWEN_EXPORT inline constexpr ascii_impl::ascii_find_first_not_of_fn ascii_find_first_not_of{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_find_last_not_of_fn ascii_find_last_not_of{};

BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_any_fn ascii_is_any{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_digit_fn ascii_is_digit{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_bit_fn ascii_is_bit{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_octal_digit_fn ascii_is_octal_digit{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_hex_digit_fn ascii_is_hex_digit{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_lower_fn ascii_is_lower{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_upper_fn ascii_is_upper{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_alphabetic_fn ascii_is_alphabetic{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_alphanumeric_fn ascii_is_alphanumeric{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_punctuation_fn ascii_is_punctuation{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_graphic_fn ascii_is_graphic{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_printing_fn ascii_is_printing{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_horizontal_whitespace_fn ascii_is_horizontal_whitespace{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_whitespace_fn ascii_is_whitespace{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_is_control_fn ascii_is_control{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_to_lower_copy_fn ascii_to_lower_copy{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_to_upper_copy_fn ascii_to_upper_copy{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_to_lower_fn ascii_to_lower{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_to_upper_fn ascii_to_upper{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_case_insensitive_compare_fn ascii_case_insensitive_compare{};
BIZWEN_EXPORT inline constexpr ascii_impl::ascii_case_insensitive_equals_fn ascii_case_insensitive_equals{};

} // namespace bizwen

#pragma pop_macro("BIZWEN_EXPORT")
