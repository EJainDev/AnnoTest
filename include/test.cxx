export module annotest;

export import :sources;
export import :matchers;
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

// Discovery diagnostics contain only string substitutions. Avoid instantiating
// the general formatting machinery while checking every reflected method.
template <typename... Values>
consteval std::string discovery_format(std::string_view text, const Values&... values) {
  std::array<std::string_view, sizeof...(Values)> arguments{std::string_view(values)...};
  std::string result;
  std::size_t start = 0, argument = 0;
  for (auto position = text.find("{}"); position != std::string_view::npos;
       position = text.find("{}", start)) {
    if (argument == arguments.size()) throw std::logic_error("Invalid discovery diagnostic");
    result += text.substr(start, position - start);
    result += arguments[argument++];
    start = position + 2;
  }
  result += text.substr(start);
  return result;
}
template <std::meta::info Annotation>
consteval void validate_condition() {
  if constexpr (std::meta::has_template_arguments(std::meta::type_of(Annotation))) {
    constexpr auto kind = std::meta::template_of(std::meta::type_of(Annotation));
    if constexpr (kind == ^^enabled_if || kind == ^^disabled_if) {
      using Policy = typename[:std::meta::type_of(Annotation):];
      if constexpr (!std::is_invocable_r_v<bool, decltype(Policy::predicate),
                                           const test_context&> &&
                    !std::is_invocable_r_v<bool, decltype(Policy::predicate)>)
        throw std::logic_error(
            "Execution conditions must return bool and accept no arguments or const test_context&");
    }
  }
}

template <std::meta::info func>
consteval bool not_has_required_parameter() {
  auto params = std::meta::parameters_of(func);
  if (params.size() == 1 && std::meta::type_of(params[0]) == ^^test_context&) return true;
  for (auto arg : params) {
    if (!std::meta::has_default_argument(arg)) return false;
  }

  return true;
}

