
export module annotest:structs;

import :utils;

import std;

import structural_tuple;

template <typename... Ts>
using tuple = structural_tuple::tuple<Ts...>;

namespace annotest {
export enum class operating_system {
  windows,
  macos,
  linux_os,
  unknown,
};

export enum class fixture_lifecycle { per_test, shared };
export struct nested {};
export struct timeout {
  double seconds;
};
struct condition_reason {
  char reason[256]{};
  constexpr explicit condition_reason(const char* message) {
    if (!message) return;
    std::size_t index = 0;
    for (; message[index]; ++index) {
      if (index == 255)
        throw std::logic_error("Execution condition reasons must not exceed 255 bytes");
      reason[index] = message[index];
    }
  }
};
export template <auto Predicate>
struct enabled_if : condition_reason {
  static constexpr auto predicate = Predicate;
  constexpr enabled_if(const char* reason = "Execution condition not met")
      : condition_reason(reason) {}
};
export template <auto Predicate>
struct disabled_if : condition_reason {
  static constexpr auto predicate = Predicate;
  constexpr disabled_if(const char* reason = "Execution condition matched")
      : condition_reason(reason) {}
};

export template <typename T = int>
struct before_each {};

template <std::size_t N>
struct fixed_string {
  char value[N];

  constexpr fixed_string(const char (&str)[N]) {
    for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
  }

  static constexpr bool is_empty() { return N == 1; }
};

export template <fixed_string Name>
struct tag {
  static constexpr auto name = Name.value;
};
export template <auto Generator>
struct name_generator {
  static constexpr auto generate = Generator;
};
export template <auto Generator>
struct suite_name_generator {
  static constexpr auto generate = Generator;
};
export constexpr std::string readable_name(std::string_view name) {
  std::string result;
  for (char c : name) result += c == '_' ? ' ' : c;
  return result;
}
export template <auto Provider>
struct parameter_source {
  static constexpr auto provider = Provider;
};
export template <auto Provider = nullptr>
struct test_factory {};
export template <typename... Ts>
struct named_case {
  std::string name;
  std::tuple<Ts...> values;
};
export template <typename... Ts>
auto case_of(std::string name, Ts&&... values) {
  return named_case<std::decay_t<Ts>...>{std::move(name), {std::forward<Ts>(values)...}};
}

export template <fixed_string Name = "">
struct test {
  bool disabled = false;
  unsigned repetitions = 1;
  static constexpr auto name = Name.value;

  constexpr auto is_empty() const { return Name.is_empty(); }
};

export template <fixed_string Name = "">
struct suite {
  bool disabled = false;
  fixture_lifecycle lifecycle = fixture_lifecycle::per_test;
  static constexpr auto name = Name.value;

  constexpr auto is_empty() const { return Name.is_empty(); }
};

export template <typename T = int>
struct after_each {};

export struct before_all {};
export struct after_all {};

export template <typename... Ts>
struct parameters {
  tuple<Ts...> params;

  constexpr parameters(Ts... args) : params{tuple{args...}} {}

  template <typename... Us>
  constexpr tuple<parameters<Ts..., Us>...> cross(const parameters<Us...>& other) const {
    if constexpr (sizeof...(Us) == 0) {
      return tuple<>{};
    } else {
      return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return [&]<std::size_t... Js>(std::index_sequence<Js...>) {
          auto append = [&]<std::size_t J>() {
            return parameters<Ts..., Us... [J]> {
              params.template get<Is>()..., other.params.template get<J>()
            };
          };
          return tuple{append.template operator()<Js>()...};
        }(std::make_index_sequence<sizeof...(Us)>{});
      }(std::make_index_sequence<sizeof...(Ts)>{});
    }
  }
};

template <typename T>
concept parameters_t = is_instantiation_of<parameters, T>::value;

// Construct concatenated values directly. StructuralTuple's tuple_cat first
// default-constructs its elements, which parameters and parameterize_pair cannot do.
template <structural_tuple::tuple_t T>
constexpr auto as_std_tuple(const T& values) {
  return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    return std::tuple{values.template get<Is>()...};
  }(std::make_index_sequence<T::size()>{});
}

template <structural_tuple::tuple_t... Ts>
constexpr auto concatenate_tuples(const Ts&... values) {
  return std::apply([](auto... elements) { return tuple{elements...}; },
                    std::tuple_cat(as_std_tuple(values)...));
}

