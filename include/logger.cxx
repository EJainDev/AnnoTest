export module annotest:logger;

import std;
import :diagnostics;

import :exceptions;

import serial_xml;

namespace xml = serial_xml;

namespace annotest {
export enum class failure_types {
  assertion,
  expectation,
  contract_violation,
  uncaught_exception,
  unknown
};

// XML 1.0 accepts Unicode scalar values, but arbitrary captured bytes need
// a visible representation. Preserve valid UTF-8 and escape each rejected byte.
std::string xml_text(std::string_view text) {
  constexpr std::string_view hex = "0123456789abcdef";
  std::string result;
  result.reserve(text.size());
  auto escape_byte = [&](unsigned char byte) {
    result += "\\x";
    result += hex[byte >> 4];
    result += hex[byte & 15];
  };
  for (std::size_t i = 0; i < text.size();) {
    auto first = static_cast<unsigned char>(text[i]);
    std::size_t length = 1;
    unsigned int value = first;
    unsigned int minimum = 0;
    if (first >= 0xc2 && first <= 0xdf) {
      length = 2;
      value = first & 0x1f;
      minimum = 0x80;
    } else if (first >= 0xe0 && first <= 0xef) {
      length = 3;
      value = first & 0x0f;
      minimum = 0x800;
    } else if (first >= 0xf0 && first <= 0xf4) {
      length = 4;
      value = first & 0x07;
      minimum = 0x10000;
    } else if (first >= 0x80) {
      escape_byte(first);
      ++i;
      continue;
    }
    bool valid = length <= text.size() - i;
    if (valid) {
      for (std::size_t j = 1; j < length; ++j) {
        auto byte = static_cast<unsigned char>(text[i + j]);
        if ((byte & 0xc0) != 0x80) {
          valid = false;
          break;
        }
        value = (value << 6) | (byte & 0x3f);
      }
    }
    valid = valid && value >= minimum && value <= 0x10ffff && !(value >= 0xd800 && value <= 0xdfff);
    const bool xml_character =
        value == 9 || value == 10 || value == 13 || (value >= 0x20 && value <= 0xd7ff) ||
        (value >= 0xe000 && value <= 0xfffd) || (value >= 0x10000 && value <= 0x10ffff);
    if (valid && xml_character) {
      result.append(text.substr(i, length));
      i += length;
    } else {
      escape_byte(first);
      ++i;
    }
  }
  return result;
}

struct failure_cdata {
  std::optional<std::pair<std::string, std::size_t>> location;
  std::string message;
  std::optional<stacktrace_type> stacktrace = capture_stacktrace();
  bool sanitize_xml = false;
  std::optional<std::string> decoded_stacktrace;
};

struct failure_xml_log {
  [[= xml::attribute]] failure_types type;
  [[= xml::attribute]] std::string message;
  // SerialXML escapes raw text, including embedded CDATA terminators.
  [[ = xml::raw, = xml::no_unpack ]] failure_cdata cdata;
  [[ = xml::attribute, = xml::exclude_on_empty ]] std::string phase;
};

struct skipped_xml_log {
  [[= xml::attribute]] std::string message;
};
struct property_xml_log {
  [[= xml::attribute]] std::string name;
  [[= xml::attribute]] std::string value;
};
struct test_xml_log {
  [[ = xml::attribute, = xml::name{"classname"} ]] std::string test_class;
  [[= xml::attribute]] std::string name;
  [[= xml::attribute]] double time;
  [[= xml::attribute]] std::string file;
  [[= xml::attribute]] std::size_t line = 0;
  [[= xml::skip]] bool failed = false;
  [[= xml::attribute]] unsigned int errors;

  [[= xml::name{"system-out"}]] std::string sysout;
  [[= xml::name{"system-err"}]] std::string syserr;

