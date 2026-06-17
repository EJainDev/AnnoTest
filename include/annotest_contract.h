#pragma once

import annotest;

import std;

void handle_contract_violation(const std::contracts::contract_violation& violation) {
  const auto& loc = violation.location();

  annotest::contract_violation_occurred = true;
  if (auto handler = annotest::custom_handler.load()) {
    handler(violation);
  }
}