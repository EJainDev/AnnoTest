export module annotest:sources;
import std;
import :structs;

export namespace annotest {
struct dynamic_case {
  std::string name;
  std::function<void()> body;
};
// RFC 4180 quoting, embedded newlines, and doubled quotes. Values remain strings;
// test providers decide how to convert each column to its domain type.
inline std::vector<named_case<std::vector<std::string>>> csv_cases(
    const std::filesystem::path& file, bool header = false) {
  std::ifstream stream(file, std::ios::binary);
  if (!stream) throw std::runtime_error("Cannot open CSV source");
  std::string text{std::istreambuf_iterator<char>(stream), {}};
  std::vector<named_case<std::vector<std::string>>> cases;
  std::vector<std::string> row;
  std::string field;
  bool quoted = false, closed = false, at_start = true, row_started = false;
  auto finish_row = [&] {
    row.push_back(std::move(field));
    field.clear();
    if (header)
      header = false;
    else
      cases.push_back(case_of(std::to_string(cases.size() + 1), std::move(row)));
    row.clear();
    at_start = true;
    closed = false;
    row_started = false;
  };
  for (std::size_t i = 0; i < text.size(); ++i) {
    char c = text[i];
    row_started = true;
    if (quoted) {
      if (c == '"') {
        if (i + 1 < text.size() && text[i + 1] == '"') {
          field += '"';
          ++i;
        } else {
          quoted = false;
          closed = true;
        }
      } else
        field += c;
    } else if (c == '"' && at_start && !closed) {
      quoted = true;
      at_start = false;
    } else if (c == ',') {
      row.push_back(std::move(field));
      field.clear();
      at_start = true;
      closed = false;
    } else if (c == '\n' || c == '\r') {
      if (c == '\r' && i + 1 < text.size() && text[i + 1] == '\n') ++i;
      finish_row();
    } else {
      if (closed || c == '"') throw std::runtime_error("Malformed CSV quoting");
      field += c;
      at_start = false;
    }
  }
  if (quoted) throw std::runtime_error("Unterminated CSV field");
  if (row_started || !row.empty()) finish_row();
  return cases;
}
inline std::vector<named_case<std::string>> line_cases(const std::filesystem::path& file) {
  std::ifstream stream(file);
  if (!stream) throw std::runtime_error("Cannot open line source");
  std::vector<named_case<std::string>> cases;
  for (std::string line; std::getline(stream, line);) {
    if (line.ends_with('\r')) line.pop_back();
    cases.push_back(case_of(std::to_string(cases.size() + 1), std::move(line)));
  }
  return cases;
}
}  // namespace annotest
