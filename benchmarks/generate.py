"""Generate equivalent regular-test translation units for both frameworks."""
import argparse
from pathlib import Path


def generate(framework, count, suites, workload="regular"):
    if workload == "parameterized":
        return generate_parameterized(framework, count, suites)
    suites = min(suites, count)
    lines = (["import annotest;", "import std;", "using namespace annotest;"]
             if framework == "annotest" else ["#include <gtest/gtest.h>", "#include <iostream>", "#include <string>"])
    lines.append("static int executed = 0;")
    for suite in range(suites):
        name = f"Bench{suite}"
        if framework == "annotest":
            lines += [f"struct {name} {{", "  int value = 0;",
                      "  [[=before_each{}]] void setup() { value = 1; }",
                      "  [[=after_each{}]] void teardown() { value = 0; }"]
        else:
            lines += [f"class {name} : public testing::Test {{", " protected:",
                      "  int value = 0;", "  void SetUp() override { value = 1; }",
                      "  void TearDown() override { value = 0; }", "};"]
        for case in range(suite, count, suites):
            payload = ["    std::cout << std::string(1024 * 1024, 'x');"] if workload == "output" else []
            if framework == "annotest":
                lines += [f"  [[=test{{}}]] void case_{case}() {{",
                          f"    ++executed; assert_equal(value + {case}, {case + 1});", *payload, "  }"]
            else:
                lines += [f"TEST_F({name}, Case{case}) {{",
                          f"  ++executed; ASSERT_EQ(value + {case}, {case + 1});", *payload, "}"]
        if framework == "annotest":
            lines.append("};")
    lines += ["int main(int argc, char** argv) {", "  int status = 0;"]
    if framework == "annotest":
        lines += [f"  status |= run_tests<Bench{suite}>(argc, argv);" for suite in range(suites)]
    else:
        lines += ["  testing::InitGoogleTest(&argc, argv);", "  status = RUN_ALL_TESTS();"]
    lines += [f"  return status || executed != {count};", "}"]
    return "\n".join(lines) + "\n"


def generate_parameterized(framework, count, suites):
    suites = min(suites, count)
    lines = (["import annotest;", "import std;", "using namespace annotest;"]
             if framework == "annotest" else ["#include <gtest/gtest.h>"])
    lines.append("static int executed = 0;")
    for suite in range(suites):
        name = f"bench_{suite}"
        values = list(range(suite, count, suites))
        if framework == "annotest":
            parameters = ", ".join(f"parameters({value})" for value in values)
            lines += [f"struct {name} {{", "  int value = 0;",
                      "  [[=before_each{}]] void setup() { value = 1; }",
                      "  [[=after_each{}]] void teardown() { value = 0; }",
                      f"  [[=test{{}}, =parameterize{{{parameters}}}]] void data(int input) {{",
                      "    ++executed; assert_equal(input + 1, input + value);", "  }", "};"]
        else:
            parameters = ", ".join(map(str, values))
            lines += [f"class {name} : public testing::TestWithParam<int> {{", " protected:",
                      "  int value = 0;", "  void SetUp() override { value = 1; }",
                      "  void TearDown() override { value = 0; }", "};",
                      f"TEST_P({name}, data) {{ ++executed; ASSERT_EQ(GetParam() + 1, GetParam() + value); }}",
                      f"INSTANTIATE_TEST_SUITE_P(inputs, {name}, testing::Values({parameters}));"]
    lines += ["int main(int argc, char** argv) {", "  int status = 0;"]
    if framework == "annotest":
        lines += [f"  status |= run_tests<bench_{suite}>(argc, argv);" for suite in range(suites)]
    else:
        lines += ["  testing::InitGoogleTest(&argc, argv);", "  status = RUN_ALL_TESTS();"]
    lines += [f"  return status || executed != {count};", "}"]
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--count", type=int, required=True)
    parser.add_argument("--suites", type=int, default=3)
    args = parser.parse_args()
    if args.count < 1 or args.suites < 1:
        parser.error("count and suites must be positive")
    args.output.mkdir(parents=True, exist_ok=True)
    for workload in ("regular", "parameterized", "output"):
        for framework in ("annotest", "gtest"):
            stem = framework if workload == "regular" else f"{framework}_{workload}"
            path = args.output / f"{stem}_{args.count}.cpp"
            contents = generate(framework, args.count, args.suites, workload)
            if not path.exists() or path.read_text() != contents:
                path.write_text(contents)


if __name__ == "__main__":
    main()
