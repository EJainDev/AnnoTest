import annotest;

import std;

using namespace annotest;

struct TupleTests {
  [[= Test{}]] void CreateTuple() {
    auto t = tuple(42, 3.14, "hello");
    expectEqual(t.template get<0>(), 42);
    expectEqual(t.template get<1>(), 3.14);
    expectEqual(t.template get<2>(), "hello");
  }

  [[= Test{}]] void TupleCat() {
    auto t1 = tuple(1, 2);
    auto t2 = tuple(3.14, "world");
    auto t3 = tuple_cat(t1, t2);
    expectEqual(t3.template get<0>(), 1);
    expectEqual(t3.template get<1>(), 2);
    expectEqual(t3.template get<2>(), 3.14);
    expectEqual(t3.template get<3>(), "world");
  }
};

int main(int argc, char** argv) { return test<TupleTests>(argc, argv); }