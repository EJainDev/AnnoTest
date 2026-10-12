Runtime Sources, Factories, and Listeners
=========================================

Named providers expand arguments into discoverable cases. CSV and line helpers
read local files; factories return named callable children. The example also
registers a runtime test and installs a reporter listener.

Providers, registrations, and factories must reproduce the same case names in
each worker invocation when using isolation.

.. literalinclude:: ../../../examples/example14_runtime_sources.cpp
   :language: cpp

Build and run ``example14_runtime_sources``.