// Gets all required information from the testing class
template <std::meta::info Suite>
consteval auto get_tests() {
  static constexpr auto members = std::define_static_array(inherited_members(Suite));
  static constexpr auto suite_annotations = std::define_static_array(expanded_annotations(Suite));

  std::vector<std::meta::info> before_hooks, after_hooks, before_all_hooks, after_all_hooks;
  std::vector<const char*> suite_tags;
  suite_test result;
  result.suite = Suite;
  result.name = std::define_static_string(std::meta::has_template_arguments(Suite)
                                              ? std::meta::display_string_of(Suite)
                                              : std::meta::identifier_of(Suite));
  template for (constexpr auto a : suite_annotations) {
    validate_condition<a>();
    if constexpr (std::meta::has_template_arguments(std::meta::type_of(a)) &&
                  std::meta::template_of(std::meta::type_of(a)) == ^^suite) {
      constexpr auto info = std::meta::extract<typename[:std::meta::type_of(a):]>(a);
      result.disabled = info.disabled;
      result.lifecycle = info.lifecycle;
      if (!info.is_empty()) result.name = std::define_static_string(std::string_view(info.name));
    }
    if constexpr (std::meta::has_template_arguments(std::meta::type_of(a)) &&
                  std::meta::template_of(std::meta::type_of(a)) == ^^tag) {
      suite_tags.push_back(std::meta::extract<typename[:std::meta::type_of(a):]>(a).name);
    }
    if constexpr (std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(a):]>, timeout>)
      result.timeout_seconds = std::meta::extract<timeout>(a).seconds;
  }
  template for (constexpr auto a : suite_annotations) {
    if constexpr (std::meta::has_template_arguments(std::meta::type_of(a)) &&
                  std::meta::template_of(std::meta::type_of(a)) == ^^suite_name_generator) {
      using Policy = typename[:std::meta::type_of(a):];
      result.name = std::define_static_string(Policy::generate(std::string_view(result.name)));
    }
  }
  result.tags = std::define_static_array(suite_tags);

  if (std::string_view(result.name).empty() ||
      std::string_view(result.name).find_first_of("\r\n\t") != std::string_view::npos) {
    throw std::logic_error("Suite names must not contain tabs or line breaks");
  }
  std::vector<internal_test> children;
  children.reserve(members.size());

  template for (constexpr auto m : members) {
    bool test_found = false;
    bool lifecycle_found = false;
    bool before_found = false, after_found = false;

    // GCC exposes function-template annotations only through a specialization.
    // Test templates therefore require defaults (as documented); ordinary
    // templates without defaults must not make an otherwise valid suite fail.
    // Guard against auto-generated constructors and destructors.
    if constexpr (std::meta::has_identifier(m) &&
                  (std::meta::is_function(m) ||
                   (std::meta::is_template(m) &&
                    std::meta::can_substitute(
                        m, std::define_static_array(std::vector<std::meta::info>{}))))) {
      static constexpr auto member =
          (std::meta::is_function(m)
               ? m
               : std::meta::substitute(m,
                                       std::define_static_array(std::vector<std::meta::info>{})));
      static constexpr auto annotations = std::define_static_array(expanded_annotations(member));

      std::vector<const char*> test_tags(suite_tags);
      internal_test current_test{};
      current_test.timeout_seconds = result.timeout_seconds;
      current_test.test = member;
      current_test.raw_test = m;
      current_test.parameterized = false;
      current_test.num_parameterizations = 0;

      template for (constexpr auto a : annotations) {
        validate_condition<a>();
        if constexpr (std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(a):]>, timeout>)
          current_test.timeout_seconds = std::meta::extract<timeout>(a).seconds;
        if constexpr (std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(a):]>,
                                     before_all> ||
                      std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(a):]>,
                                     after_all>) {
          if (lifecycle_found || test_found)
            throw std::logic_error("Suite hooks cannot share test or lifecycle annotations");
          if (!std::meta::is_static_member(member))
            throw std::logic_error("Suite hooks must be static methods");
          lifecycle_found = true;
          if constexpr (std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(a):]>,
                                       before_all>)
            before_all_hooks.push_back(member);
          else
            after_all_hooks.push_back(member);
        }
        if constexpr (std::meta::has_template_arguments(std::meta::type_of(a))) {
          constexpr auto t = std::meta::template_of(std::meta::type_of(a));

          if constexpr (t == ^^test) {
            if (lifecycle_found) {
              throw std::logic_error(
                  discovery_format("Cannot have a `test` annotation on a lifecycle "
                              "(before_each or after_each) method. Method name: {}",
                              std::meta::identifier_of(m)));
            }

            auto test_info = std::meta::extract<typename[:std::meta::type_of(a):]>(a);
            current_test.disabled = result.disabled || test_info.disabled;
            if (test_info.repetitions == 0 || test_info.repetitions > 1000000)
              throw std::logic_error("Test repetitions must be between 1 and 1000000");
            current_test.repetitions = test_info.repetitions;

            current_test.name = !test_info.is_empty()
                                    ? std::define_static_string(std::string_view(test_info.name))
                                    : std::define_static_string(std::meta::identifier_of(m));
            test_found = true;
          } else if constexpr (t == ^^before_each) {
            if (lifecycle_found && !before_found && !after_found)
              throw std::logic_error("Suite hooks cannot share test or lifecycle annotations");
            if (before_found) {
              throw std::logic_error(discovery_format(
                  "Duplicate before_each annotation detected. First one at {} and second one at {}",
                  std::meta::identifier_of(result.before_each), std::meta::identifier_of(m)));
            } else if (test_found) {
              throw std::logic_error(
                  discovery_format("before_each cannot be declared on a `test` method or `suite` class. "
                              "Test/Suite name: {}",
                              std::meta::identifier_of(m)));
            }

            lifecycle_found = true;
            before_found = true;
            before_hooks.push_back(member);
            result.before_each = m;
            result.has_before_each = true;
          } else if constexpr (t == ^^after_each) {
            if (lifecycle_found && !before_found && !after_found)
              throw std::logic_error("Suite hooks cannot share test or lifecycle annotations");
            if (after_found) {
              throw std::logic_error(discovery_format(
                  "Duplicate after_each annotation detected. First one at {} and second one at {}",
                  std::meta::identifier_of(result.after_each), std::meta::identifier_of(m)));
            } else if (test_found) {
              throw std::logic_error(
                  discovery_format("after_each cannot be declared on a `test` method. Method name: {}",
                              std::meta::identifier_of(m)));
            }
            lifecycle_found = true;
            after_found = true;
            after_hooks.push_back(member);
            result.after_each = m;
            result.has_after_each = true;
          } else if constexpr (t == ^^parameterize || t == ^^parameterize_template ||
                               t == ^^parameterize_dual) {
            current_test.parameterization_type =
                t == ^^parameterize
                    ? parameterization_types::Value
                    : (t == ^^parameterize_template ? parameterization_types::Template
                                                    : parameterization_types::Dual);
            current_test.parameterize_annotation = a;
            current_test.parameterized = true;
            using AnnoType = typename[:std::meta::type_of(a):];
            current_test.num_parameterizations = AnnoType::N;
          } else if constexpr (t == ^^tag) {
            test_tags.push_back(std::meta::extract<typename[:std::meta::type_of(a):]>(a).name);
          } else if constexpr (t == ^^parameter_source || t == ^^test_factory) {
            current_test.runtime_parameters = t == ^^parameter_source;
            current_test.dynamic = t == ^^test_factory;
            current_test.parameterize_annotation = a;
          } else if constexpr (t == ^^requires_os) {
            current_test.has_requires_os = true;
            current_test.requires_os_annotation = a;
          } else if constexpr (t == ^^disallows_os) {
            current_test.has_disallows_os = true;
            current_test.disallows_os_annotation = a;
          }
        }
      }

      if (test_found) {
        bool own_generator = false;
        template for (constexpr auto a : annotations) {
          if constexpr (std::meta::has_template_arguments(std::meta::type_of(a)) &&
                        std::meta::template_of(std::meta::type_of(a)) == ^^name_generator) {
            using Policy = typename[:std::meta::type_of(a):];
            current_test.name =
                std::define_static_string(Policy::generate(std::string_view(current_test.name)));
            own_generator = true;
          }
        }
        if (!own_generator) {
          template for (constexpr auto a : suite_annotations) {
            if constexpr (std::meta::has_template_arguments(std::meta::type_of(a)) &&
                          std::meta::template_of(std::meta::type_of(a)) == ^^name_generator) {
              using Policy = typename[:std::meta::type_of(a):];
              current_test.name =
                  std::define_static_string(Policy::generate(std::string_view(current_test.name)));
            }
          }
        }
        if (std::string_view(current_test.name).empty())
          throw std::logic_error("Test names must not be empty");
      }
      if (test_found || lifecycle_found) {
        if (!std::meta::is_public(m)) {
          throw std::logic_error(
              discovery_format("Annotated test and lifecycle methods must be public: {}",
                          std::meta::identifier_of(m)));
        }
        if (std::meta::is_deleted(member)) {
          throw std::logic_error(discovery_format("Annotated methods must not be deleted: {}",
                                             std::meta::identifier_of(m)));
        }
        if (lifecycle_found && !not_has_required_parameter<member>()) {
          throw std::logic_error(discovery_format("Lifecycle hooks must not have required arguments: {}",
                                             std::meta::identifier_of(m)));
        }
      }
      if (test_found) {
        if (!current_test.parameterized && !current_test.runtime_parameters &&
            !current_test.dynamic && !not_has_required_parameter<member>()) {
          throw std::logic_error(discovery_format(
              "Test has required arguments without parameterization: {}", current_test.name));
        }
        if (current_test.parameterized && current_test.num_parameterizations == 0) {
          throw std::logic_error(discovery_format(
              "Test parameterization must contain at least one case: {}", current_test.name));
        }
        if (std::string_view(current_test.name).find_first_of("\r\n\t") != std::string_view::npos) {
          throw std::logic_error("Test names must not contain tabs or line breaks");
        }
        for (const auto& existing : children) {
          if (std::string_view(existing.name) == current_test.name) {
            throw std::logic_error(discovery_format("Duplicate test name: {}", current_test.name));
          }
        }
        if (current_test.timeout_seconds < 0 || !std::isfinite(current_test.timeout_seconds))
          throw std::logic_error("Test timeout must be finite and nonnegative");
        current_test.tags = std::define_static_array(test_tags);
        children.emplace_back(current_test);
      }
    }
  }

  // Base names are selection aliases for parameterized tests. Reserve them
  // alongside concrete case names so every filter has exactly one meaning.
  // Two concrete names can coincide only when their bases coincide (already
  // rejected above). Check base aliases against canonical decimal suffixes
  // directly, instead of constructing and comparing every expanded name.
  for (const auto& child : children) {
    if (!child.parameterized) continue;
    const auto base = std::string_view(child.name);
    const auto limit = static_cast<std::size_t>(child.num_parameterizations);
    std::size_t first_collision = limit + 1;
    const char* collision = nullptr;
    for (const auto& alias : children) {
      auto name = std::string_view(alias.name);
      if (!name.starts_with(base) || name.size() <= base.size() + 1 ||
          name[base.size()] != '.') continue;
      auto suffix = name.substr(base.size() + 1);
      if (suffix.front() < '1' || suffix.front() > '9') continue;
      std::size_t index = 0;
      bool valid = true;
      for (char digit : suffix) {
        if (digit < '0' || digit > '9' || index > limit / 10 ||
            index * 10 + (digit - '0') > limit) {
          valid = false;
          break;
        }
        index = index * 10 + (digit - '0');
      }
      if (valid && index < first_collision) {
        first_collision = index;
        collision = alias.name;
      }
    }
    if (collision)
      throw std::logic_error(discovery_format("Duplicate expanded test name: {}", collision));
  }

  result.before_hooks = std::define_static_array(before_hooks);
  result.after_hooks = std::define_static_array(after_hooks);
  result.before_all_hooks = std::define_static_array(before_all_hooks);
  result.after_all_hooks = std::define_static_array(after_all_hooks);
  return suite_discovery{result, std::define_static_array(children)};
}

using test_callback = void (*)(void*);

int execute_test(void* context, std::string_view name, test_callback test, test_callback before,
                 test_callback after, bool extensions = true);
void record_exception(const std::string& phase);
int execute_cleanup(std::string_view name, std::function<void()> cleanup);

template <auto Method, typename T>
void invoke_test_method(void* context) {
  if constexpr (!requires(T& value) { value.[:Method:](); })
    static_cast<T*>(context)->[:Method:](current_test());
  else
    static_cast<T*>(context)->[:Method:]();
}

