
export module annotest:structs;

import :utils;

import std;

import structural_tuple;

template <typename... Ts>
using tuple = structural_tuple::tuple<Ts...>;

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
};

template <typename T>
concept parameters_t = is_instantiation_of<parameters, T>::value;

export template <parameters_t... Params>
struct parameterize {
  static constexpr std::size_t N = sizeof...(Params);

  tuple<Params...> parameters;

  constexpr parameterize(tuple<Params...> args) : parameters{args} {}
  constexpr parameterize(Params... args) : parameters{tuple{args...}} {}

  template <structural_tuple::tuple_t... Us>
  constexpr parameterize(Us... args) : parameters{structural_tuple::tuple_cat(args...)} {}
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
    static constexpr auto TupleT = ^^structural_tuple::tuple_cat_t<Ts...>;
    return typename[:std::meta::substitute(
                         ^^parameterize_template,
                         std::meta::template_arguments_of(std::meta::dealias(TupleT))):]{};
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
  constexpr parameterize_dual(Ts... args) : sets{structural_tuple::tuple_cat(args...)} {}
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

  bool has_requires_os = false;
  std::meta::info requires_os_annotation = ^^int;
  bool has_disallows_os = false;
  std::meta::info disallows_os_annotation = ^^int;

  std::meta::info parameterize_annotation = ^^int;
  parameterization_types parameterization_type = parameterization_types::None;

  const char* name;

  bool disabled = false;
  int num_parameterizations = 0;
  bool parameterized = false;
};

struct suite_test {
  std::meta::info suite;

  bool has_before_each = false;
  std::meta::info before_each = ^^int;

  bool has_after_each = false;
  std::meta::info after_each = ^^int;
};
}  // namespace annotest