  [[ = xml::raw, = xml::iter{"failure", "failures"},
     = xml::exclude_on_empty ]] std::vector<failure_xml_log>
      failures;
  [[ = xml::raw, = xml::iter{"skipped", "skipped"},
     = xml::exclude_on_empty ]] std::vector<skipped_xml_log>
      skip_entries;
  [[ = xml::iter{"property", "properties"}, = xml::exclude_on_empty ]] std::vector<property_xml_log>
      properties;
};

export enum class test_status { passed, failed, skipped };
export struct failure_result {
  failure_types type;
  std::string message, phase;
  std::optional<std::pair<std::string, std::size_t>> location;
};
inline thread_local std::vector<failure_result>* intercepted_failures = nullptr;
inline std::vector<failure_result>* all_thread_interception = nullptr;
inline std::mutex interception_mutex;
inline bool break_on_failure = false;
void debugger_break();
export struct test_result {
  std::string suite, name;
  test_status status = test_status::passed;
  double seconds = 0;
  std::string skip_reason;
  std::size_t failures = 0;
  std::vector<failure_result> details;
  std::vector<std::pair<std::string, std::string>> properties;
  std::string standard_output, standard_error;
  std::string file, parameters;
  std::size_t line = 0;
  std::vector<std::string> tags;
};
export struct assertion_event {
  std::string message, phase;
  std::optional<std::pair<std::string, std::size_t>> location;
};
export struct test_info {
  std::string suite, name, file, parameters;
  std::size_t line = 0;
  std::vector<std::string> tags;
  bool disabled = false;
};
export struct suite_result {
  std::string name;
  unsigned tests = 0, failed = 0, skipped = 0, errors = 0;
  double seconds = 0;
};
export struct test_listener {
  virtual ~test_listener() = default;
  virtual void on_discovery(std::string_view, std::string_view) {}
  virtual void on_test_discovered(const test_info&) {}
  virtual void on_suite_result(const suite_result&) {}
  virtual void on_suite_start(std::string_view) {}
  virtual void on_suite_end(std::string_view) {}
  virtual void on_test_start(std::string_view, std::string_view) {}
  virtual void on_assertion(const assertion_event&) {}
  virtual void on_test_end(const test_result&) {}
  virtual void on_run_end(std::span<const test_result>) {}
  virtual void before_test() {}
  virtual void after_test() {}
};
inline std::vector<std::shared_ptr<test_listener>> listeners;
inline std::unordered_map<test_listener*, std::string> listener_scopes;
inline std::vector<std::shared_ptr<test_listener>> selected_listeners;
inline bool listener_selection_active = false;
inline std::string observer_suite;
struct observer_failure {
  std::string suite, phase, message;
};
inline std::vector<observer_failure> observer_failures;
export inline void add_listener(std::shared_ptr<test_listener> listener,
                                std::string suite_scope = {}) {
  if (!listener) throw std::invalid_argument("Null test listener");
  listener_scopes[listener.get()] = std::move(suite_scope);
  listeners.push_back(std::move(listener));
}
export inline void clear_listeners() {
  listeners.clear();
  listener_scopes.clear();
}
inline auto listener_snapshot() {
  return listener_selection_active ? selected_listeners : listeners;
}
inline void select_listeners(std::string_view suite) {
  observer_suite = suite;
  listener_selection_active = true;
  selected_listeners.clear();
  for (const auto& listener : listeners) {
    const auto& scope = listener_scopes[listener.get()];
    if (scope.empty() || suite == scope || suite.starts_with(scope + "."))
      selected_listeners.push_back(listener);
  }
}
inline std::recursive_mutex observer_mutex;
template <typename Callback>
void notify_listeners(std::string_view phase, Callback callback) {
  std::lock_guard lock(observer_mutex);
  for (const auto& listener : listener_snapshot()) {
    try {
      callback(*listener);
    } catch (const std::exception& error) {
      observer_failures.push_back({observer_suite, std::string(phase), error.what()});
    } catch (...) {
      observer_failures.push_back(
          {observer_suite, std::string(phase), "Unknown listener exception"});
    }
  }
}
inline bool quiet_execution = false;
inline std::string current_phase;
inline thread_local std::vector<std::string> traces;
export class scoped_trace {
 public:
  explicit scoped_trace(std::string message,
                        std::source_location location = std::source_location::current()) {
    traces.push_back(std::format("{}:{}: {}", location.file_name(), location.line(), message));
  }
  scoped_trace(const scoped_trace&) = delete;
  scoped_trace& operator=(const scoped_trace&) = delete;
  ~scoped_trace() { traces.pop_back(); }
};
export using scoped_message = scoped_trace;

export class temporary_directory {
 public:
  temporary_directory() {
    auto base = std::filesystem::temp_directory_path();
    std::random_device random;
    for (int attempt = 0; attempt < 100; ++attempt) {
      directory_ = base / std::format("annotest-{:08x}-{:08x}", random(), random());
      if (std::filesystem::create_directory(directory_)) return;
    }
    throw std::runtime_error("Cannot create unique temporary directory");
  }
  temporary_directory(const temporary_directory&) = delete;
  temporary_directory& operator=(const temporary_directory&) = delete;
  ~temporary_directory() {
    std::error_code error;
    std::filesystem::remove_all(directory_, error);
  }
  const std::filesystem::path& path() const { return directory_; }

