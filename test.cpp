import cpputils.data_structures;

import std;

using namespace cpputils::data_structures;

int main() {
  vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  VectorFromStream<int> result;
  vec.stream()
      .filter([](const int& val) { return val % 2 == 0; })
      .map([](const int& val) { return val + 1; })
      .map([](const int& val) { return val * 2; })
      .collect<VectorFromStream<int>>(result)();
  for (const auto& val : *result.data) {
    std::cout << val << " ";
  }
  std::cout << '\n';

  return 0;
}