struct suite_hook_state {
  std::shared_ptr<suite_hook_state> parent;
  std::function<void()> before, after;
  test_context context;
  test_xml_log preparation;
  bool entered = false, finished = false, claimed = false, has_hooks = false;
};
void ensure_suite_hooks(const std::shared_ptr<suite_hook_state>& state);
void finish_suite_hooks(const std::shared_ptr<suite_hook_state>& state);

void delete_fixture_context(test_context* context) noexcept;
using fixture_context = std::unique_ptr<test_context, decltype(&delete_fixture_context)>;
fixture_context make_fixture_context();

template <typename T>
struct fixture_state {
  std::shared_ptr<suite_hook_state> suite_hooks;
  std::unique_ptr<T> value;
  fixture_context context{nullptr, delete_fixture_context};
  std::function<std::unique_ptr<T>(test_context&)> construct;
  std::function<void()> parent_before, parent_after;
  bool parent_entered = false;
  bool retain = false;
};

void copy_context(test_context& target, const test_context& source);

template <typename T>
void destroy_fixture(fixture_state<T>& fixture, bool preserve_context = false) {
  current_phase = "destructor";
  // Release before delete so a noexcept(false) destructor is reported rather than
  // being invoked through unique_ptr's noexcept destructor/reset.
  auto* value = fixture.value.release();
  try {
    delete value;
  } catch (...) {
    record_exception(current_phase);
  }
  if (!preserve_context) fixture.context.reset();
}

template <auto Suite, typename T>
void begin_fixture(fixture_state<T>& fixture) {
  ensure_suite_hooks(fixture.suite_hooks);
  if (fixture.parent_before) {
    fixture.parent_entered = true;
    fixture.parent_before();
  }
  if (!fixture.context) fixture.context = make_fixture_context();
  copy_context(*fixture.context, current_test());
  current_phase = "constructor";
  if (!fixture.value) {
    if (fixture.construct)
      fixture.value = fixture.construct(*fixture.context);
    else if constexpr (std::is_constructible_v<T, test_context&>)
      fixture.value = std::make_unique<T>(*fixture.context);
    else if constexpr (std::is_default_constructible_v<T>)
      fixture.value = std::make_unique<T>();
    else
      throw std::logic_error("Fixture requires an enclosing fixture constructor");
    if (!fixture.value) throw std::logic_error("Fixture factory returned null");
  }
  current_phase = "before_each";
  static constexpr auto hooks = Suite.before_hooks;
  static constexpr auto indices = std::make_index_sequence<hooks.size>{};
  template for (constexpr auto i : indices) {
    static constexpr auto method = hooks.data[i];
    invoke_test_method<method, T>(fixture.value.get());
  }
}

template <auto Suite, typename T>
void end_fixture(fixture_state<T>& fixture, bool force = false, bool preserve_context = false) {
  if (fixture.value) {
    static constexpr auto hooks = Suite.after_hooks;
    static constexpr auto indices = std::make_index_sequence<hooks.size>{};
    template for (constexpr auto i : indices) {
      static constexpr auto method = hooks.data[hooks.size - 1 - i];
      current_phase = "after_each";
      try {
        invoke_test_method<method, T>(fixture.value.get());
      } catch (...) {
        record_exception(current_phase);
      }
    }
    if (force || !fixture.retain) destroy_fixture(fixture, preserve_context);
  }
  if (fixture.parent_entered) {
    fixture.parent_entered = false;
    fixture.parent_after();
  }
}

template <auto Suite, typename T>
int execute_fixture(fixture_state<T>& fixture, std::string_view name,
                    std::function<void(T&)> body) {
  struct callbacks {
    fixture_state<T>* fixture;
    std::function<void(T&)> body;
  } actions{&fixture, std::move(body)};
  return execute_test(
      &actions, name,
      [](void* p) {
        auto& a = *static_cast<callbacks*>(p);
        a.body(*a.fixture->value);
      },
      [](void* p) { begin_fixture<Suite>(*static_cast<callbacks*>(p)->fixture); },
      [](void* p) { end_fixture<Suite>(*static_cast<callbacks*>(p)->fixture); });
}

// Static test bodies need only a function pointer. Keep the fixture callbacks
// shared per suite instead of instantiating std::function for every method/case.
template <auto Suite, typename T>
int execute_static_fixture(fixture_state<T>& fixture, std::string_view name, test_callback body) {
  struct callbacks {
    fixture_state<T>* fixture;
    test_callback body;
  } actions{&fixture, body};
  return execute_test(
      &actions, name,
      [](void* p) {
        auto& a = *static_cast<callbacks*>(p);
        a.body(a.fixture->value.get());
      },
      [](void* p) { begin_fixture<Suite>(*static_cast<callbacks*>(p)->fixture); },
      [](void* p) { end_fixture<Suite>(*static_cast<callbacks*>(p)->fixture); });
}

template <auto Suite, auto Test, typename T>
int run_test(fixture_state<T>& fixture, std::string_view name) {
  return execute_static_fixture<Suite>(fixture, name, invoke_test_method<Test, T>);
}

template <auto test_name, auto i>
consteval auto get_parameterized_test_name() {
  return std::define_static_string(std::string(test_name) + "." + to_string(i + 1));
}

struct parameter_data {
  const void* values;
  std::string (*describe)(const void*);
};

// An explicit reference also avoids GCC dropping the reference when deducing
// decltype(auto) directly from a class template parameter object.
template <auto Parameter>
inline constexpr const std::remove_cv_t<decltype(Parameter)>& parameter_object = Parameter;

template <auto Parameter>
consteval decltype(auto) stored_parameter() {
  if constexpr (std::is_class_v<decltype(Parameter)> || std::is_union_v<decltype(Parameter)>)
    return (parameter_object<Parameter>);
  else
    return Parameter;
}

template <auto Parameters>
consteval auto parameter_storage() {
  constexpr auto scalar_types = []<typename... Values>(std::tuple<Values...>*) {
    return ((!std::is_class_v<Values> && !std::is_union_v<Values>) && ...);
  }(static_cast<decltype(as_std_tuple(Parameters))*>(nullptr));
  if constexpr (scalar_types) {
    return as_std_tuple(Parameters);
  } else {
    // Keep references to class template parameter objects, whose identity can
    // be observed by const-reference arguments. Scalars are stored by value.
    return []<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::tuple<decltype(stored_parameter<Parameters.template get<Is>()>())...>{
          stored_parameter<Parameters.template get<Is>()>()...};
    }(std::make_index_sequence<std::remove_cv_t<decltype(Parameters)>::size()>{});
  }
}

std::string format_parameter_list(std::span<const std::string> fields);

template <typename Values>
std::string print_parameters(const Values& values) {
  // Diagnostics use the same owning tuple types as the original invocation.
  return std::apply(
      [](const auto&... args) {
        // Built-in numeric tuples have no user-defined formatter to preserve.
        // Keep their default tuple syntax without instantiating tuple parsing.
        if constexpr (((std::is_arithmetic_v<std::remove_cvref_t<decltype(args)>> &&
                        std::formattable<std::remove_cvref_t<decltype(args)>, char> &&
                        !std::same_as<std::remove_cvref_t<decltype(args)>, char>) && ...)) {
          std::array<std::string, sizeof...(args)> fields{print_value(args)...};
          return format_parameter_list(fields);
        } else {
          return print_value(std::tuple{args...});
        }
      },
      values);
}

