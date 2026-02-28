#include "ascii.hpp"
#include "tests/generated_vectors.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

using namespace std::string_view_literals;

constexpr auto empty = ""sv;
static_assert(bizwen::ascii_find_first_not_of(empty, bizwen::ascii_classification::any) == empty.end());
static_assert(bizwen::ascii_find_last_not_of(empty, bizwen::ascii_classification::any) == empty.end());

constexpr auto hex = "deadBEEF"sv;
static_assert(bizwen::ascii_find_first_not_of(hex, bizwen::ascii_classification::hex_digit) == hex.end());

constexpr auto bad_hex = "deadBEEG"sv;
static_assert(bizwen::ascii_find_first_not_of(bad_hex, bizwen::ascii_classification::hex_digit) == bad_hex.begin() + 7);
static_assert(bizwen::ascii_find_last_not_of(bad_hex, bizwen::ascii_classification::hex_digit) == bad_hex.begin() + 7);

static_assert(bizwen::ascii_is_hex_digit(hex));
static_assert(!bizwen::ascii_is_hex_digit(bad_hex));
static_assert(bizwen::ascii_is_hex_digit(u"deadBEEF"sv));
static_assert(bizwen::ascii_is_hex_digit(U"deadBEEF"sv));
static_assert(bizwen::ascii_is_hex_digit(L"deadBEEF"sv));
static_assert(bizwen::ascii_case_insensitive_compare("AbC"sv, "aBc"sv) == std::strong_ordering::equal);
static_assert(bizwen::ascii_case_insensitive_equals("AbC"sv, "aBc"sv));
static_assert(!bizwen::ascii_case_insensitive_equals("AbC"sv, "aBd"sv));
static_assert(bizwen::ascii_case_insensitive_equals(u"AbC"sv, u"aBc"sv));
static_assert(bizwen::ascii_case_insensitive_equals(U"AbC"sv, U"aBc"sv));
static_assert(bizwen::ascii_case_insensitive_equals(L"AbC"sv, L"aBc"sv));

