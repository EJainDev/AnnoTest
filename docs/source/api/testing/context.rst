Test context and resources
==========================

.. cpp:namespace:: annotest

Contexts provide metadata and resource ownership for an invocation, fixture
constructor, or suite hook. Contexts are injected as ``test_context&`` into
supported methods, hooks, and constructors.

.. cpp:struct:: test_context

   Metadata and resources for a test scope. Constructor contexts remain valid through fixture destruction; suite-hook contexts remain valid through after_all.

.. cpp:member:: std::string test_context::suite

   Full suite name, including enclosing names and parameter suffixes.

.. cpp:member:: std::string test_context::name

   Case name within its suite; _suite for suite hooks.

.. cpp:member:: std::string test_context::parameters

   Formatted parameter description, when available.

.. cpp:member:: std::vector<std::string> test_context::tags

   Case labels, including inherited suite tags.

.. cpp:member:: unsigned test_context::repetition = 0

   Zero-based combined invocation index across annotation and CLI repetitions.

.. cpp:member:: std::unique_ptr<temporary_directory> test_context::directory

   Lazily initialized temporary-directory owner. Prefer temp_dir() for access.

.. cpp:member:: resource_scope test_context::resources

   Resource store for this context.

.. cpp:function:: const std::filesystem::path& test_context::temp_dir()

   Create a unique temporary directory on first access; subsequent accesses return the same path.

   :returns: Borrowed path, valid until context cleanup.
   :throws std::filesystem::filesystem_error: The filesystem operation fails.
   :throws std::runtime_error: A unique directory could not be created.

   Invocation cleanup removes the directory after extension cleanup. Constructor contexts retain it until fixture destruction.

.. cpp:function:: test_context& current_test()

   Access the active invocation context. This may differ from a fixture's constructor context. Shared constructor contexts have their identity refreshed before each case.

   :returns: Reference to the active test_context.
   :throws std::logic_error: No case is active.

.. cpp:function:: void record_property(std::string name, std::string value)

   Append a report property to the active case. Duplicate names are retained. Suite-setup properties are copied into participating cases.

   :param name: Property name.
   :param value: Property text; convert numeric values to strings first.
   :returns: Nothing.
   :throws std::logic_error: No test is active.

Resource ownership
------------------

.. cpp:class:: resource_scope

   Noncopyable store of shared references, keyed by string. Context cleanup releases entries in reverse registration order.

.. cpp:function:: template <typename T, typename Factory> std::shared_ptr<T> resource_scope::get_or_create(std::string key, Factory factory)

   Return an existing entry, or call the factory without arguments and retain its pointer.

   :param key: Unique key within this store.
   :param factory: Nullary callable returning a non-null std::shared_ptr<T>. Called only for a new key.
   :returns: Shared ownership of the resource.
   :throws std::logic_error: An existing key has a different type.
   :throws std::invalid_argument: The factory returns null.

   Factory exceptions propagate. Resources may outlive the context if another shared_ptr retains ownership.

.. cpp:function:: void resource_scope::clear()

   Release the store's references in reverse registration order.

.. cpp:class:: temporary_directory

   Noncopyable owner of a unique directory. Destruction recursively removes its contents using the nonthrowing filesystem cleanup overload.

.. cpp:function:: temporary_directory::temporary_directory()

   Create a unique directory under std::filesystem::temp_directory_path(). Filesystem errors propagate; exhausting the creation attempts throws std::runtime_error.

.. cpp:function:: const std::filesystem::path& temporary_directory::path() const

   Return the directory path by reference, valid for this object's lifetime.