template <typename Values>
std::string describe_parameters(const void* data) {
  return print_parameters(*static_cast<const Values*>(data));
}

// Values specialize only static storage, not executable invocation code.
template <auto Parameters>
consteval parameter_data describe_parameter_data() {
  static constexpr auto values = parameter_storage<Parameters>();
  using Values = std::remove_cv_t<decltype(values)>;
  return {&values, describe_parameters<Values>};
}

template <typename Value>
decltype(auto) parameter_argument(const Value& value) {
  // Class NTTPs are const lvalues; scalar NTTPs are prvalues. Preserve those
  // categories when reading values from storage (including int&& arguments).
  if constexpr (std::is_class_v<Value> || std::is_union_v<Value>)
    return (value);
  else
    return Value(value);
}

template <auto Method, typename T, typename Values>
void invoke_parameter_method(void* object, const void* data) {
  if constexpr (std::tuple_size_v<Values> == 0) {
    invoke_test_method<Method, T>(object);
  } else {
    const auto& values = *static_cast<const Values*>(data);
    current_test().parameters = print_parameters(values);
    record_property("parameters", current_test().parameters);
    std::apply(
        [&](const auto&... args) {
          static_cast<T*>(object)->[:Method:](parameter_argument(args)...);
        },
        values);
  }
}

struct parameter_invocation {
  parameter_data parameters;
  void (*invoke)(void*, const void*);
};

std::string parameter_case_name(std::string_view base, int index);

template <auto Method, typename T, auto Parameters>
consteval parameter_invocation describe_parameter_invocation() {
  using Values = decltype(parameter_storage<Parameters>());
  return {describe_parameter_data<Parameters>(), invoke_parameter_method<Method, T, Values>};
}

template <auto Suite, typename T>
int execute_parameter_fixture(fixture_state<T>& fixture, const parameter_invocation& test,
                              std::string_view name) {
  struct callbacks {
    fixture_state<T>* fixture;
    const parameter_invocation* test;
  } actions{&fixture, &test};
  return execute_test(
      &actions, name,
      [](void* p) {
        auto& a = *static_cast<callbacks*>(p);
        a.test->invoke(a.fixture->value.get(), a.test->parameters.values);
      },
      [](void* p) { begin_fixture<Suite>(*static_cast<callbacks*>(p)->fixture); },
      [](void* p) { end_fixture<Suite>(*static_cast<callbacks*>(p)->fixture); });
}

template <auto Suite, internal_test Test>
int run_parameterized_test(auto& s, int parameterize_target_idx) {
  using Annotation = typename[:std::meta::type_of(Test.parameterize_annotation):];
  static constexpr auto parameterize_struct =
      std::meta::extract<Annotation>(Test.parameterize_annotation);
  static constexpr auto indices = std::make_index_sequence<Annotation::N>{};
  using T = std::remove_reference_t<decltype(*s.value)>;
  static constexpr auto cases = [] {
    std::array<parameter_invocation, Annotation::N> result{};
    template for (constexpr auto i : indices) {
      constexpr auto parameters = parameterize_struct.parameters.template get<i>().params;
      result[i] = describe_parameter_invocation<Test.test, T, parameters>();
    }
    return result;
  }();
  if (parameterize_target_idx < 0 || parameterize_target_idx >= cases.size()) return 0;
  auto name = parameter_case_name(Test.name, parameterize_target_idx);
  return execute_parameter_fixture<Suite>(s, cases[parameterize_target_idx], name);
}

template <auto Suite, internal_test Test>
int run_parameterized_template_test(auto& s, int parameterize_target_idx) {
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

    if (i != parameterize_target_idx) continue;

    static constexpr auto specialization = std::meta::substitute(Test.raw_test, template_args);
    status_code |= run_test<Suite, specialization>(s, test_name);

    break;
  }

  return status_code;
}

template <auto Suite, internal_test Test>
int run_parameterized_dual_test(auto& s, int parameterize_target_idx) {
  using Annotation = typename[:std::meta::type_of(Test.parameterize_annotation):];
  static constexpr auto parameterize_struct =
      std::meta::extract<Annotation>(Test.parameterize_annotation);

  static constexpr auto template_args_list = std::define_static_array(
      std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation)));
  static constexpr auto idx_sequence = std::make_index_sequence<parameterize_struct.N>{};
  using T = std::remove_reference_t<decltype(*s.value)>;
  static constexpr auto cases = [] {
    std::array<parameter_invocation, Annotation::N> result{};
    template for (constexpr auto i : idx_sequence) {
      constexpr auto template_args = std::define_static_array(std::meta::template_arguments_of(
          std::meta::template_arguments_of(template_args_list[i])[0]));
      constexpr auto parameters = parameterize_struct.sets.template get<i>().params.params;
      constexpr auto specialization = std::meta::substitute(Test.raw_test, template_args);
      result[i] = describe_parameter_invocation<specialization, T, parameters>();
    }
    return result;
  }();
  if (parameterize_target_idx < 0 || parameterize_target_idx >= cases.size()) return 0;
  auto name = parameter_case_name(Test.name, parameterize_target_idx);
  return execute_parameter_fixture<Suite>(s, cases[parameterize_target_idx], name);
}

template <internal_test Test>
std::string parameter_description(int index) {
  using annotation = typename[:std::meta::type_of(Test.parameterize_annotation):];
  std::string result;
  if constexpr (Test.parameterization_type == parameterization_types::Value ||
                Test.parameterization_type == parameterization_types::Dual) {
    static constexpr auto source = std::meta::extract<annotation>(Test.parameterize_annotation);
    static constexpr auto indices = std::make_index_sequence<annotation::N>{};
    static constexpr auto cases = [] {
      std::array<parameter_data, annotation::N> output{};
      template for (constexpr auto i : indices) {
        if constexpr (Test.parameterization_type == parameterization_types::Value)
          output[i] = describe_parameter_data<source.parameters.template get<i>().params>();
        else
          output[i] = describe_parameter_data<source.sets.template get<i>().params.params>();
      }
      return output;
    }();
    if (index >= 0 && index < cases.size()) result = cases[index].describe(cases[index].values);
  }
  if constexpr (Test.parameterization_type == parameterization_types::Template ||
                Test.parameterization_type == parameterization_types::Dual) {
    static constexpr auto groups = std::define_static_array(
        std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation)));
    static constexpr auto indices = std::make_index_sequence<groups.size()>{};
    template for (constexpr auto i : indices) {
      if (index == i) {
        constexpr auto group = Test.parameterization_type == parameterization_types::Dual
                                   ? std::meta::template_arguments_of(groups[i])[0]
                                   : groups[i];
        if (!result.empty()) result += "; ";
        result += "types: ";
        bool first = true;
        template for (constexpr auto type :
                      std::define_static_array(std::meta::template_arguments_of(group))) {
          if (!std::exchange(first, false)) result += ", ";
          result += std::meta::display_string_of(type);
        }
      }
    }
  }
  return result;
}

