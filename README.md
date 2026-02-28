# ascii-afo

A plain header-only ASCII classification/transform library, but with a modern C++23 iterator/range API.

This repository exposes the API as [Algorithm Function Objects](https://en.cppreference.com/w/cpp/algorithm/ranges#Algorithm_function_objects) (AFOs),
which enables the call style `bizwen::ascii_is_digit(r)` while keeping all overloads as a single callable object per API name.
Another way is to expose the same API as function templates, see the sibling repository [ascii-tool](https://github.com/yexuanxiao/ascii-tool).

## Synopsis

```cpp
// All callables are constexpr objects.
enum class ascii_classification: /* unspecified */
{
    any = /* unspecified */,
    digit = /* unspecified */,
    bit = /* unspecified */,
    octal_digit = /* unspecified */,
    hex_digit = /* unspecified */,
    lower = /* unspecified */,
    upper = /* unspecified */,
    alphabetic = /* unspecified */,
    alphanumeric = /* unspecified */,
    punctuation = /* unspecified */,
    graphic = /* unspecified */,
    printing = /* unspecified */,
    horizontal_whitespace = /* unspecified */,
    whitespace = /* unspecified */,
    control = /* unspecified */,
};
// AFOs
// Find (returns iterator to the first/last invalid element; end => all valid).
template <typename In>
In ascii_find_first_not_of(In begin, In end, ascii_classification cls);
template <ascii_classification Class, typename R>
std::ranges::iterator_t<R> ascii_find_first_not_of(R&& r, ascii_classification cls); // requires borrowed_range

template <typename In>
In ascii_find_last_not_of(In begin, In end, ascii_classification cls);
template <ascii_classification Class, typename R>
std::ranges::iterator_t<R> ascii_find_last_not_of(R&& r, ascii_classification cls); // requires borrowed_range

template <typename In>
bool ascii_is_any(In begin, In end);
template <typename R>
bool ascii_is_any(R&& r);
template <typename In>
bool ascii_is_digit(In begin, In end);
template <typename R>
bool ascii_is_digit(R&& r);
template <typename In>
bool ascii_is_bit(In begin, In end);
template <typename R>
bool ascii_is_bit(R&& r);
template <typename In>
bool ascii_is_octal_digit(In begin, In end);
template <typename R>
bool ascii_is_octal_digit(R&& r);
template <typename In>
bool ascii_is_hex_digit(In begin, In end);
template <typename R>
bool ascii_is_hex_digit(R&& r);
template <typename In>
bool ascii_is_lower(In begin, In end);
template <typename R>
bool ascii_is_lower(R&& r);
template <typename In>
bool ascii_is_upper(In begin, In end);
template <typename R>
bool ascii_is_upper(R&& r);
template <typename In>
bool ascii_is_alphabetic(In begin, In end);
template <typename R>
bool ascii_is_alphabetic(R&& r);
template <typename In>
bool ascii_is_alphanumeric(In begin, In end);
template <typename R>
bool ascii_is_alphanumeric(R&& r);
template <typename In>
bool ascii_is_punctuation(In begin, In end);
template <typename R>
bool ascii_is_punctuation(R&& r);
template <typename In>
bool ascii_is_graphic(In begin, In end);
template <typename R>
bool ascii_is_graphic(R&& r);
template <typename In>
bool ascii_is_printing(In begin, In end);
template <typename R>
bool ascii_is_printing(R&& r);
template <typename In>
bool ascii_is_horizontal_whitespace(In begin, In end);
template <typename R>
bool ascii_is_horizontal_whitespace(R&& r);
template <typename In>
bool ascii_is_whitespace(In begin, In end);
template <typename R>
bool ascii_is_whitespace(R&& r);
template <typename In>
bool ascii_is_control(In begin, In end);
template <typename R>
bool ascii_is_control(R&& r);

// Case mapping (output)
template <typename In, typename Out>
Out ascii_to_lower_copy(In begin, In end, Out out);
template <typename R, typename Out>
Out ascii_to_lower_copy(R&& r, Out out);

template <typename In, typename Out>
Out ascii_to_upper_copy(In begin, In end, Out out);
template <typename R, typename Out>
Out ascii_to_upper_copy(R&& r, Out out);

// Case mapping (in-place)
template <typename InOut>
void ascii_to_lower(InOut begin, InOut end);
template <typename R>
void ascii_to_lower(R&& r);

template <typename InOut>
void ascii_to_upper(InOut begin, InOut end);
template <typename R>
void ascii_to_upper(R&& r);

// Case-insensitive compare/equality
template <typename In1, typename In2>
std::strong_ordering ascii_case_insensitive_compare(In1 b1, In1 e1, In2 b2, In2 e2);
template <typename R1, typename R2>
std::strong_ordering ascii_case_insensitive_compare(R1&& r1, R2&& r2);

template <typename In1, typename In2>
bool ascii_case_insensitive_equals(In1 b1, In1 e1, In2 b2, In2 e2);
template <typename R1, typename R2>
bool ascii_case_insensitive_equals(R1&& r1, R2&& r2);
```

`R` must model `std::contiguous_range` and `In` must satisfy `std::contiguous_iterator`.

If [`begin`, `end`) is not a valid range, the behavior is undefined.

For `ascii_to_lower_copy/ascii_to_upper_copy` output overloads, if the input and output ranges overlap, the behavior is
undefined.

## Example

```cpp
#include "ascii.hpp"

#include <cassert>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

int main()
{
    std::string_view s = "deadBEEF"sv;
    assert(bizwen::ascii_find_first_not_of(s, bizwen::ascii_classification::hex_digit) == s.end());

    std::string_view bad = "deadBEEG"sv;
    auto it = bizwen::ascii_find_first_not_of(bad, bizwen::ascii_classification::hex_digit);
    assert(it != bad.end());
    assert(*it == 'G');

    std::string lower(s.size(), '\0');
    bizwen::ascii_to_lower_copy(s, lower.begin());
    assert(lower == "deadbeef"sv);

    std::string in_place = "Hello, World!";
    bizwen::ascii_to_upper(in_place);
    assert(in_place == "HELLO, WORLD!"sv);

    assert(bizwen::ascii_case_insensitive_equals("AbC"sv, "aBc"sv));
}
```

## Module support

Compile the `ascii_afo.cpp` file as a C++ module interface unit, allowing the library to be used as a module.
Note that it depends on the `std` module.
