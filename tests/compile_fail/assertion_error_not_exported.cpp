import annotest;

// This must stay inaccessible to importing code: assertions stop the test,
// and consumers should not catch the runner's internal control-flow exception.
using HiddenException = annotest::assertion_error;

int main() { return 0; }
