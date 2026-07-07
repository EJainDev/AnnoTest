
export module annotest:structs;

import :tuple;

import std;

namespace annotest {
export enum class OS {
  Windows,
  Mac,
  Linux,
  Unknown,
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

export template <fixed_string Name = "">
struct test {
  bool disabled = false;
  static constexpr auto name = Name.value;

  constexpr auto is_empty() const { return Name.is_empty(); }
};

export template <fixed_string Name = "">
struct suite {
  bool disabled = false;
  static constexpr auto name = Name.value;

  constexpr auto is_empty() const { return Name.is_empty(); }
};

export template <typename T = int>
struct after_each {};

export template <typename... Ts>
struct parameters {
  tuple<Ts...> params;

  constexpr parameters(Ts... args) : params{tuple{args...}} {}

  template <typename... Us>
  constexpr tuple<parameters<Ts..., Us>...> cross(const parameters<Us...>& other) const {
    tuple<parameters<Ts..., Us>...> result;

    template for (constexpr auto i : std::make_index_sequence<sizeof...(Us)>{}) {
      result.params.template get<i>() =
          parameters{tuple_cat(params, tuple{other.params.template get<i>()})};
    }

    return result;
  }
};

template <typename T>
concept parameters_t = is_instantiation_of<parameters, T>::value;

export template <parameters_t... Params>
struct parameterize {
  static constexpr std::size_t N = sizeof...(Params);

  tuple<Params...> parameters;

  constexpr parameterize(tuple<Params...> args) : parameters{args} {}
  constexpr parameterize(Params... args) : parameters{tuple{args...}} {}

  template <tuple_t... Us>
  constexpr parameterize(Us... args) : parameters{tuple_cat(args...)} {}
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

  template <tuple_t... Ts>
  consteval static auto of() {
    return typename[:std::meta::substitute(^^parameterize_template, get_all_types<Ts...>()):]{};
  }
};

export template <template_params_t T, parameters_t P>
struct parameterize_pair {
  P params;
};

template <typename T>
concept parameterize_pair_t = is_instantiation_of<parameterize_pair, T>::value;

template <parameterize_pair_t... Pairs>
struct parameterize_dual {
  static constexpr std::size_t N = sizeof...(Pairs);

  tuple<Pairs...> sets;

  constexpr parameterize_dual(tuple<Pairs...> args) : sets{args} {}
  constexpr parameterize_dual(Pairs... args) : sets{tuple{args...}} {}

  template <tuple_t... Ts>
  constexpr parameterize_dual(Ts... args) : sets{tuple_cat(args...)} {}
};

export template <int N>
struct requires_os {
  OS os[N];
};

template <typename... Ts>
requires_os(Ts...) -> requires_os<sizeof...(Ts)>;

export template <int N>
struct disallows_os {
  OS os[N];
};

template <typename... Ts>
disallows_os(Ts...) -> disallows_os<sizeof...(Ts)>;

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

  std::meta::info requires_os_annotation;
  std::meta::info disallows_os_annotation;

  std::meta::info parameterize_annotation;
  parameterization_types parameterization_type = parameterization_types::None;

  const char* name;

  bool disabled;
  int num_parameterizations;
  bool parameterized;
};

struct suite_test {
  std::meta::info suite;

  bool has_before_each = false;
  std::meta::info before_each;

  bool has_after_each = false;
  std::meta::info after_each;

  std::meta::info requires_os_annotation;
  std::meta::info disallows_os_annotation;

  std::meta::info parameterize_annotation;

  const char* name;

  bool disabled;
  int num_parameterizations;
  bool parameterized;

  std::vector<internal_test> children;
};

template <typename T>
concept suite_test_t = std::is_same_v<suite_test, T>;

struct namespace_test {
  bool has_before_each = false;
  std::meta::info before_each;

  bool has_after_each = false;
  std::meta::info after_each;

  std::vector<suite_test> children;
};

template <typename T>
concept namespace_test_t = std::is_same_v<namespace_test, T>;

template <internal_test Test>
consteval std::string list_tests(int depth = 0) pre(depth > 0) {
  std::string result = std::format("{:\t<{}}{}", depth, Test.name);

  if constexpr (Test.num_parameterizations > 0) {
    ++depth;
    for (auto i = 0; i < Test.num_parameterizations; ++i) {
      result += std::format("\n{:\t<{}}{}.{}", depth, Test.name, i + 1);
    }
  } else {
    result += '\n';
  }
  return std::define_static_string(result);
}

template <auto Container>
  requires(namespace_test_t<decltype(Container)> || suite_test_t<decltype(Container)>)
consteval std::string list_tests(int depth = 0) {
  std::string result = std::format("{:\t<{}}{}\n", depth, std::meta::identifier_of(Container));

  ++depth;

  template for (constexpr auto member : Container.children) {
    result += list_tests(Container.children.template get<member>(), depth);
  }

  return std::define_static_string(result);
};

template <std::meta::info Container>
consteval std::meta::info test_container() {
  if constexpr (std::meta::is_namespace(Container)) {
    return ^^namespace_test;
  }
  if constexpr (std::meta::is_class_type(Container)) {
    return ^^suite_test;
  }
  throw std::logic_error(std::format("Container must be a namespace or class. Found: {}",
                                     std::meta::identifier_of(Container)));
}

template <std::meta::info Container>
using test_container_t = typename[:test_container<Container>():];

template <std::meta::info Container>
consteval std::meta::info test_child() {
  if constexpr (std::meta::is_namespace(Container)) {
    return ^^suite_test;
  }
  if constexpr (std::meta::is_class_type(Container)) {
    return ^^internal_test;
  }
  static_assert(false, "Container must be a namespace or class");
}

template <std::meta::info Container>
using test_child_t = typename[:test_child<Container>():];
}  // namespace annotest