export template <parameters_t... Params>
struct parameterize {
  static constexpr std::size_t N = sizeof...(Params);

  tuple<Params...> parameters;

  constexpr parameterize(tuple<Params...> args) : parameters{args} {}
  constexpr parameterize(Params... args) : parameters{tuple{args...}} {}

  template <structural_tuple::tuple_t... Us>
  constexpr parameterize(Us... args) : parameters{concatenate_tuples(args...)} {}
};

export template <typename... Ts>
struct template_params {
  template <typename Other>
  struct cross;

  template <typename... Us>
  struct cross<template_params<Us...>> {
    using type = tuple<template_params<Ts..., Us>...>;
  };

  template <typename Other>
  using cross_t = typename cross<Other>::type;
};

template <typename T>
concept template_params_t = is_instantiation_of<template_params, T>::value;

export template <template_params_t... Args>
struct parameterize_template {
  static constexpr std::size_t N = sizeof...(Args);

  template <structural_tuple::tuple_t... Ts>
  consteval static auto of() {
    using combined = structural_tuple::tuple_cat_t<Ts...>;
    return typename[:std::meta::substitute(^^annotest::parameterize_template,
                                           combined::types_refl):]{};
  }
};

export template <template_params_t T, parameters_t P>
struct parameterize_pair {
  P params;
};

template <typename T>
concept parameterize_pair_t = is_instantiation_of<parameterize_pair, T>::value;

export template <parameterize_pair_t... Pairs>
struct parameterize_dual {
  static constexpr std::size_t N = sizeof...(Pairs);

  tuple<Pairs...> sets;

  constexpr parameterize_dual(tuple<Pairs...> args) : sets{args} {}
  constexpr parameterize_dual(Pairs... args) : sets{tuple{args...}} {}

  template <structural_tuple::tuple_t... Ts>
  constexpr parameterize_dual(Ts... args) : sets{concatenate_tuples(args...)} {}
};

export template <int N>
struct requires_os {
  operating_system os[N];
};

template <typename... Ts>
requires_os(Ts...) -> requires_os<sizeof...(Ts)>;

export template <int N>
struct disallows_os {
  operating_system os[N];
};

template <typename... Ts>
disallows_os(Ts...) -> disallows_os<sizeof...(Ts)>;

struct static_tags {
  const char* const* data = nullptr;
  std::size_t size = 0;
  constexpr static_tags() = default;
  constexpr static_tags(std::span<const char* const> labels)
      : data(labels.data()), size(labels.size()) {}
  constexpr auto begin() const { return data; }
  constexpr auto end() const { return data ? data + size : data; }
};

enum class parameterization_types {
  None,
  Value,
  Template,
  Dual,
};

// Has to be a structural type to pass to pass to functions since passing `std::meta::info` as a
// regular parameter removes `constexpr` from it creating an invalid use case.
struct internal_test {
  std::meta::info test;
  std::meta::info raw_test;

  bool has_requires_os = false;
  std::meta::info requires_os_annotation = ^^int;
  bool has_disallows_os = false;
  std::meta::info disallows_os_annotation = ^^int;

  std::meta::info parameterize_annotation = ^^int;
  parameterization_types parameterization_type = parameterization_types::None;

  const char* name;

  static_tags tags;
  double timeout_seconds = 0;
  bool runtime_parameters = false;
  bool dynamic = false;
  bool disabled = false;
  int num_parameterizations = 0;
  bool parameterized = false;
  unsigned repetitions = 1;
};

struct static_hooks {
  const std::meta::info* data = nullptr;
  std::size_t size = 0;
  consteval static_hooks() = default;
  consteval static_hooks(std::span<const std::meta::info> hooks)
      : data(hooks.data()), size(hooks.size()) {}
};

struct suite_test {
  std::meta::info suite;
  const char* name = nullptr;
  bool disabled = false;

  fixture_lifecycle lifecycle = fixture_lifecycle::per_test;
  static_tags tags;
  double timeout_seconds = 0;

  static_hooks before_hooks, after_hooks, before_all_hooks, after_all_hooks;
  bool has_before_each = false;
  std::meta::info before_each = ^^int;

  bool has_after_each = false;
  std::meta::info after_each = ^^int;
};
}  // namespace annotest
