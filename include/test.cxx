module;

#include <unistd.h>

#include <cstdio>

export module annotest;

export import :contracts;
export import :exceptions;
export import :asserts;
export import :expects;
export import :death_test;
export import :posix;
export import :utils;
export import :structs;
export import :logger;

import std;

import CLI11;
import structural_tuple;

template <typename... Ts>
using tuple = structural_tuple::tuple<Ts...>;

namespace annotest {
// Helper function to call a test with all the parameters in a tuple
template <std::size_t N, typename F>
constexpr auto with_indices(const F f) -> decltype(auto) {
  return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) {
    return f(std::integral_constant<std::size_t, Is>{}...);
  }(std::make_index_sequence<N>{});
}

// Gets all required information from the testing class
template <std::meta::info Suite>
consteval auto get_tests() {
  static constexpr auto members = std::define_static_array(
      std::meta::members_of(Suite, std::meta::access_context::unprivileged()));
  static constexpr auto size = members.size();

  suite_test result;
  result.suite = Suite;

  std::vector<internal_test> children;

  std::string test_list = std::string(std::meta::identifier_of(Suite)) + ".\n";
  template for (constexpr auto m : members) {
    bool test_found = false;
    bool lifecycle_found = false;

    // Guard against auto-generated constructors and destructors
    if constexpr (std::meta::has_identifier(m) &&
                  (std::meta::is_function(m) || std::meta::is_template(m))) {
      static constexpr auto member =
          (std::meta::is_function(m)
               ? m
               : std::meta::substitute(m,
                                       std::define_static_array(std::vector<std::meta::info>{})));
      static constexpr auto annotations =
          std::define_static_array(std::meta::annotations_of(member));

      internal_test current_test{};
      current_test.test = member;
      current_test.raw_test = m;
      current_test.parameterized = false;
      current_test.num_parameterizations = 0;

      template for (constexpr auto a : annotations) {
        constexpr auto t = std::meta::template_of(std::meta::type_of(a));

        if constexpr (t == ^^test) {
          if (lifecycle_found) {
            throw std::logic_error(
                std::format("Cannot have a `test` annotation on a lifecycle "
                            "(before_each or after_each) method. Method name: {}",
                            std::meta::identifier_of(m)));
          }

          static constexpr auto template_args =
              std::define_static_array(std::meta::template_arguments_of(std::meta::type_of(a)));

          auto test_info =
              std::meta::extract<typename[:std::meta::substitute(^^test, template_args):]>(a);
          current_test.disabled = test_info.disabled;

          std::string str_test_name;
          if (!test_info.is_empty()) {
            str_test_name = test_info.name;
          }
          current_test.name = !test_info.is_empty()
                                  ? std::define_static_string(str_test_name)
                                  : std::define_static_string(std::meta::identifier_of(m));
          test_found = true;
        } else if constexpr (t == ^^before_each) {
          if (result.has_before_each) {
            throw std::logic_error(std::format(
                "Duplicate before_each annotation detected. First one at {} and second one at {}",
                std::meta::identifier_of(result.before_each), std::meta::identifier_of(m)));
          } else if (test_found) {
            throw std::logic_error(
                std::format("before_each cannot be declared on a `test` method or `suite` class. "
                            "Test/Suite name: {}",
                            std::meta::identifier_of(m)));
          }

          lifecycle_found = true;
          result.before_each = m;
          result.has_before_each = true;
        } else if constexpr (t == ^^after_each) {
          if (result.has_after_each) {
            throw std::logic_error(std::format(
                "Duplicate after_each annotation detected. First one at {} and second one at {}",
                std::meta::identifier_of(result.after_each), std::meta::identifier_of(m)));
          } else if (test_found) {
            throw std::logic_error(
                std::format("after_each cannot be declared on a `test` method. Method name: {}",
                            std::meta::identifier_of(m)));
          }
          lifecycle_found = true;
          result.after_each = m;
          result.has_after_each = true;
        } else if constexpr (t == ^^parameterize || t == ^^parameterize_template ||
                             t == ^^parameterize_dual) {
          current_test.parameterization_type =
              t == ^^parameterize ? parameterization_types::Value
                                  : (t == ^^parameterize_template ? parameterization_types::Template
                                                                  : parameterization_types::Dual);
          current_test.parameterize_annotation = a;
          current_test.parameterized = true;
          using AnnoType = typename[:std::meta::type_of(a):];
          current_test.num_parameterizations = AnnoType::N;
        } else if constexpr (t == ^^requires_os) {
          current_test.has_requires_os = true;
          current_test.requires_os_annotation = a;
        } else if constexpr (t == ^^disallows_os) {
          current_test.has_disallows_os = true;
          current_test.disallows_os_annotation = a;
        }
      }

      if (test_found) {
        if (current_test.num_parameterizations > 0) {
          for (int i = 0; i < current_test.num_parameterizations; ++i) {
            test_list = test_list + "\n\t" + current_test.name + "." + to_string(i + 1);
          }
        } else {
          test_list = test_list + "\n\t" + current_test.name;
        }

        children.emplace_back(current_test);
      }
    }
  }

  return tuple(result, std::define_static_array(children), std::define_static_string(test_list));
}