 private:
  std::filesystem::path directory_;
};
export class resource_scope {
  struct entry {
    std::string key;
    std::type_index type;
    std::shared_ptr<void> value;
  };
  std::vector<entry> entries;

 public:
  resource_scope() = default;
  resource_scope(const resource_scope&) = delete;
  resource_scope& operator=(const resource_scope&) = delete;
  ~resource_scope() { clear(); }
  void clear() {
    while (!entries.empty()) entries.pop_back();
  }
  template <typename T, typename Factory>
  std::shared_ptr<T> get_or_create(std::string key, Factory factory) {
    for (auto& entry : entries)
      if (entry.key == key) {
        if (entry.type != typeid(T)) throw std::logic_error("Resource key has a different type");
        return std::static_pointer_cast<T>(entry.value);
      }
    std::shared_ptr<T> value = std::invoke(factory);
    if (!value) throw std::invalid_argument("Null scoped resource");
    entries.push_back({std::move(key), typeid(T), value});
    return value;
  }
};
export struct test_context {
  std::string suite, name, parameters;
  std::vector<std::string> tags;
  unsigned repetition = 0;
  std::unique_ptr<temporary_directory> directory;
  resource_scope resources;
  const std::filesystem::path& temp_dir() {
    if (!directory) directory = std::make_unique<temporary_directory>();
    return directory->path();
  }
};
inline test_context* active_context = nullptr;
export inline test_context& current_test() {
  if (!active_context) throw std::logic_error("No active test");
  return *active_context;
}
export inline void record_property(std::string name, std::string value);

inline std::mutex failures_mtx;

std::string xml_path(const std::filesystem::path& path) {
  const auto bytes = path.u8string();
  return xml_text(std::string(bytes.begin(), bytes.end()));
}

struct[[= xml::name{"testsuite"}]] suite_xml_log {
  [[= xml::attribute]] std::string name;
  [[ = xml::attribute, = xml::format{xml_path} ]] std::filesystem::path file;
  [[ = xml::attribute, = xml::name{"tests"} ]] unsigned int total_tests;
  [[ = xml::attribute, = xml::name{"failures"} ]] unsigned int failed = 0;
  [[= xml::attribute]] unsigned int skipped = 0;
  [[= xml::attribute]] unsigned int errors = 0;
  [[= xml::attribute]] double time = 0.0;
  [[= xml::attribute]] std::string timestamp;

  [[ = xml::raw, = xml::iter{"testcase", "testcases"},
     = xml::exclude_on_empty ]] std::vector<test_xml_log>
      testcases;

  void add_failure(const failure_xml_log& log) {
    auto failure = log;
    failure.phase = current_phase;
    for (const auto& trace : traces) {
      failure.message += "\nTrace: " + trace;
      failure.cdata.message += "\nTrace: " + trace;
    }
    {
      std::lock_guard lock(interception_mutex);
      auto* target = intercepted_failures ? intercepted_failures : all_thread_interception;
      if (target) {
        target->push_back({failure.type, failure.message, failure.phase, failure.cdata.location});
        return;
      }
    }
    if (break_on_failure) debugger_break();
    {
      std::lock_guard lock(failures_mtx);
      if (testcases.empty()) return;
      testcases.back().failures.emplace_back(failure);
    }
    static thread_local bool notifying = false;
    if (notifying) return;
    notifying = true;
    struct guard {
      bool& flag;
      ~guard() { flag = false; }
    } cleanup{notifying};
    notify_listeners("on_assertion", [&](test_listener& listener) {
      listener.on_assertion({failure.message, failure.phase, failure.cdata.location});
    });
  }

