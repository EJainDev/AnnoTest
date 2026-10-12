export module annotest:utils;

import std;

namespace annotest {
std::string format_runtime(std::string_view text, std::format_args values);

template <typename... Values>
std::string format_values(std::format_string<const Values&...> text, const Values&... values) {
  return format_runtime(text.get(), std::make_format_args(values...));
}

template <template <typename...> class Template, typename T>
struct is_instantiation_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_instantiation_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
struct is_instantiation_of<Template, const T> : is_instantiation_of<Template, T> {};

template <template <typename...> class Template, typename T>
struct is_instantiation_of<Template, T&> : is_instantiation_of<Template, T> {};

template <template <typename...> class Template, typename T>
struct is_instantiation_of<Template, T&&> : is_instantiation_of<Template, T> {};

template <std::integral T>
constexpr std::string to_string(T value) {
  if (value == 0) {
    return "0";
  }

  // Handle INT_MIN safely to prevent undefined behavior overflow
  using UnsignedT = std::make_unsigned_t<T>;
  UnsignedT abs_value = 0;
  bool is_negative = false;

  if constexpr (std::is_signed_v<T>) {
    if (value < 0) {
      is_negative = true;
      // Cast to unsigned before negating avoids overflow on INT_MIN
      abs_value = static_cast<UnsignedT>(0) - static_cast<UnsignedT>(value);
    } else {
      abs_value = static_cast<UnsignedT>(value);
    }
  } else {
    abs_value = value;
  }

  // Calculate required buffer size
  std::size_t size = 0;
  UnsignedT temp = abs_value;
  while (temp > 0) {
    size++;
    temp /= 10;
  }
  if (is_negative) {
    size++;
  }

  // Populate the string from right to left
  std::string result(size, '\0');
  std::size_t index = size - 1;

  while (abs_value > 0) {
    result[index--] = static_cast<char>('0' + (abs_value % 10));
    abs_value /= 10;
  }

  if (is_negative) {
    result[0] = '-';
  }

  return result;
}

consteval auto get_annotations(std::meta::info member) { return std::meta::annotations_of(member); }

export template <typename E>
  requires std::is_enum_v<E>
constexpr std::string enum_to_string(E value) {
  template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
    if (value == [:e:]) {
      return std::string(std::meta::identifier_of(e));
    }
  }

  return "<invalid enum value>";
}

template <typename T>
inline constexpr bool c_string_operand =
    std::same_as<T, std::nullptr_t> || std::same_as<T, char*> || std::same_as<T, const char*> ||
    std::same_as<T, wchar_t*> || std::same_as<T, const wchar_t*>;
template <typename T, typename U>
concept c_string_pair = c_string_operand<T> && c_string_operand<U> &&
                        (std::same_as<T, std::nullptr_t> || std::same_as<U, std::nullptr_t> ||
                         std::same_as<std::remove_cv_t<std::remove_pointer_t<T>>,
                                      std::remove_cv_t<std::remove_pointer_t<U>>>);
template <typename T, typename U>
using c_string_char =
    std::conditional_t<std::same_as<T, std::nullptr_t>,
                       std::conditional_t<std::same_as<U, std::nullptr_t>, char,
                                          std::remove_cv_t<std::remove_pointer_t<U>>>,
                       std::remove_cv_t<std::remove_pointer_t<T>>>;

template <typename Char>
Char fold_ascii(Char value) {
  return value >= Char('A') && value <= Char('Z') ? value + Char('a' - 'A') : value;
}
template <typename Char>
bool c_strings_equal(const Char* first, const Char* second, bool ignore_case) {
  if (!first || !second) return first == second;
  for (; *first && *second; ++first, ++second)
    if (ignore_case ? fold_ascii(*first) != fold_ascii(*second) : *first != *second) return false;
  return *first == *second;
}

// Encode wide characters without depending on the process locale. Invalid
// code units remain visible rather than being dropped from a diagnostic.
template <typename Char>
std::string string_bytes(std::basic_string_view<Char> text) {
  if constexpr (std::same_as<Char, char>)
    return std::string(text);
  else {
    std::string result;
    for (std::size_t i = 0; i < text.size(); ++i) {
      auto code = static_cast<std::uint32_t>(text[i]);
      if constexpr (sizeof(Char) == 2) {
        if (code >= 0xd800 && code <= 0xdbff && i + 1 < text.size()) {
          auto low = static_cast<std::uint32_t>(text[i + 1]);
          if (low >= 0xdc00 && low <= 0xdfff) {
            code = 0x10000 + ((code - 0xd800) << 10) + low - 0xdc00;
            ++i;
          }
        }
      }
      if (code > 0x10ffff || (code >= 0xd800 && code <= 0xdfff))
        result += format_values("\\u{{{:x}}}", code);
      else if (code < 0x80)
        result += static_cast<char>(code);
      else {
        if (code < 0x800)
          result += static_cast<char>(0xc0 | (code >> 6));
        else {
          if (code < 0x10000)
            result += static_cast<char>(0xe0 | (code >> 12));
          else {
            result += static_cast<char>(0xf0 | (code >> 18));
            result += static_cast<char>(0x80 | ((code >> 12) & 0x3f));
          }
          result += static_cast<char>(0x80 | ((code >> 6) & 0x3f));
        }
        result += static_cast<char>(0x80 | (code & 0x3f));
      }
    }
    return result;
  }
}
inline std::string quote_string(std::string_view text) {
  std::string result = "\"";
  for (unsigned char c : text) {
    switch (c) {
      case '\\':
        result += "\\\\";
        break;
      case '"':
        result += "\\\"";
        break;
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\0':
        result += "\\0";
        break;
      default:
        if (c < 0x20 || c == 0x7f)
          result += format_values("\\x{:02x}", c);
        else
          result += c;
    }
  }
  return result + "\"";
}
template <typename Char>
std::string print_c_string(const Char* value) {
  if (!value) return "nullptr";
  return quote_string(string_bytes(std::basic_string_view<Char>(value)));
}
template <typename T, typename U>
  requires c_string_pair<T, U>