template <std::meta::info func>
consteval bool not_has_required_parameter() {
  static constexpr auto args = std::define_static_array(std::meta::parameters_of(func));
  template for (constexpr auto arg : args) {
    if constexpr (!std::meta::has_default_argument(arg)) {
      return false;
    }
  }

  return true;
}

// Temporary files let large writes complete without full pipes or background reader threads.
class std_streams {
 public:
  std_streams() {
    std::cout.flush();
    std::cerr.flush();
    std::fflush(nullptr);
    out = std::tmpfile();
    err = std::tmpfile();
    saved_out = dup(STDOUT_FILENO);
    saved_err = dup(STDERR_FILENO);
    if (!out || !err || saved_out < 0 || saved_err < 0) {
      restore();
      throw std::runtime_error("Cannot capture test output");
    }
    if (dup2(fileno(out), STDOUT_FILENO) < 0 || dup2(fileno(err), STDERR_FILENO) < 0) {
      restore();
      throw std::runtime_error("Cannot redirect test output");
    }
  }
  std_streams(const std_streams&) = delete;
  std_streams& operator=(const std_streams&) = delete;
  ~std_streams() {
    std::cout.flush();
    std::cerr.flush();
    std::fflush(nullptr);
    if (out) suite_log.testcases.back().sysout = read_file(out);
    if (err) suite_log.testcases.back().syserr = read_file(err);
    restore();
  }

 private:
  static std::string read_file(std::FILE* file) {
    std::rewind(file);
    std::string result;
    char buffer[4096];
    while (auto n = std::fread(buffer, 1, sizeof(buffer), file)) result.append(buffer, n);
    return result;
  }
  void restore() {
    if (saved_out >= 0) {
      dup2(saved_out, STDOUT_FILENO);
      close(saved_out);
      saved_out = -1;
    }
    if (saved_err >= 0) {
      dup2(saved_err, STDERR_FILENO);
      close(saved_err);
      saved_err = -1;
    }
    if (out) {
      std::fclose(out);
      out = nullptr;
    }
    if (err) {
      std::fclose(err);
      err = nullptr;
    }
  }
  std::FILE* out = nullptr;
  std::FILE* err = nullptr;
  int saved_out = -1;
  int saved_err = -1;
};

void record_exception(const std::string& phase) {
  try {
    throw;
  } catch (const assertion_error& e) {
    // Assertion helpers have already recorded their failure; manually thrown errors have not.
    if (!e.recorded()) {
      suite_log.add_failure({failure_types::assertion, e.message(), {.message = e.message()}});
    }
  } catch (const std::exception& e) {
    auto msg = std::format("{}: {}", phase, e.what());
    suite_log.add_failure({failure_types::uncaught_exception, msg, {.message = msg}});
  } catch (...) {
    auto msg = phase + ": unknown exception";
    suite_log.add_failure({failure_types::unknown, msg, {.message = msg}});
  }
}

