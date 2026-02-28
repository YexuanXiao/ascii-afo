#include "ascii.hpp"

#include <cassert>
#include <compare>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

int main()
{
    // Find algorithms (return iterator to the first/last invalid element; end => all valid).
    constexpr auto empty = ""sv;
    static_assert(bizwen::ascii_find_first_not_of(empty, bizwen::ascii_classification::any) == empty.end());
    static_assert(bizwen::ascii_find_last_not_of(empty, bizwen::ascii_classification::any) == empty.end());

    constexpr auto any_ok = "Hello"sv;
    static_assert(bizwen::ascii_find_first_not_of(any_ok, bizwen::ascii_classification::any) == any_ok.end());
    constexpr auto any_bad = "\x80"sv;
    static_assert(bizwen::ascii_find_first_not_of(any_bad, bizwen::ascii_classification::any) == any_bad.begin());
    static_assert(bizwen::ascii_find_last_not_of(any_bad, bizwen::ascii_classification::any) == any_bad.begin());

    constexpr auto digit_ok = "0123456789"sv;
    static_assert(bizwen::ascii_find_first_not_of(digit_ok, bizwen::ascii_classification::digit) == digit_ok.end());
    constexpr auto digit_bad = "123a"sv;
    static_assert(bizwen::ascii_find_first_not_of(digit_bad, bizwen::ascii_classification::digit) == digit_bad.begin() + 3);
    static_assert(bizwen::ascii_find_last_not_of(digit_bad, bizwen::ascii_classification::digit) == digit_bad.begin() + 3);

    constexpr auto bit_ok = "01"sv;
    static_assert(bizwen::ascii_find_first_not_of(bit_ok, bizwen::ascii_classification::bit) == bit_ok.end());
    constexpr auto bit_bad = "012"sv;
    static_assert(bizwen::ascii_find_first_not_of(bit_bad, bizwen::ascii_classification::bit) == bit_bad.begin() + 2);

    constexpr auto oct_ok = "01234567"sv;
    static_assert(bizwen::ascii_find_first_not_of(oct_ok, bizwen::ascii_classification::octal_digit) == oct_ok.end());
    constexpr auto oct_bad = "8"sv;
    static_assert(bizwen::ascii_find_first_not_of(oct_bad, bizwen::ascii_classification::octal_digit) == oct_bad.begin());

    constexpr auto hex_ok = "deadBEEF"sv;
    static_assert(bizwen::ascii_find_first_not_of(hex_ok, bizwen::ascii_classification::hex_digit) == hex_ok.end());
    constexpr auto hex_bad = "deadBEEG"sv;
    static_assert(bizwen::ascii_find_first_not_of(hex_bad, bizwen::ascii_classification::hex_digit) == hex_bad.begin() + 7);
    static_assert(bizwen::ascii_find_last_not_of(hex_bad, bizwen::ascii_classification::hex_digit) == hex_bad.begin() + 7);

    constexpr auto lower_ok = "abcxyz"sv;
    static_assert(bizwen::ascii_find_first_not_of(lower_ok, bizwen::ascii_classification::lower) == lower_ok.end());
    constexpr auto lower_bad = "abC"sv;
    static_assert(bizwen::ascii_find_first_not_of(lower_bad, bizwen::ascii_classification::lower) == lower_bad.begin() + 2);

    constexpr auto upper_ok = "ABCXYZ"sv;
    static_assert(bizwen::ascii_find_first_not_of(upper_ok, bizwen::ascii_classification::upper) == upper_ok.end());
    constexpr auto upper_bad = "ABc"sv;
    static_assert(bizwen::ascii_find_first_not_of(upper_bad, bizwen::ascii_classification::upper) == upper_bad.begin() + 2);

    constexpr auto alpha_ok = "AbCd"sv;
    static_assert(bizwen::ascii_find_first_not_of(alpha_ok, bizwen::ascii_classification::alphabetic) == alpha_ok.end());
    constexpr auto alpha_bad = "Ab1"sv;
    static_assert(bizwen::ascii_find_first_not_of(alpha_bad, bizwen::ascii_classification::alphabetic) == alpha_bad.begin() + 2);

    constexpr auto alnum_ok = "Ab1"sv;
    static_assert(bizwen::ascii_find_first_not_of(alnum_ok, bizwen::ascii_classification::alphanumeric) == alnum_ok.end());
    constexpr auto alnum_bad = "Ab-"sv;
    static_assert(bizwen::ascii_find_first_not_of(alnum_bad, bizwen::ascii_classification::alphanumeric) == alnum_bad.begin() + 2);

    constexpr auto punct_ok = "[]{}()"sv;
    static_assert(bizwen::ascii_find_first_not_of(punct_ok, bizwen::ascii_classification::punctuation) == punct_ok.end());
    constexpr auto punct_bad = "A"sv;
    static_assert(bizwen::ascii_find_first_not_of(punct_bad, bizwen::ascii_classification::punctuation) == punct_bad.begin());

    constexpr auto graphic_ok = "AZaz09!~"sv;
    static_assert(bizwen::ascii_find_first_not_of(graphic_ok, bizwen::ascii_classification::graphic) == graphic_ok.end());
    constexpr auto graphic_bad = " "sv;
    static_assert(bizwen::ascii_find_first_not_of(graphic_bad, bizwen::ascii_classification::graphic) == graphic_bad.begin());

    constexpr auto print_ok = " "sv;
    static_assert(bizwen::ascii_find_first_not_of(print_ok, bizwen::ascii_classification::printing) == print_ok.end());
    constexpr auto print_bad = "\n"sv;
    static_assert(bizwen::ascii_find_first_not_of(print_bad, bizwen::ascii_classification::printing) == print_bad.begin());

    constexpr auto hws_ok = " \t"sv;
    static_assert(bizwen::ascii_find_first_not_of(hws_ok, bizwen::ascii_classification::horizontal_whitespace) == hws_ok.end());
    constexpr auto hws_bad = "\n"sv;
    static_assert(bizwen::ascii_find_first_not_of(hws_bad, bizwen::ascii_classification::horizontal_whitespace) == hws_bad.begin());

    constexpr auto ws_ok = " \f\n\r\t\v"sv;
    static_assert(bizwen::ascii_find_first_not_of(ws_ok, bizwen::ascii_classification::whitespace) == ws_ok.end());
    constexpr auto ws_bad = "x"sv;
    static_assert(bizwen::ascii_find_first_not_of(ws_bad, bizwen::ascii_classification::whitespace) == ws_bad.begin());

    constexpr auto ctrl_ok = "\x00\x1F\x7F"sv;
    static_assert(bizwen::ascii_find_first_not_of(ctrl_ok, bizwen::ascii_classification::control) == ctrl_ok.end());
    constexpr auto ctrl_bad = " "sv;
    static_assert(bizwen::ascii_find_first_not_of(ctrl_bad, bizwen::ascii_classification::control) == ctrl_bad.begin());

    // Compatibility wrappers (bool all-of).
    static_assert(bizwen::ascii_is_hex_digit(hex_ok));

    // Case mapping (output).
    std::string_view mixed = "AbC123"sv;
    std::string lower(mixed.size(), '\0');
    auto lower_end = bizwen::ascii_to_lower_copy(mixed.begin(), mixed.end(), lower.begin());
    assert(lower_end == lower.end());
    assert(lower == "abc123"sv);

    std::string upper(mixed.size(), '\0');
    auto upper_end = bizwen::ascii_to_upper_copy(mixed, upper.begin());
    assert(upper_end == upper.end());
    assert(upper == "ABC123"sv);

    // Case mapping (in-place).
    std::string in_place = "HeLLo, World!";
    bizwen::ascii_to_lower(in_place);
    assert(in_place == "hello, world!"sv);
    bizwen::ascii_to_upper(in_place.begin(), in_place.end());
    assert(in_place == "HELLO, WORLD!"sv);

    // Case-insensitive compare/equality.
    static_assert(bizwen::ascii_case_insensitive_compare("AbC"sv, "aBc"sv) == std::strong_ordering::equal);
    static_assert(bizwen::ascii_case_insensitive_equals("AbC"sv, "aBc"sv));
    static_assert(!bizwen::ascii_case_insensitive_equals("AbC"sv, "aBd"sv));

    assert(bizwen::ascii_case_insensitive_equals("AaaB"sv, "aaab"sv));
    assert(bizwen::ascii_case_insensitive_compare("abc"sv, "abd"sv) == std::strong_ordering::less);
    assert(bizwen::ascii_case_insensitive_compare("abE"sv, "abd"sv) == std::strong_ordering::greater);

    std::string_view lhs = "AbC"sv;
    std::string_view rhs = "aBd"sv;
    assert(bizwen::ascii_case_insensitive_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()) ==
           std::strong_ordering::less);
}
