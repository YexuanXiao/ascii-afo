#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include "./benchmark.hpp"

namespace
{

constexpr std::array<std::size_t, 4> kFixedLengths{16, 64, 256, 4096};

void FixedLengths(benchmark::internal::Benchmark *b)
{
    for (auto len : kFixedLengths)
        b->Arg(static_cast<std::int64_t>(len));
}

using table_predicate_t = bool (*)(char8_t) noexcept;

template <bizwen::ascii_classification Cls>
constexpr table_predicate_t table_predicate() noexcept
{
    if constexpr (Cls == bizwen::ascii_classification::any)
        return is_ascii_any_u8;
    else if constexpr (Cls == bizwen::ascii_classification::digit)
        return is_ascii_digit_u8;
    else if constexpr (Cls == bizwen::ascii_classification::bit)
        return is_ascii_bit_u8;
    else if constexpr (Cls == bizwen::ascii_classification::octal_digit)
        return is_ascii_octal_digit_u8;
    else if constexpr (Cls == bizwen::ascii_classification::hex_digit)
        return is_ascii_hex_digit_u8;
    else if constexpr (Cls == bizwen::ascii_classification::lower)
        return is_ascii_lower_u8;
    else if constexpr (Cls == bizwen::ascii_classification::upper)
        return is_ascii_upper_u8;
    else if constexpr (Cls == bizwen::ascii_classification::alphabetic)
        return is_ascii_alphabetic_u8;
    else if constexpr (Cls == bizwen::ascii_classification::alphanumeric)
        return is_ascii_alphanumeric_u8;
    else if constexpr (Cls == bizwen::ascii_classification::punctuation)
        return is_ascii_punctuation_u8;
    else if constexpr (Cls == bizwen::ascii_classification::graphic)
        return is_ascii_graphic_u8;
    else if constexpr (Cls == bizwen::ascii_classification::printing)
        return is_ascii_printing_u8;
    else if constexpr (Cls == bizwen::ascii_classification::horizontal_whitespace)
        return is_ascii_horizontal_whitespace_u8;
    else if constexpr (Cls == bizwen::ascii_classification::whitespace)
        return is_ascii_whitespace_u8;
    else if constexpr (Cls == bizwen::ascii_classification::control)
        return is_ascii_control_u8;
    else
        static_assert(Cls == bizwen::ascii_classification::any, "unhandled ascii_classification");
}

constexpr std::u8string_view kPatternPunctuation = u8R"(!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)";
constexpr std::u8string_view kPatternGraphic =
    u8R"(!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";
constexpr std::u8string_view kPatternPrinting =
    u8R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

constexpr std::array<char8_t, 2> kPatternHorizontalWhitespace{u8' ', u8'\t'};
constexpr std::array<char8_t, 6> kPatternWhitespace{u8' ', u8'\f', u8'\n', u8'\r', u8'\t', u8'\v'};
constexpr std::array<char8_t, 33> kPatternControl{
    u8'\x00', u8'\x01', u8'\x02', u8'\x03', u8'\x04', u8'\x05', u8'\x06', u8'\x07', u8'\x08', u8'\x09',
    u8'\x0A', u8'\x0B', u8'\x0C', u8'\x0D', u8'\x0E', u8'\x0F', u8'\x10', u8'\x11', u8'\x12', u8'\x13',
    u8'\x14', u8'\x15', u8'\x16', u8'\x17', u8'\x18', u8'\x19', u8'\x1A', u8'\x1B', u8'\x1C', u8'\x1D',
    u8'\x1E', u8'\x1F', u8'\x7F'};

template <bizwen::ascii_classification Cls>
constexpr std::span<const char8_t> valid_pattern() noexcept
{
    if constexpr (Cls == bizwen::ascii_classification::any)
        return std::span<const char8_t>(kPatternPrinting.data(), kPatternPrinting.size());
    else if constexpr (Cls == bizwen::ascii_classification::digit)
    {
        constexpr std::u8string_view pattern = u8"0123456789";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::bit)
    {
        constexpr std::u8string_view pattern = u8"01";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::octal_digit)
    {
        constexpr std::u8string_view pattern = u8"01234567";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::hex_digit)
    {
        constexpr std::u8string_view pattern = u8"0123456789abcdefABCDEF";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::lower)
    {
        constexpr std::u8string_view pattern = u8"abcdefghijklmnopqrstuvwxyz";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::upper)
    {
        constexpr std::u8string_view pattern = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::alphabetic)
    {
        constexpr std::u8string_view pattern = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::alphanumeric)
    {
        constexpr std::u8string_view pattern = u8"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        return std::span<const char8_t>(pattern.data(), pattern.size());
    }
    else if constexpr (Cls == bizwen::ascii_classification::punctuation)
        return std::span<const char8_t>(kPatternPunctuation.data(), kPatternPunctuation.size());
    else if constexpr (Cls == bizwen::ascii_classification::graphic)
        return std::span<const char8_t>(kPatternGraphic.data(), kPatternGraphic.size());
    else if constexpr (Cls == bizwen::ascii_classification::printing)
        return std::span<const char8_t>(kPatternPrinting.data(), kPatternPrinting.size());
    else if constexpr (Cls == bizwen::ascii_classification::horizontal_whitespace)
        return std::span<const char8_t>(kPatternHorizontalWhitespace.data(), kPatternHorizontalWhitespace.size());
    else if constexpr (Cls == bizwen::ascii_classification::whitespace)
        return std::span<const char8_t>(kPatternWhitespace.data(), kPatternWhitespace.size());
    else if constexpr (Cls == bizwen::ascii_classification::control)
        return std::span<const char8_t>(kPatternControl.data(), kPatternControl.size());
    else
        static_assert(Cls == bizwen::ascii_classification::any, "unhandled ascii_classification");
}

template <bizwen::ascii_classification Cls>
std::vector<char8_t> make_valid_input(std::size_t len)
{
    std::vector<char8_t> data(len);
    if (len == 0)
        return data;

    constexpr auto pattern = valid_pattern<Cls>();
    for (std::size_t i = 0; i != data.size(); ++i)
        data[i] = pattern[i % pattern.size()];

    return data;
}

template <bizwen::ascii_classification Cls>
void BM_ascii_find_first_not_of(benchmark::State &state)
{
    auto len = static_cast<std::size_t>(state.range(0));
    auto data = make_valid_input<Cls>(len);
    auto rg = std::span<const char8_t>(data.data(), data.size());

    for (auto _ : state)
    {
        auto it = bizwen::ascii_find_first_not_of(rg, Cls);
        benchmark::DoNotOptimize(it);
    }

    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(len));
}

