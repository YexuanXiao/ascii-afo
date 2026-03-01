#pragma once

#include <cstddef>

#include "./ascii.hpp"

inline constexpr bool is_ascii_any_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_any) != 0;
}

inline constexpr bool is_ascii_digit_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_digit) != 0;
}

inline constexpr bool is_ascii_bit_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_bit) != 0;
}

inline constexpr bool is_ascii_octal_digit_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_octal_digit) != 0;
}

inline constexpr bool is_ascii_hex_digit_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_hex_digit) != 0;
}

inline constexpr bool is_ascii_lower_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_lower) != 0;
}

inline constexpr bool is_ascii_upper_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_upper) != 0;
}

inline constexpr bool is_ascii_alphabetic_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_alphabetic) != 0;
}

inline constexpr bool is_ascii_alphanumeric_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_alphanumeric) != 0;
}

inline constexpr bool is_ascii_punctuation_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_punctuation) != 0;
}

inline constexpr bool is_ascii_graphic_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_graphic) != 0;
}

inline constexpr bool is_ascii_printing_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_printing) != 0;
}

inline constexpr bool is_ascii_horizontal_whitespace_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_horizontal_whitespace) != 0;
}

inline constexpr bool is_ascii_whitespace_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_whitespace) != 0;
}

inline constexpr bool is_ascii_control_u8(char8_t c) noexcept
{
    return (bizwen::detail::class_table[static_cast<std::size_t>(c)] & bizwen::detail::ascii_class_control) != 0;
}
