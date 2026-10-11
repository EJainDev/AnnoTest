// Dual parameterization couples a template argument set with runtime arguments.
import annotest;
import std;

using namespace annotest;

constexpr auto integers =
    parameterize_pair<template_params<int>, parameters<int, int>>{parameters(7, 14)};
constexpr auto floating =
    parameterize_pair<template_params<double>, parameters<double, double>>{parameters(2.5, 5.0)};

struct DualParameterTests {
  template <typename T = int>
  [[ = test{}, = parameterize_dual{integers, floating} ]] void double_value(T input, T expected) {
    assert_equal(expected, input + input);
  }
};

int main(int argc, char** argv) { return run_tests<DualParameterTests>(argc, argv); }
