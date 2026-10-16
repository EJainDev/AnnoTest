module;
#include <version>

#include "annotest_platform.h"

export module annotest:diagnostics;
import std;

namespace annotest {
#if defined(__cpp_lib_stacktrace) && !defined(ANNOTEST_NATIVE_STACKTRACE)
using stacktrace_type = std::stacktrace;
inline auto capture_stacktrace() { return std::stacktrace::current(); }
#else
using stacktrace_type = std::string;
inline auto capture_stacktrace() { return annotest_platform::capture_stacktrace(); }
#endif
}  // namespace annotest