struct runner_options {
  std::vector<std::string> includes, excludes, include_tags, exclude_tags;
  bool run_disabled = false, shuffle = false, fail_fast = false, isolate = false, worker = false;
  bool seed_given = false;
  unsigned worker_repetition = 0;
  unsigned repeat = 1, seed = 0, shard_count = 1, shard_index = 0, jobs = 1;
  double timeout_seconds = 0;
  std::string worker_result;
  bool list_tests = false, list_json = false;
  bool should_exit = false;
  int exit_code = 0;
  std::string xml_output, json_output;
  bool brief = false, break_on_failure = false;
  std::vector<std::string> test_names;
};

runner_options parse_options(int argc, char** argv);

struct discovered_test {
  const char* name = nullptr;
  bool parameterized = false;
  int parameter_count = 0;
  bool disabled = false;
  bool has_required_os = false;
  std::span<const operating_system> required_os;
  std::span<const operating_system> disallowed_os;
  test_callback body = nullptr;
  int (*run)(void*, int) = nullptr;
};

template <auto Suite, internal_test Test, typename T>
int run_discovered_test(void* context, int index) {
  auto& s = *static_cast<fixture_state<T>*>(context);
  if constexpr (Test.parameterized) {
    if constexpr (Test.parameterization_type == parameterization_types::Value) {
      return run_parameterized_test<Suite, Test>(s, index);
    } else if constexpr (Test.parameterization_type == parameterization_types::Template) {
      return run_parameterized_template_test<Suite, Test>(s, index);
    } else {
      return run_parameterized_dual_test<Suite, Test>(s, index);
    }
  } else {
    static constexpr auto name = Test.name;
    return run_test<Suite, Test.test>(s, name);
  }
}