namespace
{
using ascii_test_vectors::blob;
using ascii_test_vectors::cmp_vector;
using ascii_test_vectors::classification_id;
using ascii_test_vectors::find_vector;
using ascii_test_vectors::map_vector;
using ascii_test_vectors::ordering;

[[nodiscard]] ordering to_ordering(std::strong_ordering o)
{
    if (o == std::strong_ordering::less)
        return ordering::less;
    if (o == std::strong_ordering::greater)
        return ordering::greater;
    return ordering::equal;
}

template <typename T>
std::vector<T> cast_bytes(std::span<unsigned char const> input)
{
    std::vector<T> out;
    out.resize(input.size());
    for (std::size_t i{}; i < input.size(); ++i)
        out[i] = static_cast<T>(input[i]);
    return out;
}

template <typename T, typename IsIt, typename IsRg>
void check_find_case(bizwen::ascii_classification cls, std::span<T const> input, std::size_t first_invalid_index,
                     std::size_t last_invalid_index, IsIt is_it, IsRg is_rg)
{
    auto expected_first = first_invalid_index == input.size() ? input.end() : input.begin() + first_invalid_index;
    auto expected_last = last_invalid_index == input.size() ? input.end() : input.begin() + last_invalid_index;
    bool expected_ok = first_invalid_index == input.size();

    auto it_first = bizwen::ascii_find_first_not_of(input.begin(), input.end(), cls);
    auto rg_first = bizwen::ascii_find_first_not_of(input, cls);
    assert(it_first == expected_first);
    assert(rg_first == expected_first);

    auto it_last = bizwen::ascii_find_last_not_of(input.begin(), input.end(), cls);
    auto rg_last = bizwen::ascii_find_last_not_of(input, cls);
    assert(it_last == expected_last);
    assert(rg_last == expected_last);

    assert(is_it(input.begin(), input.end()) == expected_ok);
    assert(is_rg(input) == expected_ok);
}

template <typename T>
void check_find_one(classification_id id, std::span<T const> input, std::size_t first_invalid_index,
                    std::size_t last_invalid_index)
{
    switch (id)
    {
    case classification_id::any:
        check_find_case(bizwen::ascii_classification::any, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_any(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_any(r); });
        return;
    case classification_id::digit:
        check_find_case(bizwen::ascii_classification::digit, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_digit(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_digit(r); });
        return;
    case classification_id::bit:
        check_find_case(bizwen::ascii_classification::bit, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_bit(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_bit(r); });
        return;
    case classification_id::octal_digit:
        check_find_case(bizwen::ascii_classification::octal_digit, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_octal_digit(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_octal_digit(r); });
        return;
    case classification_id::hex_digit:
        check_find_case(bizwen::ascii_classification::hex_digit, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_hex_digit(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_hex_digit(r); });
        return;
    case classification_id::lower:
        check_find_case(bizwen::ascii_classification::lower, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_lower(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_lower(r); });
        return;
    case classification_id::upper:
        check_find_case(bizwen::ascii_classification::upper, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_upper(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_upper(r); });
        return;
    case classification_id::alphabetic:
        check_find_case(bizwen::ascii_classification::alphabetic, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_alphabetic(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_alphabetic(r); });
        return;
    case classification_id::alphanumeric:
        check_find_case(bizwen::ascii_classification::alphanumeric, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_alphanumeric(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_alphanumeric(r); });
        return;
    case classification_id::punctuation:
        check_find_case(bizwen::ascii_classification::punctuation, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_punctuation(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_punctuation(r); });
        return;
    case classification_id::graphic:
        check_find_case(bizwen::ascii_classification::graphic, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_graphic(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_graphic(r); });
        return;
    case classification_id::printing:
        check_find_case(bizwen::ascii_classification::printing, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_printing(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_printing(r); });
        return;
    case classification_id::horizontal_whitespace:
        check_find_case(bizwen::ascii_classification::horizontal_whitespace, input, first_invalid_index,
                        last_invalid_index, [](auto b, auto e) { return bizwen::ascii_is_horizontal_whitespace(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_horizontal_whitespace(r); });
        return;
    case classification_id::whitespace:
        check_find_case(bizwen::ascii_classification::whitespace, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_whitespace(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_whitespace(r); });
        return;
    case classification_id::control:
        check_find_case(bizwen::ascii_classification::control, input, first_invalid_index, last_invalid_index,
                        [](auto b, auto e) { return bizwen::ascii_is_control(b, e); },
                        [](auto &&r) { return bizwen::ascii_is_control(r); });
        return;
    }
    assert(false);
}

void check_find()
{
    for (find_vector const &v : ascii_test_vectors::find_vectors)
    {
        blob in = ascii_test_vectors::inputs[v.input_index];
        std::span<unsigned char const> input{in.data, in.size};

        auto as_char = cast_bytes<char>(input);
        check_find_one(v.id, std::span<char const>{as_char.data(), as_char.size()}, v.first_invalid_index,
                       v.last_invalid_index);

        auto as_wchar = cast_bytes<wchar_t>(input);
        check_find_one(v.id, std::span<wchar_t const>{as_wchar.data(), as_wchar.size()}, v.first_invalid_index,
                       v.last_invalid_index);

        auto as_char8 = cast_bytes<char8_t>(input);
        check_find_one(v.id, std::span<char8_t const>{as_char8.data(), as_char8.size()}, v.first_invalid_index,
                       v.last_invalid_index);

        auto as_char16 = cast_bytes<char16_t>(input);
        check_find_one(v.id, std::span<char16_t const>{as_char16.data(), as_char16.size()}, v.first_invalid_index,
                       v.last_invalid_index);

        auto as_char32 = cast_bytes<char32_t>(input);
        check_find_one(v.id, std::span<char32_t const>{as_char32.data(), as_char32.size()}, v.first_invalid_index,
                       v.last_invalid_index);
    }

    // Values outside [0, 255] are non-ASCII and must fail all predicates.
    {
        std::array<char16_t, 1> x{static_cast<char16_t>(0x0100)};
        assert(bizwen::ascii_find_first_not_of(x, bizwen::ascii_classification::any) == x.begin());
        assert(bizwen::ascii_find_last_not_of(x, bizwen::ascii_classification::any) == x.begin());
        assert(!bizwen::ascii_is_any(x));

        assert(bizwen::ascii_find_first_not_of(x, bizwen::ascii_classification::digit) == x.begin());
        assert(bizwen::ascii_find_last_not_of(x, bizwen::ascii_classification::digit) == x.begin());
        assert(!bizwen::ascii_is_digit(x));

        assert(bizwen::ascii_find_first_not_of(x, bizwen::ascii_classification::hex_digit) == x.begin());
        assert(bizwen::ascii_find_last_not_of(x, bizwen::ascii_classification::hex_digit) == x.begin());
        assert(!bizwen::ascii_is_hex_digit(x));

        assert(bizwen::ascii_find_first_not_of(x, bizwen::ascii_classification::whitespace) == x.begin());
        assert(bizwen::ascii_find_last_not_of(x, bizwen::ascii_classification::whitespace) == x.begin());
        assert(!bizwen::ascii_is_whitespace(x));
    }
}

template <typename T>
void check_mapping_one(std::span<unsigned char const> input, std::span<unsigned char const> lower,
                       std::span<unsigned char const> upper)
{
    auto in = cast_bytes<T>(input);
    std::span<T const> s{in.data(), in.size()};

    {
        std::vector<T> out(s.size());
        auto end = bizwen::ascii_to_lower_copy(s.begin(), s.end(), out.begin());
        assert(end == out.end());
        for (std::size_t i{}; i < out.size(); ++i)
            assert(static_cast<unsigned char>(out[i]) == lower[i]);

        std::vector<T> out_range(s.size());
        auto end2 = bizwen::ascii_to_lower_copy(s, out_range.begin());
        assert(end2 == out_range.end());
        for (std::size_t i{}; i < out_range.size(); ++i)
            assert(static_cast<unsigned char>(out_range[i]) == lower[i]);
    }

    {
        std::vector<T> out(s.size());
        auto end = bizwen::ascii_to_upper_copy(s.begin(), s.end(), out.begin());
        assert(end == out.end());
        for (std::size_t i{}; i < out.size(); ++i)
            assert(static_cast<unsigned char>(out[i]) == upper[i]);

        std::vector<T> out_range(s.size());
        auto end2 = bizwen::ascii_to_upper_copy(s, out_range.begin());
        assert(end2 == out_range.end());
        for (std::size_t i{}; i < out_range.size(); ++i)
            assert(static_cast<unsigned char>(out_range[i]) == upper[i]);
    }

    {
        std::vector<T> in_place(s.begin(), s.end());
        bizwen::ascii_to_lower(in_place);
        for (std::size_t i{}; i < in_place.size(); ++i)
            assert(static_cast<unsigned char>(in_place[i]) == lower[i]);

        bizwen::ascii_to_upper(in_place.begin(), in_place.end());
        for (std::size_t i{}; i < in_place.size(); ++i)
            assert(static_cast<unsigned char>(in_place[i]) == upper[i]);
    }
}

void check_mapping()
{
    for (map_vector const &v : ascii_test_vectors::map_vectors)
    {
        blob in = ascii_test_vectors::inputs[v.input_index];
        blob expected_lower = ascii_test_vectors::lowers[v.input_index];
        blob expected_upper = ascii_test_vectors::uppers[v.input_index];

        std::span<unsigned char const> input{in.data, in.size};
        std::span<unsigned char const> lower{expected_lower.data, expected_lower.size};
        std::span<unsigned char const> upper{expected_upper.data, expected_upper.size};

        check_mapping_one<char>(input, lower, upper);
        check_mapping_one<wchar_t>(input, lower, upper);
        check_mapping_one<char8_t>(input, lower, upper);
        check_mapping_one<char16_t>(input, lower, upper);
        check_mapping_one<char32_t>(input, lower, upper);
    }

    // Non-ASCII wide code units are left unchanged by mapping.
    {
        std::array<char16_t, 2> x{u'A', static_cast<char16_t>(0x0100)};
        std::array<char16_t, 2> out{};
        auto end = bizwen::ascii_to_lower_copy(x, out.begin());
        assert(end == out.end());
        assert(out[0] == u'a');
        assert(out[1] == static_cast<char16_t>(0x0100));

        bizwen::ascii_to_upper(out);
        assert(out[0] == u'A');
        assert(out[1] == static_cast<char16_t>(0x0100));
    }
}

template <typename T>
void check_compare_one(std::span<unsigned char const> l, std::span<unsigned char const> r, ordering expected)
{
    auto lhs = cast_bytes<T>(l);
    auto rhs = cast_bytes<T>(r);

    std::span<T const> ls{lhs.data(), lhs.size()};
    std::span<T const> rs{rhs.data(), rhs.size()};

    auto ord_it = bizwen::ascii_case_insensitive_compare(ls.begin(), ls.end(), rs.begin(), rs.end());
    auto ord_rg = bizwen::ascii_case_insensitive_compare(ls, rs);

    assert(to_ordering(ord_it) == expected);
    assert(to_ordering(ord_rg) == expected);

    bool eq_it = bizwen::ascii_case_insensitive_equals(ls.begin(), ls.end(), rs.begin(), rs.end());
    bool eq_rg = bizwen::ascii_case_insensitive_equals(ls, rs);
    assert(eq_it == (expected == ordering::equal));
    assert(eq_rg == (expected == ordering::equal));
}

void check_compare()
{
    for (cmp_vector const &v : ascii_test_vectors::compare_vectors)
    {
        blob lhs = ascii_test_vectors::inputs[v.lhs_index];
        blob rhs = ascii_test_vectors::inputs[v.rhs_index];

        std::span<unsigned char const> l{lhs.data, lhs.size};
        std::span<unsigned char const> r{rhs.data, rhs.size};

        check_compare_one<char>(l, r, v.expected);
        check_compare_one<wchar_t>(l, r, v.expected);
        check_compare_one<char8_t>(l, r, v.expected);
        check_compare_one<char16_t>(l, r, v.expected);
        check_compare_one<char32_t>(l, r, v.expected);
    }

    {
        std::array<char32_t, 1> x{static_cast<char32_t>(0x0100)};
        std::array<char32_t, 1> y{static_cast<char32_t>(0x0101)};
        assert(bizwen::ascii_case_insensitive_compare(x, y) == std::strong_ordering::less);
        assert(!bizwen::ascii_case_insensitive_equals(x, y));
    }
}

} // namespace

int main()
{
    check_find();
    check_mapping();
    check_compare();
}
