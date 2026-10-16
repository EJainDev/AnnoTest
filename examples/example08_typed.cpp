// Template parameterization runs one method specialization for each type set.
import annotest;
import std;

using namespace annotest;

using type_pairs = template_params<int>::cross_t<template_params<char, double>>;
constexpr auto pair_cases = parameterize_template<>::of<type_pairs>();

struct TypedTests {
  // Default template arguments allow reflection to inspect the annotated method.
  template <typename T = int>
  [[ = test{}, = parameterize_template<template_params<int>, template_params<double>>{} ]] void
      vector_round_trip() {
    std::vector<T> values;
    values.push_back(T{3});
    values.push_back(T{7});
    assert_equal(T{7}, values.back());
    values.pop_back();
    assert_equal(T{3}, values.front());
  }

  template <typename First = int, typename Second = char>
  [[ = test{}, = pair_cases ]] void pairs() {
    const std::pair<First, Second> value{First{3}, Second{7}};
    assert_equal(First{3}, value.first);
    assert_equal(Second{7}, value.second);
  }
};

int main(int argc, char** argv) { return run_tests<TypedTests>(argc, argv); }
