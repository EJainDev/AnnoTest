export module annotest;

export import :contracts;
export import :diagnostics;
export import :exceptions;
export import :asserts;
export import :expects;
export import :death_test;
export import :posix;
export import :utils;
export import :structs;
export import :logger;

import std;

import structural_tuple;

namespace annotest {
struct suite_discovery {
  suite_test suite;
  std::span<const internal_test> tests;
};

// Gets all required information from the testing class
template <std::meta::info Suite>
consteval auto get_tests() {
  static constexpr auto members = std::define_static_array(
      std::meta::members_of(Suite, std::meta::access_context::unprivileged()));

  suite_test result;
  result.suite = Suite;
  result.name = std::define_static_string(std::meta::identifier_of(Suite));
  template for (constexpr auto a : std::define_static_array(std::meta::annotations_of(Suite))) {
    if constexpr (std::meta::has_template_arguments(std::meta::type_of(a)) &&
                  std::meta::template_of(std::meta::type_of(a)) == ^^suite) {
      constexpr auto info = std::meta::extract<typename[:std::meta::type_of(a):]>(a);
      result.disabled = info.disabled;
      if (!info.is_empty()) result.name = std::define_static_string(std::string_view(info.name));
    }
  }

  std::vector<internal_test> children;
  children.reserve(members.size());

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

          auto test_info = std::meta::extract<typename[:std::meta::type_of(a):]>(a);
          current_test.disabled = result.disabled || test_info.disabled;

          current_test.name = !test_info.is_empty()
                                  ? std::define_static_string(std::string_view(test_info.name))
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
        children.emplace_back(current_test);
      }
    }
  }

  return suite_discovery{result, std::define_static_array(children)};
}

template <std::meta::info func>
consteval bool not_has_required_parameter() {
  for (auto arg : std::meta::parameters_of(func)) {
    if (!std::meta::has_default_argument(arg)) return false;
  }

  return true;
}

using test_callback = void (*)(void*);

int execute_test(void* context, std::string_view name, test_callback test, test_callback before,
                 test_callback after);

template <auto Method, typename T, auto... Parameters>
void invoke_test_method(void* context) {
  static_cast<T*>(context)->[:Method:](Parameters...);
}

template <auto Suite, auto Test, auto... Parameters, typename T>
int run_test(T& s, std::string_view name) {
  test_callback before = nullptr;
  test_callback after = nullptr;
  if constexpr (Suite.has_before_each) {
    before = invoke_test_method<Suite.before_each, T>;
  }
  if constexpr (Suite.has_after_each) {
    after = invoke_test_method<Suite.after_each, T>;
  }
  return execute_test(&s, name, invoke_test_method<Test, T, Parameters...>, before, after);
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

struct runner_options {
  bool list_tests = false;
  bool should_exit = false;
  int exit_code = 0;
  std::string xml_output;
  std::vector<std::string> test_names;
};

runner_options parse_options(int argc, char** argv);

struct discovered_test {
  const char* name = nullptr;
  bool parameterized = false;
  int parameter_count = 0;
  bool disabled = false;
  bool has_required_os = false;
  std::span<const OS> required_os;
  std::span<const OS> disallowed_os;
  test_callback body = nullptr;
  int (*run)(void*, int) = nullptr;
};

void list_tests(std::string_view suite_name, std::span<const discovered_test> tests);

template <auto Suite, internal_test Test, typename T>
int run_discovered_test(void* context, int index) {
  auto& s = *static_cast<T*>(context);
  if constexpr (Test.parameterized) {
    if constexpr (Test.parameterization_type == parameterization_types::Value) {
      return run_parameterized_test<Suite, Test>(s, index);
    } else if constexpr (Test.parameterization_type == parameterization_types::Template) {
      return run_parameterized_template_test<Suite, Test>(s, index);
    } else {
      return run_parameterized_dual_test<Suite, Test>(s, index);
    }
  }
}

template <auto Suite, internal_test Test, typename T>
consteval discovered_test describe_test() {
  discovered_test entry{.name = Test.name,
                        .parameterized = Test.parameterized,
                        .parameter_count = Test.num_parameterizations,
                        .disabled = Test.disabled,
                        .has_required_os = Test.has_requires_os};
  if constexpr (Test.parameterized) {
    entry.run = run_discovered_test<Suite, Test, T>;
  } else if constexpr (not_has_required_parameter<Test.test>()) {
    entry.body = invoke_test_method<Test.test, T>;
  }
  if constexpr (Test.has_requires_os) {
    static constexpr auto annotation =
        std::meta::extract<typename[:std::meta::type_of(Test.requires_os_annotation):]>(
            Test.requires_os_annotation);
    entry.required_os = std::define_static_array(annotation.os);
  }
  if constexpr (Test.has_disallows_os) {
    static constexpr auto annotation =
        std::meta::extract<typename[:std::meta::type_of(Test.disallows_os_annotation):]>(
            Test.disallows_os_annotation);
    entry.disallowed_os = std::define_static_array(annotation.os);
  }
  return entry;
}

std::chrono::steady_clock::time_point begin_suite(std::string_view suite_name, const char* file,
                                                  bool filtered);

int execute_suite(void* context, std::string_view suite_name,
                  std::span<const discovered_test> tests, const runner_options& options,
                  std::vector<std::string>& remaining_names,
                  std::chrono::steady_clock::time_point suite_start, test_callback before,
                  test_callback after);

export template <typename T>
  requires std::is_class_v<T>
int run_tests(int argc, char** argv) {
  static constexpr auto discovery = get_tests<^^T>();
  static constexpr auto Suite = discovery.suite;
  static constexpr auto suite_name = Suite.name;
  static constexpr auto Children = discovery.tests;
  static constexpr auto plan = []() consteval {
    std::array<discovered_test, Children.size()> entries{};
    std::size_t index = 0;
    template for (constexpr auto test : Children) {
      entries[index++] = describe_test<Suite, test, T>();
    }
    return entries;
  }();

  auto options = parse_options(argc, argv);
  if (options.should_exit) return options.exit_code;
  if (options.list_tests) {
    list_tests(suite_name, plan);
    return 0;
  }
  auto remaining_names = options.test_names;
  static constexpr auto source_location = std::meta::source_location_of(^^T);
  auto start = begin_suite(suite_name, source_location.file_name(), !options.test_names.empty());
  typename[:Suite.suite:] s;
  test_callback before = nullptr;
  test_callback after = nullptr;
  if constexpr (Suite.has_before_each) before = invoke_test_method<Suite.before_each, T>;
  if constexpr (Suite.has_after_each) after = invoke_test_method<Suite.after_each, T>;
  return execute_suite(&s, suite_name, plan, options, remaining_names, start, before, after);
}

}  // namespace annotest
