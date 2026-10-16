Runtime sources and dynamic cases
=================================

.. cpp:namespace:: annotest

.. cpp:struct:: template <typename... Ts> named_case

   One named runtime argument set.

.. cpp:member:: template <typename... Ts> std::string named_case<Ts...>::name

   Case suffix. Must be nonempty, unique within its provider, and contain no tabs or line breaks.

.. cpp:member:: template <typename... Ts> std::tuple<Ts...> named_case<Ts...>::values

   Arguments passed to the method or suite constructor.

.. cpp:function:: template <typename... Ts> auto case_of(std::string name, Ts&&... values)

   Create a named runtime argument set, decaying and storing each supplied value.

   :param name: Concrete case suffix.
   :param values: Arguments for one invocation.
   :returns: named_case<std::decay_t<Ts>...>.

.. cpp:struct:: dynamic_case

   One factory-produced child case.

.. cpp:member:: std::string dynamic_case::name

   Stable child name; must be nonempty and unique within its factory, with no tabs or line breaks.

.. cpp:member:: std::function<void()> dynamic_case::body

   Nullary child body. A factory fixture remains alive while its children execute.

.. cpp:function:: std::vector<named_case<std::vector<std::string>>> csv_cases(const std::filesystem::path& file, bool header = false)

   Read CSV input into one named case per data row. Fields remain strings. Supports quoted fields, embedded newlines, doubled quotes, and LF/CRLF/CR line endings.

   :param file: Input path.
   :param header: Discard the first row when true.
   :returns: Cases named 1, 2, …, each containing one vector of column strings.
   :throws std::runtime_error: The file cannot be opened or quoting is malformed.

   An empty file produces no cases; parameter_source rejects an empty case set. No column-to-domain conversion is performed.

.. cpp:function:: std::vector<named_case<std::string>> line_cases(const std::filesystem::path& file)

   Read one case per line, stripping the line terminator and a trailing carriage return. Blank lines remain empty-string cases.

   :param file: Input path.
   :returns: Cases named 1, 2, …, each containing one string argument.
   :throws std::runtime_error: The file cannot be opened.

Use :cpp:struct:`parameter_source` to attach a provider and
:cpp:struct:`test_factory` to generate dynamic children. Both execute during
discovery and must produce stable names and data across subprocess replay.
For a procedure, see :doc:`/examples/runtime_sources`.