template <bizwen::ascii_classification Cls>
void BM_ranges_find_if_not_fnptr(benchmark::State &state)
{
    auto len = static_cast<std::size_t>(state.range(0));
    auto data = make_valid_input<Cls>(len);
    auto rg = std::span<const char8_t>(data.data(), data.size());

    constexpr auto pred = table_predicate<Cls>();
    for (auto _ : state)
    {
        auto it = std::ranges::find_if_not(rg, pred);
        benchmark::DoNotOptimize(it);
    }

    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(len));
}

template <bizwen::ascii_classification Cls>
void BM_ranges_find_if_not_table_lambda(benchmark::State &state)
{
    auto len = static_cast<std::size_t>(state.range(0));
    auto data = make_valid_input<Cls>(len);
    auto rg = std::span<const char8_t>(data.data(), data.size());

    auto pred = [](auto c) noexcept -> bool {
        if constexpr (Cls == bizwen::ascii_classification::any)
            return is_ascii_any_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::digit)
            return is_ascii_digit_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::bit)
            return is_ascii_bit_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::octal_digit)
            return is_ascii_octal_digit_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::hex_digit)
            return is_ascii_hex_digit_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::lower)
            return is_ascii_lower_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::upper)
            return is_ascii_upper_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::alphabetic)
            return is_ascii_alphabetic_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::alphanumeric)
            return is_ascii_alphanumeric_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::punctuation)
            return is_ascii_punctuation_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::graphic)
            return is_ascii_graphic_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::printing)
            return is_ascii_printing_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::horizontal_whitespace)
            return is_ascii_horizontal_whitespace_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::whitespace)
            return is_ascii_whitespace_u8(static_cast<char8_t>(c));
        else if constexpr (Cls == bizwen::ascii_classification::control)
            return is_ascii_control_u8(static_cast<char8_t>(c));
        else
            static_assert(Cls == bizwen::ascii_classification::any, "unhandled ascii_classification");
    };

    for (auto _ : state)
    {
        auto it = std::ranges::find_if_not(rg, pred);
        benchmark::DoNotOptimize(it);
    }

    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(len));
}

