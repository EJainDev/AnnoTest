import cpputils.data_structures;
import std;

using namespace cpputils::data_structures;

namespace {
struct BenchResult {
  std::string name;
  double best_ms;
  double ns_per_element;
  double million_elements_per_second;
  double checksum;
};

int parse_int_arg(int argc, char** argv, std::string_view name, int default_value) {
  const std::string prefix = std::string(name) + '=';
  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg.starts_with(prefix)) {
      return std::stoi(std::string(arg.substr(prefix.size())));
    }
  }
  return default_value;
}

template <typename Fn>
BenchResult run_benchmark(std::string name, std::size_t elements, int passes, int trials, Fn&& fn) {
  constexpr int warmup_trials = 2;
  double best_ms = std::numeric_limits<double>::max();
  double checksum = 0;

  for (int trial = 0; trial < warmup_trials + trials; ++trial) {
    const auto start = std::chrono::steady_clock::now();
    checksum = fn();
    const auto end = std::chrono::steady_clock::now();

    if (trial >= warmup_trials) {
      const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
      best_ms = std::min(best_ms, elapsed_ms);
    }
  }

  const double total_elements = static_cast<double>(elements) * static_cast<double>(passes);
  const double ns_per_element = (best_ms * 1'000'000.0) / total_elements;
  const double million_elements_per_second = total_elements / best_ms / 1'000.0;

  return {std::move(name), best_ms, ns_per_element, million_elements_per_second, checksum};
}

vector<int> build_int_input(std::size_t elements) {
  vector<int> values(static_cast<int>(elements));
  for (std::size_t index = 0; index < elements; ++index) {
    values[static_cast<int>(index)] = static_cast<int>((index * 17u + 23u) % 1000u);
  }
  return values;
}

vector<float> build_float_input(std::size_t elements) {
  vector<float> values(static_cast<int>(elements));
  for (std::size_t index = 0; index < elements; ++index) {
    values[static_cast<int>(index)] = static_cast<float>((index * 17u + 23u) % 1000u);
  }
  return values;
}

void print_result(const BenchResult& result) {
  std::cout << std::left << std::setw(16) << result.name << std::right << std::setw(12)
            << std::fixed << std::setprecision(3) << result.best_ms << " ms  " << std::setw(10)
            << std::setprecision(2) << result.ns_per_element << " ns/elem  " << std::setw(10)
            << result.million_elements_per_second << " Melem/s  checksum=" << result.checksum
            << '\n';
}
}  // namespace

int main(int argc, char** argv) {
  const int element_count = parse_int_arg(argc, argv, "--size", 1 << 22);
  const int passes = parse_int_arg(argc, argv, "--passes", 50);
  const int trials = parse_int_arg(argc, argv, "--trials", 7);

  if (element_count <= 0 || passes <= 0 || trials <= 0) {
    std::cerr << "Expected positive values for --size, --passes, and --trials\n";
    return 1;
  }

  const vector<int> int_input = build_int_input(static_cast<std::size_t>(element_count));
  const vector<float> float_input = build_float_input(static_cast<std::size_t>(element_count));

  std::cout << "Benchmarking stream API vs naive loops. Elements=" << element_count
            << ", passes=" << passes << ", trials=" << trials << ".\n";

  const BenchResult stream_int = run_benchmark("stream int", int_input.size(), passes, trials, [&] {
    double checksum = 0;
    for (int pass = 0; pass < passes; ++pass) {
      VectorFromStream<int> stream_result_data;
      int_input.stream()
          .filter([](const int& value) { return (value & 1) == 0; })
          .map([](const int& value) { return value + 1; })
          .map([](const int& value) { return value * 2; })
          .collect<VectorFromStream<int>>(stream_result_data)();
      for (const auto& val : *stream_result_data.data) {
        checksum += val;
      }
    }
    return checksum;
  });

  const BenchResult naive_int = run_benchmark("naive int", int_input.size(), passes, trials, [&] {
    double checksum = 0;
    for (int pass = 0; pass < passes; ++pass) {
      vector<int> out;
      out.reserve(int_input.size());
      for (int idx = 0; idx < int_input.size(); ++idx) {
        const int value = int_input[idx];
        if ((value & 1) == 0) {
          out.add((value + 1) * 2);
        }
      }
      for (const auto& val : out) {
        checksum += val;
      }
    }
    return checksum;
  });

  const BenchResult stream_float =
      run_benchmark("stream float", float_input.size(), passes, trials, [&] {
        double checksum = 0;
        for (int pass = 0; pass < passes; ++pass) {
          VectorFromStream<float> stream_result_data;
          float_input.stream()
              .filter([](const float& value) { return value * 2 > 10.0f; })
              .map([](const float& value) { return value + 1.5f; })
              .map([](const float& value) { return value * 2.0f; })
              .collect<VectorFromStream<float>>(stream_result_data)();
          for (const auto& val : *stream_result_data.data) {
            checksum += val;
          }
        }
        return checksum;
      });

  const BenchResult naive_float =
      run_benchmark("naive float", float_input.size(), passes, trials, [&] {
        double checksum = 0;
        for (int pass = 0; pass < passes; ++pass) {
          vector<float> out;
          out.reserve(float_input.size());
          for (int idx = 0; idx < float_input.size(); ++idx) {
            const float value = float_input[idx];
            if (value * 2.0f > 10.0f) {
              out.add((value + 1.5f) * 2.0f);
            }
          }
          for (const auto& val : out) {
            checksum += val;
          }
        }
        return checksum;
      });

  print_result(stream_int);
  print_result(naive_int);
  print_result(stream_float);
  print_result(naive_float);

  if (stream_int.checksum != naive_int.checksum || stream_float.checksum != naive_float.checksum) {
    std::cerr << "Checksum mismatch: stream int=" << stream_int.checksum
              << ", naive int=" << naive_int.checksum << ", stream float=" << stream_float.checksum
              << ", naive float=" << naive_float.checksum << '\n';
    return 2;
  }

  const double int_ratio = naive_int.best_ms / stream_int.best_ms;
  const double float_ratio = naive_float.best_ms / stream_float.best_ms;
  std::cout << "Speed ratio (naive / stream): int=" << std::fixed << std::setprecision(2)
            << int_ratio << "x, float=" << float_ratio << "x\n";
  return 0;
}