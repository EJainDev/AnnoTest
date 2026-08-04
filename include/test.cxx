export module annotest;

export import :tuple;
export import :contracts;
export import :exceptions;
export import :asserts;
export import :expects;
export import :death_test;
export import :posix;
export import :utils;
export import :structs;
export import :logger;

import CLI11;

import std;

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
          result.after_each = m;
          result.has_after_each = true;
        } else if constexpr (t == ^^parameterize || t == ^^parameterize_template ||
                             t == ^^parameterize_dual) {
          current_test.parameterize_annotation = a;
          current_test.parameterized = true;
          using AnnoType = typename[:std::meta::type_of(a):];
          current_test.num_parameterizations = AnnoType::N;
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

bool run_before_each(const auto& func) {
  try {
    func();
  } catch (const error& e) {
  } catch (const std::exception& e) {
    std::println("before_each function failed with exception: {}", e.what());
    return false;
  } catch (...) {
    std::println("before_each function failed with unknown assertion_error");
    return false;
  }
  return true;
}

void run_after_each(const auto& func) {
  try {
    func();
  } catch (const std::exception& e) {
    std::println("after_each function failed with exception: {}", e.what());
  } catch (...) {
    std::println("after_each function failed with unknown assertion_error");
  }
}

template <auto Suite, internal_test Test>
int run_parameterized_test(auto s, int parameterize_target_idx) pre(parameterize_target_idx > -2) {
  int status_code = 0;

  static constexpr auto parameterize_struct =
      std::meta::extract<([:std::meta::substitute(
                                ^^parameterize, std::meta::template_arguments_of(std::meta::type_of(
                                                    Test.parameterize_annotation))):])>(
          Test.parameterize_annotation);

  template for (constexpr auto i : std::make_index_sequence<parameterize_struct.N>{}) {
    static constexpr auto args_tuple = parameterize_struct.parameters.template get<i>();
    static constexpr auto [... args] = args_tuple;

    active_test_instance = active_test{
        .name = std::format("{}.{}({})", std::meta::identifier_of(Suite), Test.name, args_tuple)};

    if (parameterize_target_idx > 1) {
      if (i != parameterize_target_idx) {
        continue;
      }
    }

    if constexpr (Suite.has_before_each) {
      if (!run_before_each([&s]() { s.[:Suite.before_each:](); })) {
        status_code = 1;
        continue;
      }
    }

    try {
      active_test_instance.start_time = std::chrono::steady_clock::now();
      s.[:Test.test:](args...);
      active_test_instance.end_time = std::chrono::steady_clock::now();

      if (contract_violation_occurred) {
        throw assertion_error("Contract violation occurred during test execution");
      }
    } catch (const assertion_error& e) {
      active_test_instance.catch_exception(e);
      status_code = 1;
    } catch (const error& e) {
      active_test_instance.catch_exception(e);
      status_code = 1;
    } catch (const std::exception& e) {
      active_test_instance.catch_exception(e);
      status_code = 1;
    } catch (...) {
      active_test_instance.catch_exception();
      status_code = 1;
    }

    if constexpr (Suite.has_after_each) {
      if (!run_after_each([&s]() { s.[:Suite.after_each:](); })) {
        status_code = 1;
      }
    }

    if (parameterize_target_idx > 1) {
      break;
    }
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_parameterized_template_test(auto s, int parameterize_target_idx)
    pre(parameterize_target_idx > -2) {
  int status_code = 0;

  static constexpr auto template_args_list = std::define_static_array(
      std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation)));
  static constexpr auto N = template_args_list.size();

  template for (constexpr auto i : std::make_index_sequence<N>{}) {
    if (parameterize_target_idx > 1) {
      if (i != parameterize_target_idx) {
        continue;
      }
    }

    if constexpr (Suite.has_before_each) {
      if (!run_before_each([&s]() { s.[:Suite.before_each:](); })) {
        status_code = 1;
        continue;
      }
    }

    static constexpr auto template_args =
        std::define_static_array(std::meta::template_arguments_of(template_args_list[i]));

    try {
      auto start = std::chrono::steady_clock::now();
      s.[:std::meta::substitute(Test.test, template_args):]();
      auto end = std::chrono::steady_clock::now();

      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

      if (contract_violation_occurred) {
        throw assertion_error("Contract violation occurred during test execution");
      }
    } catch (const assertion_error& e) {
      std::println("Failed with assertion_error: {}", e.message());
      status_code = 1;
    } catch (const std::exception& e) {
      std::println("Failed with (uncaught) exception message: {}", e.what());
      status_code = 1;
    } catch (...) {
      std::println("Failed with unknown assertion_error");
      status_code = 1;
    }

    if constexpr (Suite.has_after_each) {
      if (!run_after_each([&s]() { s.[:Suite.after_each:](); })) {
        status_code = 1;
      }
    }

    if (parameterize_target_idx > 1) {
      break;
    }
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_parameterized_dual_test(auto s, int parameterize_target_idx)
    pre(parameterize_target_idx > -2) {
  int status_code = 0;

  static constexpr auto parameterize_struct = std::meta::extract<
      typename[:std::meta::substitute(^^parameterize_dual,
                                      std::meta::template_arguments_of(
                                          std::meta::type_of(Test.parameterize_annotation))):]>(
      Test.parameterize_annotation);

  static constexpr auto template_args_list = std::define_static_array(
      std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation)));
  static constexpr auto idx_sequence = std::make_index_sequence<parameterize_struct.N>{};

  template for (constexpr auto i : idx_sequence) {
    if (parameterize_target_idx > 1) {
      if (i != parameterize_target_idx) {
        continue;
      }
    }

    if constexpr (Suite.has_before_each) {
      if (!run_before_each([&s]() { s.[:Suite.before_each:](); })) {
        status_code = 1;
        continue;
      }
    }

    static constexpr auto template_args = std::define_static_array(std::meta::template_arguments_of(
        std::meta::template_arguments_of(template_args_list[i])[0]));
    static constexpr auto [... args] = parameterize_struct.sets.template get<i>().params.params;

    try {
      auto start = std::chrono::steady_clock::now();
      s.[:std::meta::substitute(Test.raw_test, template_args):](args...);
      auto end = std::chrono::steady_clock::now();

      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

      std::println("Parameterization passed in {:.3f} ms", duration.count() / 1'000'000.0);

      if (contract_violation_occurred) {
        throw assertion_error("Contract violation occurred during test execution");
      }
    } catch (const assertion_error& e) {
      std::println("Failed with assertion_error: {}", e.message());
      status_code = 1;
    } catch (const std::exception& e) {
      std::println("Failed with (uncaught) exception message: {}", e.what());
      status_code = 1;
    } catch (...) {
      std::println("Failed with unknown assertion_error");
      status_code = 1;
    }

    if constexpr (Suite.has_after_each) {
      if (!run_after_each([&s]() { s.[:Suite.after_each:](); })) {
        status_code = 1;
      }
    }

    if (parameterize_target_idx > 1) {
      break;
    }
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_regular_test(auto s) {
  int status_code = 0;

  static constexpr auto name = Test.name;

  std::println("Running test: {}", name);
  if constexpr (Suite.has_before_each) {
    if (!run_before_each([&s]() { s.[:Suite.before_each:](); })) {
      return 1;
    }
  }

  try {
    auto start = std::chrono::steady_clock::now();
    s.[:Test.test:]();
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    if (contract_violation_occurred) {
      throw assertion_error("Contract violation occurred during test execution");
    }

    std::println("Passed in {:.3f} ms", duration.count() / 1'000'000.0);
  } catch (const assertion_error& e) {
    std::println("Failed with assertion_error: {}", e.message());
    status_code = 1;
  } catch (const std::exception& e) {
    std::println("Failed with (uncaught) exception message: {}", e.what());
    status_code = 1;
  } catch (...) {
    std::println("Failed with unknown assertion_error");
    status_code = 1;
  }

  if constexpr (Suite.has_after_each) {
    if (!run_after_each([&s]() { s.[:Suite.after_each:](); })) {
      status_code = 1;
    }
  }

  return status_code;
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

    // Name matching
    int parameterize_target_idx = -1;
    if constexpr (!test.parameterized) {
      if (test_names.size() > 0) {
        auto pos = std::ranges::find(test_names, prefix + test.name);
        if (pos != test_names.end()) {
          continue;
        } else {
          test_names.erase(pos);
        }
      }
    } else {
      if (test_names.size() > 0) {
        static constexpr auto name = test.name;
        auto pos = std::ranges::find_if(test_names, [&](const auto& test_name) {
          return test_name.substr(0, test_name.find_last_of('.')) == (prefix + name);
        });
        if (pos == test_names.end()) {
          continue;
        } else {
          parameterize_target_idx =
              (*pos)[pos->size() - 1] - '0' - 1;  // Convert last character to index
          test_names.erase(pos);

          if (pos == test_names.end() - 1) {
            std::println(
                "Warning, no parameterization index specified for test {}, running all "
                "parameterizations",
                name);
            parameterize_target_idx = -1;
          }
        }
      }
    }

    // Skip disabled tests
    if constexpr (test.disabled) {
      std::println("Skipping disabled test: {}", test.name);
      continue;
    }

    // A catch-all flag representing whether to stop the test early for OS requirements
    bool stop = false;

    // Check for required operating systems
    if constexpr (test.has_requires_os) {
      static constexpr auto required_os_list = std::meta::extract<
          typename[:std::meta::substitute(^^requires_os,
                                          std::meta::template_arguments_of(
                                              std::meta::type_of(test.requires_os_annotation))):]>(
          test.requires_os_annotation);
      template for (constexpr auto required_os : required_os_list.os) {
        if (required_os != os) {
          std::println("Current OS {} does not match required OS {}, skipping test {}",
                       enum_to_string(required_os), enum_to_string(os), test.name);
          stop = true;
          break;
        }
      }
    }

    if (stop) {
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
      continue;
    }

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
    } else if constexpr (not_has_required_parameter<test>()) {
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

  if (test_names.size() > 0) {
    std::println(std::cerr, "Warning: The following test names were not found: {}", test_names);
  }

  return status_code;
}

}  // namespace annotest