template <bizwen::ascii_classification Cls>
void BM_ranges_find_if_not_naive_lambda(benchmark::State &state)
{
    auto len = static_cast<std::size_t>(state.range(0));
    auto data = make_valid_input<Cls>(len);
    auto rg = std::span<const char8_t>(data.data(), data.size());

    auto pred = [](auto c) noexcept -> bool {
        auto v = static_cast<unsigned int>(static_cast<char8_t>(c));

        if constexpr (Cls == bizwen::ascii_classification::any)
            return v <= 0x7F;
        else if constexpr (Cls == bizwen::ascii_classification::digit)
            return v >= static_cast<unsigned int>(u8'0') && v <= static_cast<unsigned int>(u8'9');
        else if constexpr (Cls == bizwen::ascii_classification::bit)
            return v == static_cast<unsigned int>(u8'0') || v == static_cast<unsigned int>(u8'1');
        else if constexpr (Cls == bizwen::ascii_classification::octal_digit)
            return v >= static_cast<unsigned int>(u8'0') && v <= static_cast<unsigned int>(u8'7');
        else if constexpr (Cls == bizwen::ascii_classification::hex_digit)
        {
            return (v >= static_cast<unsigned int>(u8'0') && v <= static_cast<unsigned int>(u8'9')) ||
                   (v >= static_cast<unsigned int>(u8'a') && v <= static_cast<unsigned int>(u8'f')) ||
                   (v >= static_cast<unsigned int>(u8'A') && v <= static_cast<unsigned int>(u8'F'));
        }
        else if constexpr (Cls == bizwen::ascii_classification::lower)
            return v >= static_cast<unsigned int>(u8'a') && v <= static_cast<unsigned int>(u8'z');
        else if constexpr (Cls == bizwen::ascii_classification::upper)
            return v >= static_cast<unsigned int>(u8'A') && v <= static_cast<unsigned int>(u8'Z');
        else if constexpr (Cls == bizwen::ascii_classification::alphabetic)
        {
            return (v >= static_cast<unsigned int>(u8'a') && v <= static_cast<unsigned int>(u8'z')) ||
                   (v >= static_cast<unsigned int>(u8'A') && v <= static_cast<unsigned int>(u8'Z'));
        }
        else if constexpr (Cls == bizwen::ascii_classification::alphanumeric)
        {
            return (v >= static_cast<unsigned int>(u8'0') && v <= static_cast<unsigned int>(u8'9')) ||
                   (v >= static_cast<unsigned int>(u8'a') && v <= static_cast<unsigned int>(u8'z')) ||
                   (v >= static_cast<unsigned int>(u8'A') && v <= static_cast<unsigned int>(u8'Z'));
        }
        else if constexpr (Cls == bizwen::ascii_classification::punctuation)
        {
            return (v >= 0x21 && v <= 0x2F) || (v >= 0x3A && v <= 0x40) || (v >= 0x5B && v <= 0x60) ||
                   (v >= 0x7B && v <= 0x7E);
        }
        else if constexpr (Cls == bizwen::ascii_classification::graphic)
            return v >= 0x21 && v <= 0x7E;
        else if constexpr (Cls == bizwen::ascii_classification::printing)
            return v >= 0x20 && v <= 0x7E;
        else if constexpr (Cls == bizwen::ascii_classification::horizontal_whitespace)
            return v == 0x20 || v == 0x09;
        else if constexpr (Cls == bizwen::ascii_classification::whitespace)
            return v == 0x20 || v == 0x0C || v == 0x0A || v == 0x0D || v == 0x09 || v == 0x0B;
        else if constexpr (Cls == bizwen::ascii_classification::control)
            return v <= 0x1F || v == 0x7F;
        else
            static_assert(Cls == bizwen::ascii_classification::any, "unhandled ascii_classification");
    };

    for (auto _ : state)
    {
        auto it = std::ranges::find_if_not(rg, pred);
        benchmark::DoNotOptimize(it);
    }

    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(len));
}

#define ASCII_AFO_BENCHMARK_CLS(Cls)                                                                                  \
    BENCHMARK_TEMPLATE(BM_ascii_find_first_not_of, bizwen::ascii_classification::Cls)                                 \
        ->Name("ascii_find_first_not_of/" #Cls)                                                                        \
        ->Apply(FixedLengths);                                                                                         \
    BENCHMARK_TEMPLATE(BM_ranges_find_if_not_fnptr, bizwen::ascii_classification::Cls)                                 \
        ->Name("ranges/find_if_not/fnptr/" #Cls)                                                                       \
        ->Apply(FixedLengths);                                                                                         \
    BENCHMARK_TEMPLATE(BM_ranges_find_if_not_table_lambda, bizwen::ascii_classification::Cls)                          \
        ->Name("ranges/find_if_not/lambda/" #Cls)                                                                      \
        ->Apply(FixedLengths);                                                                                         \
    BENCHMARK_TEMPLATE(BM_ranges_find_if_not_naive_lambda, bizwen::ascii_classification::Cls)                          \
        ->Name("ranges/find_if_not/naive/" #Cls)                                                                       \
        ->Apply(FixedLengths)

ASCII_AFO_BENCHMARK_CLS(any);
ASCII_AFO_BENCHMARK_CLS(digit);
ASCII_AFO_BENCHMARK_CLS(bit);
ASCII_AFO_BENCHMARK_CLS(octal_digit);
ASCII_AFO_BENCHMARK_CLS(hex_digit);
ASCII_AFO_BENCHMARK_CLS(lower);
ASCII_AFO_BENCHMARK_CLS(upper);
ASCII_AFO_BENCHMARK_CLS(alphabetic);
ASCII_AFO_BENCHMARK_CLS(alphanumeric);
ASCII_AFO_BENCHMARK_CLS(punctuation);
ASCII_AFO_BENCHMARK_CLS(graphic);
ASCII_AFO_BENCHMARK_CLS(printing);
ASCII_AFO_BENCHMARK_CLS(horizontal_whitespace);
ASCII_AFO_BENCHMARK_CLS(whitespace);
ASCII_AFO_BENCHMARK_CLS(control);

#undef ASCII_AFO_BENCHMARK_CLS

} // namespace