template <auto Suite, internal_test Test, typename T>
consteval discovered_test describe_test() {
  discovered_test entry{.name = Test.name,
                        .parameterized = Test.parameterized,
                        .parameter_count = Test.num_parameterizations,
                        .disabled = Test.disabled,
                        .has_required_os = Test.has_requires_os};
  if constexpr (!Test.runtime_parameters && !Test.dynamic) {
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

struct execution_case {
  std::string name, base_name, parameters;
  std::string file;
  std::size_t line = 0;
  std::vector<std::string> tags;
  std::string skip_reason;
  bool disabled = false;
  bool discovery_failed = false;
  double timeout_seconds = 0;
  unsigned repetitions = 1;
  std::vector<std::function<std::optional<std::string>(const test_context&)>> conditions;
  std::function<int()> run;
  std::string group_name;
  std::function<void()> finish_group;
  std::function<std::vector<execution_case>()> expand;
  std::function<void(std::shared_ptr<suite_hook_state>)> set_suite_hooks;
};
using execution_conditions =
    std::vector<std::function<std::optional<std::string>(const test_context&)>>;

execution_case make_execution_case(const discovered_test& description, static_tags tags,
                                   const std::source_location& location, bool disabled,
                                   double timeout, unsigned repetitions,
                                   const std::vector<std::string>& inherited_tags,
                                   const execution_conditions& conditions);

template <std::meta::info Reflection>
void append_conditions(execution_conditions& conditions) {
  template for (constexpr auto annotation :
                std::define_static_array(expanded_annotations(Reflection))) {
    if constexpr (std::meta::has_template_arguments(std::meta::type_of(annotation))) {
      constexpr auto kind = std::meta::template_of(std::meta::type_of(annotation));
      if constexpr (kind == ^^enabled_if || kind == ^^disabled_if) {
        using Policy = typename[:std::meta::type_of(annotation):];
        static_assert(
            std::is_invocable_r_v<bool, decltype(Policy::predicate), const test_context&> ||
                std::is_invocable_r_v<bool, decltype(Policy::predicate)>,
            "Execution conditions must return bool and accept no arguments or const test_context&");
        constexpr auto value = std::meta::extract<Policy>(annotation);
        constexpr bool enabled = kind == ^^enabled_if;
        conditions.push_back([reason = std::string(value.reason)](
                                 const test_context& context) -> std::optional<std::string> {
          bool matched;
          if constexpr (std::is_invocable_r_v<bool, decltype(Policy::predicate),
                                              const test_context&>)
            matched = std::invoke(Policy::predicate, context);
          else if constexpr (std::is_invocable_r_v<bool, decltype(Policy::predicate)>)
            matched = std::invoke(Policy::predicate);
          else
            matched = false;
          if (matched == enabled) return std::nullopt;
          return reason.empty() ? "Execution condition disabled test" : reason;
        });
      }
    }
  }
}
std::optional<std::string> condition_skip(const execution_conditions& conditions,
                                          const test_context& context);

template <std::meta::info Reflection>
void append_suite_conditions(execution_conditions& conditions) {
  template for (constexpr auto base : std::define_static_array(
                    std::meta::bases_of(Reflection, std::meta::access_context::unchecked()))) {
    if constexpr (std::meta::is_public(base))
      append_suite_conditions<std::meta::type_of(base)>(conditions);
  }
  append_conditions<Reflection>(conditions);
}
struct execution_suite {
  std::string name, file;
  std::vector<execution_case> cases;
  std::function<void()> finish_fixture;
  std::shared_ptr<suite_hook_state> suite_hooks;
  std::type_index fixture_type = typeid(void);
};

// Keep aggregate execution_suite available to implementation clients while
// compiling its construction and destruction once in the runtime module.
struct suite_builder {
  execution_suite suite;
  suite_builder(std::string name, std::string file);
  ~suite_builder();
};
std::string commit_suite(std::vector<execution_suite>& suites, execution_suite& suite);
struct static_test_description {
  discovered_test test;
  static_tags tags;
  std::source_location location;
  double timeout_seconds;
  unsigned repetitions;
  void (*conditions)(execution_conditions&) = nullptr;
  std::string (*parameters)(int) = nullptr;
};

consteval bool has_conditions(std::meta::info reflection) {
  for (auto annotation : expanded_annotations(reflection)) {
    auto type = std::meta::type_of(annotation);
    if (!std::meta::has_template_arguments(type)) continue;
    auto kind = std::meta::template_of(type);
    if (kind == ^^enabled_if || kind == ^^disabled_if) return true;
  }
  return false;
}

template <auto Suite, internal_test Test, typename T>
consteval static_test_description describe_static_test() {
  static_test_description description{describe_test<Suite, Test, T>(), Test.tags,
                                      std::meta::source_location_of(Test.test),
                                      Test.timeout_seconds, Test.repetitions};
  if constexpr (has_conditions(Test.test)) description.conditions = append_conditions<Test.test>;
  if constexpr (Test.parameterized) description.parameters = parameter_description<Test>;
  return description;
}

void append_static_cases(execution_suite& output, std::shared_ptr<void> fixture,
                         std::span<const static_test_description> descriptions,
                         bool parent_disabled, const std::vector<std::string>& inherited_tags,
                         const execution_conditions& conditions);
void append_static_case(execution_suite& output, execution_case entry,
                        std::shared_ptr<void> fixture, int (*run)(void*, int), int index = -1,
                        std::string parameters = {});
void validate_case_names(const execution_suite& suite);

int run_plan(std::vector<execution_suite> suites, const runner_options& options, int argc,
             char** argv);

template <typename Child, typename Parent, auto ParentSuite>
std::shared_ptr<fixture_state<Child>> nested_fixture(
    std::shared_ptr<fixture_state<Parent>> parent) {
  static constexpr auto child_suite = get_tests<^^Child>().suite;
  // A retained child may keep Parent& (including through its destructor).
  // Keeping the parent's state alive does not keep its per-test value alive.
  if constexpr (child_suite.lifecycle == fixture_lifecycle::shared &&
                ParentSuite.lifecycle != fixture_lifecycle::shared)
    throw std::logic_error("Shared nested fixtures require a shared enclosing fixture");
  auto child = std::make_shared<fixture_state<Child>>();
  child->suite_hooks = parent->suite_hooks;
  child->parent_before = [parent] { begin_fixture<ParentSuite>(*parent); };
  child->parent_after = [parent] { end_fixture<ParentSuite>(*parent); };
  child->construct = [parent](test_context& context) -> std::unique_ptr<Child> {
    if constexpr (std::is_constructible_v<Child, Parent&, test_context&>)
      return std::make_unique<Child>(*parent->value, context);
    else if constexpr (std::is_constructible_v<Child, Parent&>)
      return std::make_unique<Child>(*parent->value);
    else if constexpr (std::is_constructible_v<Child, test_context&>)
      return std::make_unique<Child>(context);
    else
      return std::make_unique<Child>();
  };
  return child;
}

void begin_extensions(std::vector<std::shared_ptr<test_listener>>& entered);
void end_extensions(std::span<const std::shared_ptr<test_listener>> entered);

template <auto Test, typename T>
auto invoke_factory(T& object) {
  constexpr auto provider_info =
      std::meta::template_arguments_of(std::meta::type_of(Test.parameterize_annotation))[0];
  constexpr auto provider =
      std::meta::extract<typename[:std::meta::type_of(provider_info):]>(provider_info);
  if constexpr (std::is_null_pointer_v<decltype(provider)>) {
    if constexpr (requires { object.[:Test.test:](); })
      return object.[:Test.test:]();
    else
      return object.[:Test.test:](current_test());
  } else if constexpr (std::is_member_function_pointer_v<decltype(provider)>) {
    if constexpr (std::is_invocable_v<decltype(provider), T&>)
      return std::invoke(provider, object);
    else
      return std::invoke(provider, object, current_test());
  } else
    return std::invoke(provider);
}

template <typename T>
struct factory_state {
  std::shared_ptr<fixture_state<T>> fixture;
  std::vector<dynamic_case> cases;
  std::vector<std::shared_ptr<test_listener>> extensions;
  test_xml_log preparation;
  bool claimed = false, finished = false;
  execution_conditions conditions;
  std::string skip_reason;
};

void check_case_name(std::string_view name);

template <typename T>
void append_suite(std::vector<execution_suite>&, std::string = {}, std::vector<std::string> = {},
                  bool = false, std::function<std::shared_ptr<fixture_state<T>>()> = {},
                  std::function<std::shared_ptr<fixture_state<T>>()> = {},
                  execution_conditions = {});

std::shared_ptr<suite_hook_state> make_suite_hooks_runtime(
    std::string name, std::vector<std::string> tags, std::shared_ptr<suite_hook_state> parent,
    static_tags labels, bool has_hooks, void (*before)(), void (*after)());

template <auto Suite, typename T>
std::shared_ptr<suite_hook_state> make_suite_hooks(std::string name, std::vector<std::string> tags,
                                                   std::shared_ptr<suite_hook_state> parent = {}) {
  static constexpr bool has_suite_hooks =
      Suite.before_all_hooks.size > 0 || Suite.after_all_hooks.size > 0;
  auto before = +[] {
    static constexpr auto hooks = Suite.before_all_hooks;
    static constexpr auto indices = std::make_index_sequence<hooks.size>{};
    template for (constexpr auto i : indices) {
      current_phase = "before_all";
      invoke_test_method<hooks.data[i], T>(nullptr);
    }
  };
  auto after = +[] {
    static constexpr auto hooks = Suite.after_all_hooks;
    static constexpr auto indices = std::make_index_sequence<hooks.size>{};
    template for (constexpr auto i : indices) {
      current_phase = "after_all";
      try {
        invoke_test_method<hooks.data[hooks.size - 1 - i], T>(nullptr);
      } catch (...) {
        record_exception(current_phase);
      }
    }
  };
  static constexpr auto labels = Suite.tags;
  return make_suite_hooks_runtime(std::move(name), std::move(tags), std::move(parent), labels,
                                  has_suite_hooks, before, after);
}

template <typename T>
void append_suite_instance(std::vector<execution_suite>& suites, std::string prefix = {},
                           std::vector<std::string> inherited_tags = {},
                           bool parent_disabled = false,
                           std::function<std::shared_ptr<fixture_state<T>>()> creator = {},
                           std::function<std::shared_ptr<fixture_state<T>>()> fresh_creator = {},
                           std::string case_name = {}, execution_conditions conditions = {}) {
  static constexpr auto discovery = get_tests<^^T>();
  static constexpr auto Suite = discovery.suite;
  static constexpr bool suite_disabled = Suite.disabled;
  append_suite_conditions<^^T>(conditions);
  static constexpr auto suite_name = Suite.name;
  static constexpr auto location = std::meta::source_location_of(^^T);
  suite_builder prepared(prefix + suite_name + case_name, location.file_name());
  auto& output = prepared.suite;
  static constexpr bool retained = Suite.lifecycle == fixture_lifecycle::shared;
  static constexpr bool nested_suites = [] {
    template for (constexpr auto member : std::define_static_array(
                      std::meta::members_of(^^T, std::meta::access_context::unchecked()))) {
      if constexpr (std::meta::is_type(member) && std::meta::is_class_type(member)) {
        template for (constexpr auto annotation :
                      std::define_static_array(expanded_annotations(member))) {
          if constexpr (std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(annotation):]>,
                                       nested>) return true;
        }
      }
    }
    return false;
  }();
  static constexpr bool dynamic_tests = [] {
    for (const auto& test : discovery.tests)
      if (test.dynamic) return true;
    return false;
  }();
  if constexpr (nested_suites || dynamic_tests) {
    if (!creator) creator = [] { return std::make_shared<fixture_state<T>>(); };
  }
  if constexpr (nested_suites) {
    if (!fresh_creator) fresh_creator = [] { return std::make_shared<fixture_state<T>>(); };
  }
  auto fixture = creator ? creator() : std::make_shared<fixture_state<T>>();
  fixture->retain = retained;
  auto hook_state = make_suite_hooks<Suite, T>(output.name, inherited_tags, fixture->suite_hooks);
  output.suite_hooks = hook_state;
  output.fixture_type = typeid(T);
  fixture->suite_hooks = hook_state;
  output.finish_fixture = [fixture, hook_state] {
    if (fixture->value) destroy_fixture(*fixture);
    finish_suite_hooks(hook_state);
  };
  static constexpr bool static_cases = [] {
    for (const auto& test : discovery.tests)
      if (test.dynamic || test.runtime_parameters) return false;
    return true;
  }();
  if constexpr (static_cases) {
    static constexpr auto descriptions = [] {
      std::array<static_test_description, discovery.tests.size()> result{};
      std::size_t index = 0;
      template for (constexpr auto test : discovery.tests) {
        result[index++] = describe_static_test<Suite, test, T>();
      }
      return result;
    }();
    append_static_cases(output, fixture, descriptions, parent_disabled, inherited_tags, conditions);
  } else {
    template for (constexpr auto test : discovery.tests) {
      static constexpr auto described = describe_test<Suite, test, T>();
      auto entry = make_execution_case(
          described, test.tags, std::meta::source_location_of(test.test),
          test.disabled || parent_disabled, test.timeout_seconds, test.repetitions,
          inherited_tags, conditions);
      append_conditions<test.test>(entry.conditions);
      if constexpr (test.dynamic) {
        auto prototype = entry;
        std::string suite_name = output.name;
        entry.expand = [creator, prototype, suite_name, hook_state]() mutable {
          auto group = std::make_shared<factory_state<T>>();
          group->conditions = prototype.conditions;
          group->skip_reason = prototype.skip_reason;
          group->fixture = creator();
          group->fixture->suite_hooks = hook_state;
          group->fixture->retain = true;
          group->fixture->context = make_fixture_context();
          group->fixture->context->suite = suite_name;
          group->fixture->context->name = prototype.name;
          group->fixture->context->tags = prototype.tags;
          auto* previous = active_context;
          active_context = group->fixture->context.get();
          struct context_guard {
            test_context* previous;
            ~context_guard() { active_context = previous; }
          } restore{previous};
          execute_test(
              group.get(), prototype.name,
              [](void* pointer) {
                auto& state = *static_cast<factory_state<T>*>(pointer);
                current_phase = "condition";
                if (!state.skip_reason.empty()) skip(state.skip_reason);
                if (auto reason = condition_skip(state.conditions, current_test())) skip(*reason);
                begin_extensions(state.extensions);
                begin_fixture<Suite>(*state.fixture);
                current_phase = "factory";
                auto produced = invoke_factory<test>(*state.fixture->value);
                for (auto&& item : produced) state.cases.push_back(std::move(item));
                if (state.cases.empty()) throw std::logic_error("Test factory returned no cases");
                std::unordered_set<std::string> names;
                for (const auto& item : state.cases) {
                  check_case_name(item.name);
                  if (!names.insert(item.name).second)
                    throw std::logic_error("Duplicate dynamic case name: " + item.name);
                }
              },
              nullptr, nullptr, false);
          group->preparation = std::move(suite_log.testcases.back());
          suite_log.testcases.pop_back();
          auto finish = [group] {
            if (std::exchange(group->finished, true)) return;
            auto* previous = active_context;
            active_context = group->fixture->context.get();
            end_fixture<Suite>(*group->fixture, true, true);
            end_extensions(group->extensions);
            group->extensions.clear();
            group->fixture->context.reset();
            active_context = previous;
          };
          std::vector<execution_case> output;
          if (group->preparation.failed || !group->preparation.skip_entries.empty()) {
            auto failed = prototype;
            failed.finish_group = finish;
            failed.group_name = prototype.name;
            failed.discovery_failed = group->preparation.failed;
            failed.run = [group, name = prototype.name] {
              return execute_test(
                  group.get(), name,
                  [](void* p) {
                    auto& original = static_cast<factory_state<T>*>(p)->preparation;
                    auto& entry = suite_log.testcases.back();
                    entry.failures = original.failures;
                    entry.skip_entries = original.skip_entries;
                    std::cout << original.sysout;
                    std::cerr << original.syserr;
                  },
                  nullptr, nullptr, false);
            };
            output.push_back(std::move(failed));
          } else
            for (std::size_t index = 0; index < group->cases.size(); ++index) {
              auto entry = prototype;
              entry.name += "." + group->cases[index].name;
              entry.group_name = prototype.name;
              entry.finish_group = finish;
              entry.run = [group, index, name = entry.name] {
                struct action {
                  factory_state<T>* group;
                  std::size_t index;
                } body{group.get(), index};
                return execute_test(
                    &body, name,
                    [](void* p) {
                      auto& action = *static_cast<struct action*>(p);
                      if (!std::exchange(action.group->claimed, true)) {
                        std::cout << action.group->preparation.sysout;
                        std::cerr << action.group->preparation.syserr;
                      }
                      std::invoke(action.group->cases[action.index].body);
                    },
                    nullptr, nullptr, false);
              };
              output.push_back(std::move(entry));
            }
          return output;
        };
        output.cases.push_back(std::move(entry));
      } else if constexpr (test.runtime_parameters) {
        constexpr auto provider_info =
            std::meta::template_arguments_of(std::meta::type_of(test.parameterize_annotation))[0];
        auto values = std::invoke(
            std::meta::extract<typename[:std::meta::type_of(provider_info):]>(provider_info));
        if (std::ranges::empty(values)) throw std::logic_error("Test provider returned no cases");
        int index = 0;
        for (auto&& value : values) {
          auto concrete = entry;
          ++index;
          using Value = std::decay_t<decltype(value)>;
          auto owned = std::make_shared<Value>(std::forward<decltype(value)>(value));
          if constexpr (requires { owned->name; }) {
            check_case_name(owned->name);
            concrete.name += "." + owned->name;
          } else
            concrete.name += "." + std::to_string(index);
          auto name = concrete.name;
          {
            auto& arguments = [&]() -> auto& {
              if constexpr (requires { owned->values; })
                return owned->values;
              else
                return *owned;
            }();
            concrete.parameters = print_value(arguments);
            concrete.run = [fixture, owned, name] {
              return execute_fixture<Suite, T>(*fixture, name, [owned](T& object) {
                auto& args = [&]() -> auto& {
                  if constexpr (requires { owned->values; })
                    return owned->values;
                  else
                    return *owned;
                }();
                std::apply([&](auto&&... args) { object.[:test.test:](args...); }, args);
              });
            };
          }
          output.cases.push_back(std::move(concrete));
        }
      } else if constexpr (test.parameterized) {
        for (int index = 0; index < test.num_parameterizations; ++index) {
          append_static_case(output, entry, fixture, described.run, index,
                             parameter_description<test>(index));
        }
      } else {
        append_static_case(output, std::move(entry), fixture, described.run);
      }
    }
  }
  auto nested_prefix = commit_suite(suites, output);
  std::vector<std::string> nested_tags = inherited_tags;
  static constexpr auto suite_labels = Suite.tags;
  for (auto label : suite_labels) nested_tags.emplace_back(label);
  template for (constexpr auto member : std::define_static_array(
                    std::meta::members_of(^^T, std::meta::access_context::unchecked()))) {
    if constexpr (std::meta::is_type(member) && std::meta::is_class_type(member)) {
      template for (constexpr auto annotation :
                    std::define_static_array(expanded_annotations(member))) {
        if constexpr (std::is_same_v<std::remove_cv_t<typename[:std::meta::type_of(annotation):]>,
                                     nested>)
          append_suite<typename[:member:]>(
              suites, nested_prefix, nested_tags, parent_disabled || suite_disabled,
              [fixture] { return nested_fixture<typename[:member:], T, Suite>(fixture); },
              [fresh_creator] {
                return nested_fixture<typename[:member:], T, Suite>(fresh_creator());
              },
              conditions);
      }
    }
  }
}

template <typename T, typename Arguments>
auto parameterized_fixture(std::shared_ptr<Arguments> arguments) {
  auto fixture = std::make_shared<fixture_state<T>>();
  fixture->construct = [arguments](test_context& context) {
    context.parameters = print_value(*arguments);
    current_test().parameters = context.parameters;
    record_property("suite_parameters", context.parameters);
    return std::apply(
        [&](auto&&... values) {
          if constexpr (std::is_constructible_v<T, decltype(values)..., test_context&>)
            return std::make_unique<T>(values..., context);
          else
            return std::make_unique<T>(values...);
        },
        *arguments);
  };
  return fixture;
}
template <typename T>
void append_suite(std::vector<execution_suite>& suites, std::string prefix,
                  std::vector<std::string> tags, bool disabled,
                  std::function<std::shared_ptr<fixture_state<T>>()> creator,
                  std::function<std::shared_ptr<fixture_state<T>>()> fresh_creator,
                  execution_conditions conditions) {
  bool parameterized = false;
  template for (constexpr auto annotation : std::define_static_array(expanded_annotations(^^T))) {
    if constexpr (std::meta::has_template_arguments(std::meta::type_of(annotation)) &&
                  std::meta::template_of(std::meta::type_of(annotation)) == ^^parameterize) {
      static constexpr auto source =
          std::meta::extract<typename[:std::meta::type_of(annotation):]>(annotation);
      static_assert(decltype(source)::N > 0,
                    "Suite parameterization must contain at least one case");
      parameterized = true;
      static constexpr auto indices = std::make_index_sequence<decltype(source)::N>{};
      template for (constexpr auto index : indices) {
        auto values = std::make_shared<decltype(as_std_tuple(
            source.parameters.template get<index>().params))>(
            as_std_tuple(source.parameters.template get<index>().params));
        auto factory = [values] { return parameterized_fixture<T>(values); };
        append_suite_instance<T>(suites, prefix, tags, disabled, factory, factory,
                                 "." + std::to_string(index + 1), conditions);
      }
    } else if constexpr (std::meta::has_template_arguments(std::meta::type_of(annotation)) &&
                         std::meta::template_of(std::meta::type_of(annotation)) ==
                             ^^parameterize_template) {
      static_assert(std::meta::has_template_arguments(^^T),
                    "Typed suite parameters require a class template");
      static constexpr auto groups = std::define_static_array(
          std::meta::template_arguments_of(std::meta::type_of(annotation)));
      static_assert(groups.size() > 0, "Suite parameterization must contain at least one case");
      parameterized = true;
      template for (constexpr auto group : groups) {
        static constexpr auto specialization = std::meta::substitute(
            std::meta::template_of(^^T),
            std::define_static_array(std::meta::template_arguments_of(group)));
        append_suite_instance<typename[:specialization:]>(suites, prefix, tags, disabled, {}, {},
                                                          {}, conditions);
      }
    } else if constexpr (std::meta::has_template_arguments(std::meta::type_of(annotation)) &&
                         std::meta::template_of(std::meta::type_of(annotation)) ==
                             ^^parameter_source) {
      using Source = typename[:std::meta::type_of(annotation):];
      auto cases = std::invoke(Source::provider);
      parameterized = true;
      std::unordered_set<std::string> names;
      unsigned index = 0;
      for (auto&& item : cases) {
        ++index;
        std::string name = std::to_string(index);
        if constexpr (requires { item.name; }) name = item.name;
        check_case_name(name);
        if (!names.insert(name).second)
          throw std::logic_error("Duplicate suite parameter name: " + name);
        auto arguments = [&] {
          if constexpr (requires { item.values; })
            return std::make_shared<std::decay_t<decltype(item.values)>>(item.values);
          else
            return std::make_shared<std::decay_t<decltype(item)>>(item);
        }();
        auto factory = [arguments] { return parameterized_fixture<T>(arguments); };
        append_suite_instance<T>(suites, prefix, tags, disabled, factory, factory, "." + name,
                                 conditions);
      }
      if (!index) throw std::logic_error("Suite parameter source returned no cases");
    }
  }
  if (!parameterized)
    append_suite_instance<T>(suites, prefix, tags, disabled, creator, fresh_creator, {},
                             conditions);
}
struct registered_case {
  std::string suite, file;
  std::function<execution_case()> create;
  std::function<std::shared_ptr<suite_hook_state>()> create_suite_hooks;
  std::type_index fixture_type = typeid(void);
};
inline std::vector<registered_case> registered_cases;
export inline void clear_registered_tests() { registered_cases.clear(); }
export template <typename Body>
  requires std::invocable<Body&>
void register_test(std::string suite, std::string name, Body body,
                   std::vector<std::string> tags = {},
                   std::source_location location = std::source_location::current()) {
  check_case_name(suite);
  check_case_name(name);
  auto shared_body = std::make_shared<Body>(std::move(body));
  registered_cases.push_back(
      {suite, location.file_name(), [name, shared_body, tags, location] {
         execution_case result{.name = name,
                               .base_name = name,
                               .file = location.file_name(),
                               .line = location.line(),
                               .tags = tags};
         result.run = [name, shared_body] {
           std::function<void()> callable = [shared_body] { std::invoke(*shared_body); };
           return execute_test(
               &callable, name, [](void* p) { (*static_cast<std::function<void()>*>(p))(); },
               nullptr, nullptr);
         };
         return result;
       }});
}
export template <typename Fixture, typename Factory, typename Body>
void register_test(std::string suite, std::string name, Factory factory, Body body,
                   std::vector<std::string> tags = {},
                   std::source_location location = std::source_location::current()) {
  check_case_name(suite);
  check_case_name(name);
  auto shared_factory = std::make_shared<Factory>(std::move(factory));
  auto shared_body = std::make_shared<Body>(std::move(body));
  registered_cases.push_back(
      {suite, location.file_name(),
       [name, shared_factory, shared_body, tags, location] {
         static constexpr auto description = get_tests<^^Fixture>();
         auto state = std::make_shared<fixture_state<Fixture>>();
         state->construct = [shared_factory](test_context& context) {
           if constexpr (std::is_invocable_v<Factory&, test_context&>)
             return std::invoke(*shared_factory, context);
           else
             return std::invoke(*shared_factory);
         };
         execution_case result{.name = name,
                               .base_name = name,
                               .file = location.file_name(),
                               .line = location.line(),
                               .tags = tags};
         result.set_suite_hooks = [state](std::shared_ptr<suite_hook_state> hooks) {
           state->suite_hooks = std::move(hooks);
         };
         result.run = [name, state, shared_body] {
           return execute_fixture<description.suite, Fixture>(
               *state, name,
               [shared_body](Fixture& fixture) { std::invoke(*shared_body, fixture); });
         };
         return result;
       },
       [suite, tags] {
         static constexpr auto description = get_tests<^^Fixture>();
         return make_suite_hooks<description.suite, Fixture>(suite, tags);
       },
       typeid(Fixture)});
}
void append_registered_tests(std::vector<execution_suite>& suites);
int report_runner_error(const std::exception& error);

export int run_registered_tests(int argc, char** argv);

int run_with_suites(int argc, char** argv, void (*discover)(std::vector<execution_suite>&));

export template <typename... Ts>
  requires(sizeof...(Ts) > 0 && (std::is_class_v<Ts> && ...))
int run_tests(int argc, char** argv) {
  return run_with_suites(argc, argv, [](std::vector<execution_suite>& suites) {
    (append_suite<Ts>(suites), ...);
  });
}

}  // namespace annotest
