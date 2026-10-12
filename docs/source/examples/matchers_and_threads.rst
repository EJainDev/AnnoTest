Matchers, Groups, and Helper Threads
====================================

Use matchers for structured values and ``assert_all`` to group independent checks.
``test_thread`` joins before the case returns. ``capture_failures`` lets a test
inspect intentionally failing checks without failing the outer case.

.. literalinclude:: ../../../examples/example12_matchers_and_threads.cpp
   :language: cpp

Build and run ``example12_matchers_and_threads``.
