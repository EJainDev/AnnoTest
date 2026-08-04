export module annotest:utils;

import std;

namespace annotest {
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

  return std::define_static_string(result);
}

consteval auto getAnnotations(std::meta::info member) { return std::meta::annotations_of(member); }

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

}  // namespace annotest