import annotest;
import std;
using namespace annotest;

struct[[ = suite<"resources">{}, = tag<"filesystem">{} ]] resource_tests {
  inline static std::shared_ptr<std::filesystem::path> directory;
  int fresh_value = 0;
  [[= before_all{}]] static void start(test_context& context) {
    directory = context.resources.get_or_create<std::filesystem::path>(
        "directory", [&] { return std::make_shared<std::filesystem::path>(context.temp_dir()); });
    std::ofstream(*directory / "input.txt") << "hello";
  }
  [[= after_all{}]] static void finish() {
    assert_true(std::filesystem::exists(*directory / "input.txt"));
    directory.reset();  // Context cleanup releases resources and removes the directory.
  }
  [[= before_each{}]] void reset() { assert_equal(0, fresh_value++); }
  [[= test{.repetitions = 2}]] void fresh_fixture() {
    assert_equal(1, fresh_value);
    assert_true(std::filesystem::exists(*directory / "input.txt"));
    record_property("input", "input.txt");
  }
  struct[[ = nested{}, = suite<"reader">{} ]] reader_tests {
    resource_tests& parent;
    explicit reader_tests(resource_tests& value) : parent(value) {}
    [[= before_all{}]] static void start() { assert_true(directory != nullptr); }
    [[= after_all{}]] static void finish() { assert_true(directory != nullptr); }
    [[= test{}]] void read() {
      assert_equal(1, parent.fresh_value);
      auto lines = line_cases(*directory / "input.txt");
      assert_equal(1u, lines.size());
      assert_equal("hello", std::get<0>(lines.front().values));
    }
  };
};
int main(int argc, char** argv) { return run_tests<resource_tests>(argc, argv); }
