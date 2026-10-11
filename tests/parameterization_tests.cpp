import annotest;
import std;
import structural_tuple;

using namespace annotest;

constexpr auto value_product = parameters(2, 3).cross(parameters(5, 7));
static_assert(value_product.get<0>().params.get<0>() == 2);
static_assert(value_product.get<0>().params.get<1>() == 3);
static_assert(value_product.get<0>().params.get<2>() == 5);
static_assert(value_product.get<1>().params.get<2>() == 7);
constexpr auto empty_product = parameters(1).cross(parameters());
static_assert(std::same_as<decltype(empty_product), const structural_tuple::tuple<>>);
using type_product = template_params<int>::cross_t<template_params<char, double>>;
static_assert(std::same_as<type_product, structural_tuple::tuple<template_params<int, char>,
                                                                 template_params<int, double>>>);
constexpr auto dual_int = parameterize_pair<template_params<int>, parameters<int>>{parameters(7)};
constexpr auto dual_double =
    parameterize_pair<template_params<double>, parameters<double>>{parameters(2.5)};
constexpr auto dual_tuple = structural_tuple::tuple{dual_int, dual_double};
constexpr auto more_duals = structural_tuple::tuple{dual_double, dual_int};
constexpr auto more_values = structural_tuple::tuple{parameters(10), parameters(11)};
constexpr auto values_tuple = structural_tuple::tuple{parameters(8), parameters(9)};
constexpr auto single_value = structural_tuple::tuple{parameters(12)};
constexpr auto empty_values = structural_tuple::tuple{};
constexpr auto template_sets = parameterize_template<>::of<type_product>();
static_assert(decltype(template_sets)::N == 2);
static_assert(decltype(parameterize(values_tuple))::N == 2);
static_assert(decltype(parameterize_dual(dual_tuple))::N == 2);

constexpr auto empty_base = parameters().cross(parameters(4, 5));
static_assert(empty_base.get<0>().params.get<0>() == 4);
static_assert(empty_base.get<1>().params.get<0>() == 5);
using no_types = template_params<int>::cross_t<template_params<>>;
static_assert(std::same_as<no_types, structural_tuple::tuple<>>);
static_assert(decltype(parameterize_template<>::of<no_types>())::N == 0);
static_assert(decltype(parameterize_template<>::of<type_product, type_product>())::N == 4);

struct ParameterizationTests {
  [[
    = test{},
    = parameterize<parameters<int>, parameters<int>, parameters<int>>{values_tuple, single_value}
  ]] void unequal_value_tuples(int n) {
    std::println("unequal {}", n);
  }

  [[ = test{}, = parameterize<parameters<int>>{empty_values, single_value,
                                               empty_values} ]] void empty_value_tuples(int n) {
    assert_equal(12, n);
  }

  [[
    = test{},
    = parameterize<parameters<int>, parameters<int>, parameters<int>, parameters<int>>{values_tuple,
                                                                                       more_values}
  ]] void multiple_value_tuples(int n) {
    assert_true(n >= 8 && n <= 11);
    std::println("value tuple {}", n);
  }

  template <typename T = int>
  [[
    = test{}, = parameterize_dual<std::remove_cv_t<decltype(dual_int)>,
                                  std::remove_cv_t<decltype(dual_double)>,
                                  std::remove_cv_t<decltype(dual_double)>,
                                  std::remove_cv_t<decltype(dual_int)>>{dual_tuple, more_duals}
  ]] void multiple_dual_tuples(T n) {
    std::println("dual tuple {}", n);
    assert_true(n > 0);
  }

  template <typename T = int>
  [[ = test{}, = parameterize_template<>{} ]] void empty_types() {
    assert_true(false);
  }

  template <typename T = int>
  [[ = test{}, = parameterize_dual{} ]] void empty_dual(T) {
    assert_true(false);
  }

  int invocation = 0;
  [[= before_each{}]] void setup() { std::println("setup {}", ++invocation); }
  [[= after_each{}]] void cleanup() { std::println("cleanup {}", invocation); }

  [[ = test{}, = parameterize{parameters(1), parameters(2), parameters(3)} ]] void values(int n) {
    std::println("value {}", n);
    assert_true(n >= 1 && n <= 3);
  }

  [[
    = test{}, = parameterize{parameters(1, 2, 3), parameters(-3, 2, -1)}
  ]] void multiple_arguments(int a, int b, int sum) {
    assert_equal(sum, a + b);
  }

  [[ = test{}, = parameterize(value_product) ]] void cross_values(int a, int b, int c) {
    assert_equal(5, a + b);
    std::println("cross {}", c);
  }

  [[ = test{}, = parameterize(values_tuple) ]] void tuple_values(int n) {
    assert_true(n == 8 || n == 9);
    std::println("tuple {}", n);
  }

  [[ = test{}, = parameterize{parameters(), parameters()} ]] void no_arguments() {
    std::println("empty arguments");
  }

  [[ = test{}, = parameterize{} ]] void empty() { assert_true(false); }

  template <typename T = int>
  [[ = test{}, = parameterize_template<template_params<int>, template_params<double>>{} ]] void
      types() {
    assert_true((std::same_as<T, int> || std::same_as<T, double>));
    std::println("type {}", std::same_as<T, int> ? "int" : "double");
  }

  template <typename A = int, typename B = char>
  [[ = test{}, = template_sets ]] void cross_types() {
    assert_true((std::same_as<A, int>));
    std::println("cross type {}", std::same_as<B, char> ? "char" : "double");
  }

  template <typename T = int>
  [[ = test{}, = parameterize_dual{dual_int, dual_double} ]] void dual(T n) {
    if constexpr (std::same_as<T, int>)
      assert_equal(7, n);
    else
      assert_equal(2.5, n);
    std::println("dual {}", n);
  }

  template <typename T = int>
  [[ = test{}, = parameterize_dual(dual_tuple) ]] void tuple_dual(T n) {
    std::println("tuple dual {}", n);
    assert_true(n > 0);
  }

  [[ = test<"named values">{}, = parameterize{parameters(4), parameters(5)} ]] void renamed(int n) {
    std::println("named {}", n);
  }

  [[ = test{.disabled = true}, = parameterize{parameters(1), parameters(2)} ]] void disabled(int) {
    assert_true(false);
  }

  [[ = test{},
     = parameterize{parameters(1), parameters(2), parameters(3)} ]] void failure_continues(int n) {
    assert_not_equal(2, n);
    std::println("value {}", n);
  }
};

int main(int argc, char** argv) { return run_tests<ParameterizationTests>(argc, argv); }