template <auto Suite, auto Test, auto... Parameters>
int run_test(auto& s, const std::string_view name) {
  contract_violation_occurred = false;
  suite_log.testcases.emplace_back(
      test_xml_log{.test_class = suite_log.name, .name = std::string(name)});
  auto start = std::chrono::steady_clock::now();
  {
    std_streams streams;
    bool ready = true;
    if constexpr (Suite.has_before_each) {
      try {
        s.[:Suite.before_each:]();
      } catch (...) {
        record_exception("before_each");
        ready = false;
      }
    }
    if (ready) {
      try {
        s.[:Test:](Parameters...);
      } catch (...) {
        record_exception("test");
      }
    }
    if constexpr (Suite.has_after_each) {
      try {
        s.[:Suite.after_each:]();
      } catch (...) {
        record_exception("after_each");
      }
    }
  }
  if (std::exchange(contract_violation_occurred, false)) {
    suite_log.add_failure({failure_types::contract_violation,
                           "Unexpected contract violation",
                           {.message = "Unexpected contract violation"}});
  }
  auto& result = suite_log.testcases.back();
  result.time = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  for (const auto& failure : result.failures) {
    if (failure.type == failure_types::uncaught_exception ||
        failure.type == failure_types::unknown || failure.type == failure_types::contract_violation)
      ++result.errors;
  }
  result.failed = !result.failures.empty();
  suite_log.failed += result.failed;
  suite_log.errors += result.errors;
  std::println("{}: {}", name, result.failed ? "Failed" : "Passed");
  for (const auto& failure : result.failures) std::println(std::cerr, "{}", failure.cdata);
  return result.failed ? 1 : 0;
}

template <auto test_name, auto i>
consteval auto get_parameterized_test_name() {
  return std::define_static_string(std::string(test_name) + "." + to_string(i + 1));
}