  void reset() {
    std::lock_guard lock(failures_mtx);
    name.clear();
    file.clear();
    total_tests = failed = skipped = errors = 0;
    time = 0;
    timestamp.clear();
    testcases.clear();
  }
};

inline suite_xml_log suite_log;
export inline std::vector<failure_result> current_failures() {
  std::lock_guard lock(failures_mtx);
  std::vector<failure_result> result;
  if (!suite_log.testcases.empty())
    for (const auto& failure : suite_log.testcases.back().failures)
      result.push_back({failure.type, failure.message, failure.phase, failure.cdata.location});
  return result;
}
inline std::size_t active_failure_count() {
  {
    std::lock_guard lock(interception_mutex);
    auto* captured = intercepted_failures ? intercepted_failures : all_thread_interception;
    if (captured) return captured->size();
  }
  std::lock_guard lock(failures_mtx);
  return suite_log.testcases.empty() ? 0 : suite_log.testcases.back().failures.size();
}
export inline bool has_failure() { return !current_failures().empty(); }
export inline bool has_fatal_failure() {
  return std::ranges::any_of(current_failures(),
                             [](const auto& f) { return f.type == failure_types::assertion; });
}
export inline bool has_nonfatal_failure() {
  return std::ranges::any_of(current_failures(),
                             [](const auto& f) { return f.type == failure_types::expectation; });
}
export template <typename F>
std::vector<failure_result> capture_failures(F&& callable, bool all_threads = false) {
  std::vector<failure_result> captured;
  auto* previous = intercepted_failures;
  if (all_threads) {
    std::lock_guard lock(interception_mutex);
    if (all_thread_interception)
      throw std::logic_error("An all-thread failure capture is already active");
    all_thread_interception = &captured;
  }
  intercepted_failures = &captured;
  struct guard {
    std::vector<failure_result>* previous;
    bool all_threads;
    ~guard() {
      std::lock_guard lock(interception_mutex);
      if (all_threads) all_thread_interception = nullptr;
      intercepted_failures = previous;
    }
  } restore{previous, all_threads};
  try {
    std::invoke(std::forward<F>(callable));
  } catch (const assertion_error&) {
  }
  return captured;
}
export inline std::jthread test_thread(std::function<void()> body) {
  return std::jthread([body = std::move(body)] {
    try {
      body();
    } catch (const assertion_error&) {
    } catch (const std::exception& error) {
      suite_log.add_failure(
          {failure_types::uncaught_exception, error.what(), {.message = error.what()}});
    } catch (...) {
      suite_log.add_failure({failure_types::unknown,
                             "Unknown helper-thread exception",
                             {.message = "Unknown helper-thread exception"}});
    }
  });
}
export inline void record_property(std::string name, std::string value) {
  if (!active_context || suite_log.testcases.empty()) throw std::logic_error("No active test");
  suite_log.testcases.back().properties.push_back({std::move(name), std::move(value)});
}

}  // namespace annotest

namespace std {
template <>
struct formatter<annotest::failure_cdata> : formatter<std::string> {
  constexpr auto format(const annotest::failure_cdata& cdata, auto& ctx) const {
    std::string formatted_cdata;

    if (cdata.location.has_value()) {
      formatted_cdata +=
          std::format("Location: {}:{}\n", cdata.location->first, cdata.location->second);
    }

    formatted_cdata += std::format("Message: {}\n", cdata.message);

    if (cdata.decoded_stacktrace) {
      formatted_cdata += "Stacktrace:\n" + *cdata.decoded_stacktrace + "\n";
    } else if (cdata.stacktrace.has_value()) {
      formatted_cdata += std::format("Stacktrace:\n{}\n", cdata.stacktrace.value());
    }

    if (cdata.sanitize_xml) formatted_cdata = annotest::xml_text(formatted_cdata);
    return formatter<std::string>::format(formatted_cdata, ctx);
  }
};

template <>
struct formatter<annotest::failure_types> : formatter<std::string> {
  constexpr auto format(const annotest::failure_types& type, auto& ctx) const {
    std::string type_str;
    switch (type) {
      case annotest::failure_types::assertion:
        type_str = "assertion";
        break;
      case annotest::failure_types::expectation:
        type_str = "expectation";
        break;
      case annotest::failure_types::contract_violation:
        type_str = "contract_violation";
        break;
      case annotest::failure_types::uncaught_exception:
        type_str = "uncaught_exception";
        break;
      case annotest::failure_types::unknown:
        type_str = "unknown";
        break;
    }
    return formatter<std::string>::format(type_str, ctx);
  }
};
}  // namespace std
namespace annotest {
std::string xml_report() {
  auto report = suite_log;
  report.name = xml_text(report.name);
  report.timestamp = xml_text(report.timestamp);
  for (auto& test : report.testcases) {
    test.test_class = xml_text(test.test_class);
    test.name = xml_text(test.name);
    test.sysout = xml_text(test.sysout);
    test.syserr = xml_text(test.syserr);
    for (auto& entry : test.skip_entries) entry.message = xml_text(entry.message);
    for (auto& entry : test.properties) {
      entry.name = xml_text(entry.name);
      entry.value = xml_text(entry.value);
    }
    for (auto& failure : test.failures) {
      failure.message = xml_text(failure.message);
      failure.cdata.sanitize_xml = true;
    }
  }
  return xml::to_xml(report);
}
}  // namespace annotest
