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

struct [[= tag<"basket">{}]] BasketTests {
  Basket basket;

  [[= before_each{}]] void prepare_basket() {
    basket.add(100);
    basket.add(250);
    record_property("currency", "minor-units");
  }

  [[= test{}]] void has_initial_items() {
    assert_that(basket.prices, elements_are(eq(100), eq(250)));
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
  [[= test{}]] void inspect_fatal_assertion() {
    scoped_trace trace{"checking a seeded basket"};
    bool reached_end = false;
    auto captured = capture_failures([&] {
      assert_equal(999, basket.total());
      reached_end = true;
    });
    assert_equal(std::size_t{1}, captured.size());
    assert_equal(failure_types::assertion, captured.front().type);
    assert_that(captured.front().message, has_substr("999"));
    assert_false(reached_end);
  }

  [[= test{}]] void expectations_continue() {
    bool reached_end = false;
    auto captured = capture_failures([&] {
      expect_equal(999, basket.total());
      reached_end = true;
    });
    assert_equal(std::size_t{1}, captured.size());
    assert_equal(failure_types::expectation, captured.front().type);
    assert_true(reached_end);
  }
  [[= test{}]] void attaches_metadata(test_context& context) {
    record_property("basket_total", std::to_string(basket.total()));
    assert_equal(std::string{"attaches_metadata"}, context.name);
    assert_true(std::filesystem::exists(context.temp_dir()));
  }
};

int main(int argc, char** argv) {
  return run_tests<BasketTests>(argc, argv);
}
