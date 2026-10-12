export module annotest:matchers;
import std;
import :utils;
import :asserts;
import :expects;
import :logger;
import :exceptions;

export namespace annotest {
struct match_result {
  bool matched;
  std::string explanation;
  explicit operator bool() const { return matched; }
};
template <typename F>
struct matcher {
  F predicate;
  std::string description;
  template <typename T>
  match_result operator()(const T& value) const {
    auto result = std::invoke(predicate, value);
    if constexpr (std::same_as<decltype(result), match_result>)
      return result;
    else
      return {static_cast<bool>(result), description};
  }
};
template <typename F>
auto satisfies(F predicate, std::string description = "predicate satisfied") {
  return matcher<F>{std::move(predicate), std::move(description)};
}
template <typename T>
auto eq(T expected) {
  return satisfies(
      [expected = std::move(expected)](const auto& actual) {
        return match_result{static_cast<bool>(actual == expected),
                            "Expected " + print_value(expected) + ", got " + print_value(actual) +
                                difference(expected, actual)};
      },
      "equal");
}
template <typename T>
auto ne(T value) {
  return satisfies([value = std::move(value)](const auto& actual) { return actual != value; },
                   "not equal");
}
template <typename T>
auto lt(T value) {
  return satisfies([value = std::move(value)](const auto& actual) { return actual < value; },
                   "less than");
}
template <typename T>
auto le(T value) {
  return satisfies([value = std::move(value)](const auto& actual) { return actual <= value; },
                   "less than or equal");
}
template <typename T>
auto gt(T value) {
  return satisfies([value = std::move(value)](const auto& actual) { return actual > value; },
                   "greater than");
}
template <typename T>
auto ge(T value) {
  return satisfies([value = std::move(value)](const auto& actual) { return actual >= value; },
                   "greater than or equal");
}
inline auto anything() {
  return satisfies([](const auto&) { return true; }, "any value");
}
inline auto is_null() {
  return satisfies([](const auto& p) { return p == nullptr; }, "null pointer");
}
inline auto not_null() {
  return satisfies([](const auto& p) { return p != nullptr; }, "non-null pointer");
}
inline auto is_true() {
  return satisfies([](const auto& p) { return static_cast<bool>(p); }, "true");
}
inline auto is_false() {
  return satisfies([](const auto& p) { return !static_cast<bool>(p); }, "false");
}
inline auto is_empty() {
  return satisfies([](const auto& p) { return std::ranges::empty(p); }, "empty");
}
template <typename M>
auto negate(M inner) {
  return satisfies([inner = std::move(inner)](const auto& value) { return !inner(value).matched; },
                   "negated matcher");
}
template <typename... Ms>
auto all_of(Ms... inners) {
  return satisfies(
      [inners = std::tuple{std::move(inners)...}](const auto& value) {
        match_result result{true, {}};
        std::apply(
            [&](const auto&... m) {
              (([&] {
                 if (result.matched) {
                   auto r = m(value);
                   if (!r.matched) result = r;
                 }
               }()),
               ...);
            },
            inners);
        return result;
      },
      "all matchers");
}
template <typename... Ms>
auto any_of(Ms... inners) {
  return satisfies(
      [inners = std::tuple{std::move(inners)...}](const auto& value) {
        bool matched = false;
        std::string detail;
        std::apply(
            [&](const auto&... m) {
              (([&] {
                 if (!matched) {
                   auto r = m(value);
                   matched = r.matched;
                   if (!matched) detail += r.explanation + "; ";
                 }
               }()),
               ...);
            },
            inners);
        return match_result{matched, detail};
      },
      "any matcher");
}
inline auto starts_with(std::string prefix) {
  return satisfies([prefix = std::move(prefix)](
                       const auto& value) { return std::string_view(value).starts_with(prefix); },
                   "starts with prefix");
}
inline auto ends_with(std::string suffix) {
  return satisfies([suffix = std::move(suffix)](
                       const auto& value) { return std::string_view(value).ends_with(suffix); },
                   "ends with suffix");
}
inline auto has_substr(std::string text) {
  return satisfies(
      [text = std::move(text)](const auto& value) {
        return std::string_view(value).find(text) != std::string_view::npos;
      },
      "contains substring");
}
inline auto matches_regex(std::string pattern) {
  return satisfies(
      [pattern = std::regex(pattern)](const auto& value) {
        auto text = std::string(value);
        return std::regex_match(text, pattern);
      },
      "matches regex");
}
inline auto contains_regex(std::string pattern) {
  return satisfies(
      [pattern = std::regex(pattern)](const auto& value) {
        auto text = std::string(value);
        return std::regex_search(text, pattern);
      },
      "contains regex");
}
template <typename M>
auto contains(M inner) {
  return satisfies(
      [inner = std::move(inner)](const auto& values) {
        return std::ranges::any_of(values, [&](const auto& value) { return inner(value).matched; });
      },
      "contains matching element");
}
template <typename M>
auto each(M inner) {
  return satisfies(
      [inner = std::move(inner)](const auto& values) {
        std::size_t index = 0;
        for (const auto& value : values) {
          auto result = inner(value);
          if (!result.matched)
            return match_result{false, std::format("Element {}: {}", index, result.explanation)};
          ++index;
        }
        return match_result{true, {}};
      },
      "each element matches");
}
template <typename M>
auto size_is(M inner) {
  return satisfies(
      [inner = std::move(inner)](const auto& values) { return inner(std::ranges::size(values)); },
      "size matches");
}
template <typename... Ms>
auto elements_are(Ms... inners) {
  return satisfies(
      [inners = std::tuple{std::move(inners)...}](const auto& values) {
        auto it = std::ranges::begin(values), end = std::ranges::end(values);
        std::size_t index = 0;
        match_result result{true, {}};
        std::apply(
            [&](const auto&... m) {
              (([&] {
                 if (!result.matched) return;
                 if (it == end) {
                   result = {false, "Missing element at index " + std::to_string(index)};
                   return;
                 }
                 auto item = m(*it++);
                 if (!item.matched)
                   result = {false, std::format("Element {}: {}", index, item.explanation)};
                 ++index;
               }()),
               ...);
            },
            inners);
        if (result.matched && it != end) result = {false, "Unexpected extra elements"};
        return result;
      },
      "ordered elements match");
}
template <typename... Ms>
auto unordered_elements_are(Ms... inners) {
  return satisfies(
      [inners = std::tuple{std::move(inners)...}](const auto& values) {
        constexpr auto n = sizeof...(Ms);
        std::vector<std::vector<bool>> edges;
        for (const auto& value : values) {
          std::vector<bool> row;
          std::apply([&](const auto&... m) { (row.push_back(m(value).matched), ...); }, inners);
          edges.push_back(std::move(row));
        }
        if (edges.size() != n) return false;
        std::vector<int> owner(n, -1);
        std::function<bool(int, std::vector<bool>&)> augment = [&](int item,
                                                                   std::vector<bool>& seen) {
          for (std::size_t j = 0; j < n; ++j)
            if (edges[item][j] && !seen[j]) {
              seen[j] = true;
              if (owner[j] < 0 || augment(owner[j], seen)) {
                owner[j] = item;
                return true;
              }
            }
          return false;
        };
        for (std::size_t i = 0; i < n; ++i) {
          std::vector<bool> seen(n, false);
          if (!augment(i, seen)) return false;
        }
        return true;
      },
      "unordered elements match one-to-one");
}
template <typename Member, typename M>
auto field(Member member, M inner) {
  return satisfies([member, inner = std::move(inner)](
                       const auto& object) { return inner(std::invoke(member, object)); },
                   "field/property matches");
}
template <typename Member, typename M>
auto property(Member member, M inner) {
  return field(member, std::move(inner));
}
template <typename M>
auto pointee(M inner) {
  return satisfies([inner = std::move(inner)](
                       const auto& pointer) { return pointer && inner(*pointer).matched; },
                   "pointee matches");
}
template <typename M>
auto optional_value(M inner) {
  return satisfies(
      [inner = std::move(inner)](const auto& value) { return value && inner(*value).matched; },
      "optional value matches");
}
template <typename T, typename M>
auto variant_with(M inner) {
  return satisfies(
      [inner = std::move(inner)](const auto& value) {
        auto p = std::get_if<T>(&value);
        return p && inner(*p).matched;
      },
      "variant alternative matches");
}
template <typename T>
auto near(T expected, T absolute, T relative = T{}) {
  if (absolute < T{} || relative < T{}) throw std::invalid_argument("Negative numeric tolerance");
  return satisfies(
      [=](auto value) {
        return value == expected ||
               std::abs(value - expected) <=
                   std::max(absolute, relative * std::max(std::abs(value), std::abs(expected)));
      },
      "within numeric tolerance");
}
template <std::floating_point T>
auto ulp_near(T expected, unsigned distance = 4) {
  return satisfies(
      [=](T value) {
        if (value == expected) return true;
        if (!std::isfinite(value) || !std::isfinite(expected)) return false;
        T cursor = expected;
        for (unsigned i = 0; i < distance; ++i) {
          cursor = std::nextafter(cursor, value);
          if (cursor == value) return true;
        }
        return false;
      },
      "within ULP distance");
}
inline auto is_nan() {
  return satisfies([](auto value) { return std::isnan(value); }, "NaN");
}
template <typename E>
auto throws() {
  return satisfies(
      [](const auto& callable) {
        try {
          std::invoke(callable);
        } catch (const skip_error&) {
          throw;
        } catch (const E&) {
          return true;
        } catch (...) {
          return false;
        }
        return false;
      },
      "throws expected exception");
}
template <typename E, typename M>
auto throws(M inner) {
  return satisfies(
      [inner = std::move(inner)](const auto& callable) {
        try {
          std::invoke(callable);
        } catch (const skip_error&) {
          throw;
        } catch (const E& e) {
          return inner(e).matched;
        } catch (...) {
          return false;
        }
        return false;
      },
      "exception matches");
}
inline auto does_not_throw() {
  return satisfies(
      [](const auto& callable) {
        try {
          std::invoke(callable);
          return true;
        } catch (const skip_error&) {
          throw;
        } catch (...) {
          return false;
        }
      },
      "does not throw");
}
inline std::string matcher_diagnostic(std::string_view explanation, std::string_view message) {
  std::string result = "Matcher failed: " + std::string(explanation);
  if (!message.empty()) result += "\n" + std::string(message);
  return result;
}
template <typename T, typename M>
void assert_that(const T& value, const M& match, std::string_view message = {},
                 std::source_location location = std::source_location::current()) {
  auto result = match(value);
  if (!result.matched) add_failure(matcher_diagnostic(result.explanation, message), location);
}
template <typename T, typename M>
void expect_that(const T& value, const M& match, std::string_view message = {},
                 std::source_location location = std::source_location::current()) {
  auto result = match(value);
  if (!result.matched)
    add_failure_expect(matcher_diagnostic(result.explanation, message), location);
}
inline void fail(std::string_view message = "Explicit failure",
                 std::source_location location = std::source_location::current()) {
  add_failure(message, location);
}
inline void add_failure_message(std::string_view message,
                                std::source_location location = std::source_location::current()) {
  add_failure_expect(message, location);
}
inline void assert_no_throw(auto callable,
                            std::source_location location = std::source_location::current()) {
  auto result = does_not_throw()(callable);
  if (!result.matched) add_failure("Unexpected exception", location);
}
inline void expect_no_throw(auto callable,
                            std::source_location location = std::source_location::current()) {
  auto result = does_not_throw()(callable);
  if (!result.matched) add_failure_expect("Unexpected exception", location);
}
template <typename F>
void expect_failure(F&& callable, failure_types type, std::string_view substring = {},
                    std::source_location location = std::source_location::current()) {
  auto failures = capture_failures(std::forward<F>(callable));
  if (!std::ranges::any_of(failures, [&](const auto& f) {
        return f.type == type && f.message.find(substring) != std::string::npos;
      }))
    add_failure_expect("Expected assertion failure was not produced", location);
}
template <typename F>
void assert_failure(F&& callable, failure_types type, std::string_view substring = {},
                    std::source_location location = std::source_location::current()) {
  auto failures = capture_failures(std::forward<F>(callable));
  if (!std::ranges::any_of(failures, [&](const auto& f) {
        return f.type == type && f.message.find(substring) != std::string::npos;
      }))
    add_failure("Expected assertion failure was not produced", location);
}
template <typename F>
void expect_no_fatal_failure(F&& callable,
                             std::source_location location = std::source_location::current()) {
  bool failed = false;
  try {
    std::invoke(std::forward<F>(callable));
  } catch (const assertion_error&) {
    failed = true;
  }
  if (failed) add_failure_expect("Unexpected fatal assertion failure", location);
}
template <typename F>
void assert_no_fatal_failure(F&& callable,
                             std::source_location location = std::source_location::current()) {
  bool failed = false;
  try {
    std::invoke(std::forward<F>(callable));
  } catch (const assertion_error&) {
    failed = true;
  }
  if (failed) add_failure("Unexpected fatal assertion failure", location);
}
template <typename F>
void assert_no_throw(F&& callable, std::string_view message,
                     std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  assert_no_throw(std::forward<F>(callable), location);
}
template <typename F>
void expect_no_throw(F&& callable, std::string_view message,
                     std::source_location location = std::source_location::current()) {
  scoped_message context{std::string(message), location};
  expect_no_throw(std::forward<F>(callable), location);
}

inline void run_assertion_block(const std::function<void()>& block) {
  try {
    block();
  } catch (const skip_error&) {
    throw;
  } catch (const assertion_error& error) {
    if (!error.recorded())
      suite_log.add_failure(
          {failure_types::assertion, error.message(), {.message = error.message()}});
  } catch (const std::exception& error) {
    auto message = "Grouped assertion: " + std::string(error.what());
    suite_log.add_failure({failure_types::uncaught_exception, message, {.message = message}});
  } catch (...) {
    const std::string message = "Grouped assertion: unknown exception";
    suite_log.add_failure({failure_types::unknown, message, {.message = message}});
  }
}
template <typename... Fs>
bool run_assertion_group(std::string_view name, Fs&&... blocks) {
  scoped_trace trace("Assertion group: " + std::string(name));
  auto before = active_failure_count();
  // The wrapper borrows each callable for this synchronous invocation, so
  // move-only blocks work without duplicating exception dispatch per type.
  (run_assertion_block([&] { std::invoke(std::forward<Fs>(blocks)); }), ...);
  return active_failure_count() != before;
}
template <typename... Fs>
  requires(std::invocable<Fs> && ...)
void assert_all(std::string_view name, Fs&&... blocks) {
  if (run_assertion_group(name, std::forward<Fs>(blocks)...))
    throw assertion_error{"Grouped assertions failed", true};
}
template <typename... Fs>
  requires(std::invocable<Fs> && ...)
void assert_all(Fs&&... blocks) {
  assert_all("", std::forward<Fs>(blocks)...);
}
template <typename... Fs>
  requires(std::invocable<Fs> && ...)
void expect_all(std::string_view name, Fs&&... blocks) {
  run_assertion_group(name, std::forward<Fs>(blocks)...);
}
template <typename... Fs>
  requires(std::invocable<Fs> && ...)
void expect_all(Fs&&... blocks) {
  expect_all("", std::forward<Fs>(blocks)...);
}
}  // namespace annotest
