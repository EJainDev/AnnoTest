.. index::
   single: test_context
   single: current_test

Test Context and Resources
==========================

Test methods, fixture constructors, and lifecycle hooks may accept
``annotest::test_context&``. ``annotest::current_test()`` returns the active test
context and throws ``std::logic_error`` when no test is active.

.. code-block:: cpp

    struct ContextTests {
        [[= test{}]] void metadata(test_context& context) {
            assert_equal(context.name, current_test().name);
            record_property("input", "sample");
            assert_true(std::filesystem::exists(context.temp_dir()));
        }
    };

.. index::
   single: test metadata

Fields
------

.. list-table:: test_context fields
   :header-rows: 1
   :widths: 25 35 40

   * - Field
     - Type
     - Description
   * - ``suite``
     - ``std::string``
     - The full suite name, including enclosing and parameterized suite names.
   * - ``name``
     - ``std::string``
     - The case name within its suite.
   * - ``tags``
     - ``std::vector<std::string>``
     - The case's tags, including inherited suite tags.
   * - ``parameters``
     - ``std::string``
     - The available formatted parameter description.
   * - ``repetition``
     - ``unsigned``
     - The zero-based index across annotation and command-line repetitions.
   * - ``resources``
     - ``resource_scope``
     - A store of shared resource references.

.. index::
   single: constructor contexts

Constructor contexts
--------------------

A fixture accepting ``test_context&`` receives a context owned by that fixture.
It remains valid through the fixture's destructor. Shared fixtures retain this
context across cases; the runner refreshes its identity fields before each case.
``current_test()`` refers to the active invocation's context, which may be a
different object from the fixture's constructor context.

Suite-hook contexts retain their temporary directories and resources until all
``after_all`` hooks finish. They are distinct from per-test and constructor contexts.

.. index::
   single: temp_dir
   single: temporary directories

Temporary directories
---------------------

``context.temp_dir()`` returns ``const std::filesystem::path&``. The first call
creates a unique temporary directory; later calls on the same context return
the same path. The directory and its contents are removed during context cleanup.
An invocation's directory is cleaned up after its extension cleanup hooks.
A fixture constructor's context retains its directory until fixture destruction.

.. index::
   single: resource_scope
   single: get_or_create
   single: resource store

Resource store
--------------

``context.resources.get_or_create<T>(key, factory)`` returns
``std::shared_ptr<T>``. The factory is called without arguments when the key is
first requested and must return a non-null ``std::shared_ptr<T>``.
Later requests with the same key and type return the stored pointer.
Reusing a key with a different type throws ``std::logic_error``; a null factory
result throws ``std::invalid_argument``. Factory exceptions propagate.

``resources.clear()`` releases the store's references in reverse registration
order. The store also clears during context cleanup. A resource remains alive
while another ``std::shared_ptr`` owns it.

.. code-block:: cpp

    auto& resources = current_test().resources;
    auto value = resources.get_or_create<int>(
        "counter", [] { return std::make_shared<int>(7); });
    auto same = resources.get_or_create<int>(
        "counter", [] { return std::make_shared<int>(99); });
    assert_equal(value, same);
    assert_equal(7, *same);

.. index::
   single: record_property
   single: report properties

Report properties
-----------------

``record_property(std::string name, std::string value)`` adds a property to the
active case. Properties appear in XML and JSON reports and in
``test_result::properties``. The function throws ``std::logic_error`` when no
test is active. Convert numeric values to strings before recording them.

.. code-block:: cpp

    record_property("rows", std::to_string(42));
