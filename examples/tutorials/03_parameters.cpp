import annotest;
import std;
using namespace annotest;

struct Basket {
  std::vector<int> prices;

  void add(int cents) { prices.push_back(cents); }
  int total() const {
    return std::accumulate(prices.begin(), prices.end(), 0);
  }

  int discounted_total(int percent) const {
    return total() * (100 - percent) / 100;
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
  [[= test{}, = parameterize{
      parameters(0, 350), parameters(10, 315), parameters(20, 280)}]]
  void discounts(int percent, int expected) {
    assert_equal(expected, basket.discounted_total(percent));
  }
};

int main(int argc, char** argv) {
  return run_tests<BasketTests>(argc, argv);
}