std::string string_comparison_diagnostic(T first, U second, bool equal, bool ignore_case) {
  using Char = c_string_char<T, U>;
  const Char* expected = first;
  const Char* actual = second;
  auto result = format_values(
      "Expected strings {}{}: expected {}, got {}", equal ? "equal" : "not equal",
      ignore_case ? " ignoring ASCII case" : "", print_c_string(expected), print_c_string(actual));
  if (equal && expected && actual) {
    std::size_t index = 0;
    while (expected[index] && actual[index] &&
           (ignore_case ? fold_ascii(expected[index]) == fold_ascii(actual[index])
                        : expected[index] == actual[index]))
      ++index;
    result += format_values("\nFirst difference at index {}", index);
  }
  return result;
}

export template <typename T>
std::string print_value(const T& value) {
  if constexpr (requires(std::ostream* stream) { print_to(value, stream); }) {
    std::ostringstream stream;
    print_to(value, &stream);
    return stream.str();
  } else if constexpr (c_string_operand<std::remove_cvref_t<T>>) {
    if constexpr (std::same_as<std::remove_cvref_t<T>, std::nullptr_t>)
      return "nullptr";
    else
      return print_c_string(value);
  } else if constexpr (std::is_array_v<T> &&
                       (std::same_as<std::remove_cv_t<std::remove_extent_t<T>>, char> ||
                        std::same_as<std::remove_cv_t<std::remove_extent_t<T>>, wchar_t>)) {
    using Char = std::remove_cv_t<std::remove_extent_t<T>>;
    auto end = std::ranges::find(value, Char{});
    return quote_string(string_bytes(std::basic_string_view<Char>(value, end - value)));
  } else if constexpr (std::same_as<T, std::wstring> || std::same_as<T, std::wstring_view>) {
    return quote_string(string_bytes(std::wstring_view(value)));
  } else if constexpr (std::is_default_constructible_v<
                           std::formatter<std::remove_cvref_t<T>, char>>)
    return format_values("{}", value);
  else if constexpr (std::ranges::input_range<T>) {
    std::string text = "[";
    unsigned count = 0;
    for (const auto& item : value) {
      if (count++) text += ", ";
      if (count > 100) {
        text += "...";
        break;
      }
      text += print_value(item);
    }
    return text + "]";
  } else if constexpr (requires(std::ostream& stream) { stream << value; }) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
  } else
    return std::string("<") + typeid(T).name() + ": unprintable>";
}

template <typename... Ts>
std::string diagnostic_format(std::string_view text, const Ts&... values) {
  auto strings = std::tuple{print_value(values)...};
  return std::apply(
      [&](auto&... args) { return format_runtime(text, std::make_format_args(args...)); }, strings);
}

export template <typename T, typename U>
std::string difference(const T& expected, const U& actual) {
  if constexpr (c_string_pair<T, U>) {
    return format_values("\nPointer comparison: expected @ {}, actual @ {}",
                       static_cast<const void*>(expected), static_cast<const void*>(actual));
  } else if constexpr (std::ranges::input_range<T> && std::ranges::input_range<U>) {
    auto a = std::ranges::begin(expected), b = std::ranges::begin(actual);
    std::size_t index = 0;
    while (a != std::ranges::end(expected) && b != std::ranges::end(actual)) {
      if constexpr (requires { *a == *b; }) {
        if (!(*a == *b))
          return format_values("\nFirst difference at index {}: expected {}, got {}", index,
                             print_value(*a), print_value(*b));
      }
      ++a;
      ++b;
      ++index;
    }
    if (a != std::ranges::end(expected) || b != std::ranges::end(actual))
      return format_values("\nLength differs after {} elements", index);
  }
  return {};
}

consteval std::vector<std::meta::info> expanded_annotations(std::meta::info member, int depth = 0) {
  if (depth > 16) throw std::logic_error("Composed annotation nesting exceeds 16 levels");
  std::vector<std::meta::info> result;
  for (auto a : std::meta::annotations_of(member)) {
    result.push_back(a);
    auto nested = expanded_annotations(std::meta::type_of(a), depth + 1);
    result.insert(result.end(), nested.begin(), nested.end());
  }
  return result;
}

consteval std::vector<std::meta::info> inherited_members(std::meta::info type) {
  auto own = std::meta::members_of(type, std::meta::access_context::unchecked());
  std::vector<std::meta::info> result;
  for (auto base : std::meta::bases_of(type, std::meta::access_context::unchecked())) {
    if (!std::meta::is_public(base)) continue;
    for (auto member : inherited_members(std::meta::type_of(base))) {
      bool hidden =
          std::meta::has_identifier(member) && std::ranges::any_of(own, [&](auto candidate) {
            return std::meta::has_identifier(candidate) &&
                   (std::meta::is_function(candidate) || std::meta::is_template(candidate)) &&
                   std::meta::identifier_of(candidate) == std::meta::identifier_of(member);
          });
      if (!hidden && std::ranges::find(result, member) == result.end()) result.push_back(member);
    }
  }
  result.insert(result.end(), own.begin(), own.end());
  return result;
}
}  // namespace annotest
