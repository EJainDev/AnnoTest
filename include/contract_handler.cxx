export module annotest.contracts;

import annotest;

import std;

namespace annotest {
void handle_contract_violation(const std::contracts::contract_violation& violation) {
  const auto& loc = violation.location();

  contract_violation_occurred = true;
  if (auto handler = custom_handler.load()) {
    handler(violation);
  }
}
}  // namespace annotest