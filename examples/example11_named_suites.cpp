import annotest;
import std;
using namespace annotest;
constexpr std::string suite_label(std::string_view name) {
  return "Arithmetic " + readable_name(name);
}
struct[[
  = parameterize{parameters(2, 3), parameters(10, 20)}, = name_generator<&readable_name>{},
  = suite_name_generator<&suite_label>{}
]] Addition_cases {
  int first, second;
  Addition_cases(int first, int second) : first(first), second(second) {}
  [[= test{}]] void sum_is_positive() {
    assert_greater(first + second, 0, "Positive inputs have a positive sum");
  }
  [[= test{}]] void addition_is_commutative() { assert_equal(first + second, second + first); }
};
template <typename T = int>
struct[[= parameterize_template<template_params<int>, template_params<double>>{}]] ContainerCases {
  [[= test{}]] void stores_a_value() { assert_that(std::vector<T>{T{1}}, elements_are(eq(T{1}))); }
};
int main(int argc, char** argv) { return run_tests<Addition_cases, ContainerCases<>>(argc, argv); }