template <auto Suite, internal_test Test>
int run_parameterized_test(auto& s, int parameterize_target_idx) pre(parameterize_target_idx > -2) {
  int status_code = 0;

  using Annotation = typename[:std::meta::type_of(Test.parameterize_annotation):];
  static constexpr auto parameterize_struct =
      std::meta::extract<Annotation>(Test.parameterize_annotation);
  static constexpr auto indices = std::make_index_sequence<Annotation::N>{};

  template for (constexpr auto i : indices) {
    static constexpr auto args_tuple = parameterize_struct.parameters.template get<i>();
    static constexpr auto [... args] = args_tuple.params;

    static constexpr auto test_name = Test.name;
    static constexpr auto current_test_name = get_parameterized_test_name<test_name, i>();

    if (parameterize_target_idx >= 0) {
      if (i != parameterize_target_idx) {
        continue;
      }
    }

    status_code |= run_test<Suite, Test.test, args...>(s, current_test_name);

    if (parameterize_target_idx >= 0) {
      break;
    }
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_parameterized_template_test(auto& s, int parameterize_target_idx)
    pre(parameterize_target_idx > -2) {
  int status_code = 0;

  static constexpr auto template_args_list = std::define_static_array(
      std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation)));
  using Annotation = typename[:std::meta::type_of(Test.parameterize_annotation):];
  static constexpr auto indices = std::make_index_sequence<Annotation::N>{};

  static constexpr auto name = Test.name;

  template for (constexpr auto i : indices) {
    static constexpr auto template_args =
        std::define_static_array(std::meta::template_arguments_of(template_args_list[i]));

    static constexpr auto test_name = get_parameterized_test_name<name, i>();

    if (parameterize_target_idx >= 0) {
      if (i != parameterize_target_idx) {
        continue;
      }
    }

    static constexpr auto specialization = std::meta::substitute(Test.raw_test, template_args);
    status_code |= run_test<Suite, specialization>(s, test_name);

    if (parameterize_target_idx >= 0) {
      break;
    }
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_parameterized_dual_test(auto& s, int parameterize_target_idx)
    pre(parameterize_target_idx > -2) {
  int status_code = 0;

  using Annotation = typename[:std::meta::type_of(Test.parameterize_annotation):];
  static constexpr auto parameterize_struct =
      std::meta::extract<Annotation>(Test.parameterize_annotation);

  static constexpr auto template_args_list = std::define_static_array(
      std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation)));
  static constexpr auto idx_sequence = std::make_index_sequence<parameterize_struct.N>{};

  template for (constexpr auto i : idx_sequence) {
    static constexpr auto template_args = std::define_static_array(std::meta::template_arguments_of(
        std::meta::template_arguments_of(template_args_list[i])[0]));
    static constexpr auto args_tuple = parameterize_struct.sets.template get<i>().params.params;
    static constexpr auto [... args] = args_tuple;
    static constexpr auto test_name = get_parameterized_test_name<Test.name, i>();

    if (parameterize_target_idx >= 0) {
      if (i != parameterize_target_idx) {
        continue;
      }
    }

    static constexpr auto specialization = std::meta::substitute(Test.raw_test, template_args);
    status_code |= run_test<Suite, specialization, args...>(s, test_name);

    if (parameterize_target_idx >= 0) {
      break;
    }
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_regular_test(auto& s) {
  static constexpr auto name = Test.name;

  return run_test<Suite, Test.test>(s, name);
}

export template <typename T>
  requires std::is_class_v<T>
int run_tests(int argc, char** argv) {
  static constexpr auto temp_result = get_tests<^^T>();
  static constexpr auto Suite = temp_result.template get<0>();
  static constexpr auto Children = temp_result.template get<1>();
  static constexpr auto test_list = temp_result.template get<2>();

  std::string test_name;

  CLI::App app{"AnnoTest Based Tests"};

  bool list_tests = false;

  auto list_opt = app.add_flag("-l, --list", list_tests, "List all available tests");

  std::string xml_output;
  app.add_option("--xml-output", xml_output, "Write a suite XML report to this file");

  std::vector<std::string> test_names;
  app.add_option("-t, --test-name", test_names,
                 "Run a specific test by fully qualified name (can be used multiple times for "
                 "multiple tests)");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  if (list_tests) {
    std::println(test_list);
    return 0;  // Exit after listing tests
  }

  auto remaining_names = test_names;
  suite_log.reset();
  auto suite_start = std::chrono::steady_clock::now();
  suite_log.name = std::string(std::meta::identifier_of(^^T));
  static constexpr auto source_location = std::meta::source_location_of(^^T);
  suite_log.file = source_location.file_name();
  suite_log.total_tests = 0;
  suite_log.timestamp = std::format(
      "{:%FT%TZ}", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));

  if (test_names.size() == 0) {
    std::println("Running tests in suite {}", std::meta::identifier_of(^^T));
  } else {
    std::println("Running specified tests in suite {}", std::meta::identifier_of(^^T));
  }

#if defined(__linux__)
  static constexpr auto os = OS::Linux;
#elif defined(_WIN32)
  static constexpr auto os = OS::Windows;
#elif defined(__APPLE__)
  static constexpr auto os = OS::Mac;
#else
  static constexpr auto os = OS::Unknown;
#endif

  int status_code = 0;

  typename[:Suite.suite:] s;

  std::string prefix = std::string(std::meta::identifier_of(^^T)) + '.';

  template for (constexpr auto test : Children) {
    contract_violation_occurred = false;  // Reset contract flag before each test

    static constexpr bool is_parameterized = test.parameterized;
    static constexpr int parameter_count = test.num_parameterizations;
    std::vector<int> parameterize_targets;
    auto full_name = prefix + test.name;
    if (test_names.empty()) {
      parameterize_targets.push_back(-1);
    } else {
      for (const auto& requested : test_names) {
        if (requested == full_name) {
          parameterize_targets = {-1};
          std::erase(remaining_names, requested);
        } else if (is_parameterized && requested.starts_with(full_name + ".")) {
          auto suffix = std::string_view(requested).substr(full_name.size() + 1);
          int index = 0;
          auto [end, ec] = std::from_chars(suffix.data(), suffix.data() + suffix.size(), index);
          if (ec != std::errc{} || end != suffix.data() + suffix.size() || index < 1 ||
              index > parameter_count) {
            std::println(std::cerr, "Invalid parameterization: {}", requested);
            return 1;
          }
          if (std::ranges::find(parameterize_targets, -1) == parameterize_targets.end() &&
              std::ranges::find(parameterize_targets, index - 1) == parameterize_targets.end()) {
            parameterize_targets.push_back(index - 1);
          }
          std::erase(remaining_names, requested);
        }
      }
      if (parameterize_targets.empty()) continue;
    }

    // Skip disabled tests
    if constexpr (test.disabled) {
      suite_log.skipped += is_parameterized && parameterize_targets == std::vector<int>{-1}
                               ? parameter_count
                               : parameterize_targets.size();
      std::println("Skipping disabled test: {}", test.name);
      continue;
    }

    // A catch-all flag representing whether to stop the test early for OS requirements
    bool stop = false;

    // Check for required operating systems
    if constexpr (test.has_requires_os) {
      stop = true;
      static constexpr auto required_os_list = std::meta::extract<
          typename[:std::meta::substitute(^^requires_os,
                                          std::meta::template_arguments_of(
                                              std::meta::type_of(test.requires_os_annotation))):]>(
          test.requires_os_annotation);
      template for (constexpr auto required_os : required_os_list.os) {
        if (required_os == os) {
          std::println("Current OS {} matches required OS {} for test {}",
                       enum_to_string(required_os), enum_to_string(os), test.name);
          stop = false;
          break;
        }
      }
    }

    if (stop) {
      suite_log.skipped += is_parameterized && parameterize_targets == std::vector<int>{-1}
                               ? parameter_count
                               : parameterize_targets.size();
      continue;
    }

    // Check for disallowed operating systems
    if constexpr (test.has_disallows_os) {
      static constexpr auto disallowed_os_list = std::meta::extract<
          typename[:std::meta::substitute(^^disallows_os,
                                          std::meta::template_arguments_of(
                                              std::meta::type_of(test.disallows_os_annotation))):]>(
          test.disallows_os_annotation);
      template for (constexpr auto disallowed_os : disallowed_os_list.os) {
        if (disallowed_os == os) {
          std::println("Current OS {} is disallowed for test {}, skipping",
                       enum_to_string(disallowed_os), test.name);
          stop = true;
          break;
        }
      }
    }

    if (stop) {
      suite_log.skipped += is_parameterized && parameterize_targets == std::vector<int>{-1}
                               ? parameter_count
                               : parameterize_targets.size();
      continue;
    }

    for (int parameterize_target_idx : parameterize_targets) {
      if constexpr (test.parameterized) {
        if constexpr (test.parameterization_type == parameterization_types::Value) {
          if (run_parameterized_test<Suite, test>(s, parameterize_target_idx) == 1) {
            status_code = 1;
          }
        } else if constexpr (test.parameterization_type == parameterization_types::Template) {
          if (run_parameterized_template_test<Suite, test>(s, parameterize_target_idx) == 1) {
            status_code = 1;
          }
        } else {
          if (run_parameterized_dual_test<Suite, test>(s, parameterize_target_idx) == 1) {
            status_code = 1;
          }
        }
      } else if constexpr (not_has_required_parameter<test.test>()) {
        if (run_regular_test<Suite, test>(s) == 1) {
          status_code = 1;
        }
      } else if constexpr (!test.parameterized) {
        std::println(
            "Warning: test `{}` did not execute because it has required arguments that were not "
            "given via a parameterization annotation.",
            test.name);
      }
    }
  }

  if (!remaining_names.empty()) {
    status_code = 1;
    std::println(std::cerr, "Warning: The following test names were not found: {}",
                 remaining_names);
  }

  suite_log.total_tests = suite_log.testcases.size() + suite_log.skipped;
  suite_log.time =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - suite_start).count();
  if (!xml_output.empty()) {
    try {
      std::ofstream report(xml_output);
      report << xml_report();
      report.flush();
      if (!report) throw std::runtime_error("Cannot write report file");
    } catch (const std::exception& e) {
      std::println(std::cerr, "Cannot write XML report {}: {}", xml_output, e.what());
      return 1;
    }
  }
  return status_code;
}

}  // namespace annotest
