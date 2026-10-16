import annotest;
import std;
using namespace annotest;

struct Basket {
  std::vector<int> prices;

  void add(int cents) { prices.push_back(cents); }
  int total() const {
    return std::accumulate(prices.begin(), prices.end(), 0);
  }
};

struct BasketTests {
  Basket basket;

  [[= before_each{}]] void prepare_basket() {
    basket.add(100);
    basket.add(250);
  }

  [[= test{}]] void has_initial_items() {
    assert_equal(std::size_t{2}, basket.prices.size());
    assert_equal(350, basket.total());
  }

  [[= test{}]] void adding_an_item() {
    basket.add(50);
    assert_equal(std::size_t{3}, basket.prices.size());
    assert_equal(400, basket.total());
  }
};

int main(int argc, char** argv) {
  return run_tests<BasketTests>(argc, argv);
}
