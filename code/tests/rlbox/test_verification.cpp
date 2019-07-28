#include <cstdint>
#include <memory>
#include <utility>

#include "test_include.hpp"

using rlbox::tainted;

// NOLINTNEXTLINE
TEST_CASE("RLBox test basic verification", "[verification]")
{
  const auto testVal = 5;
  const auto lb = 0;
  const auto ub = 10;

  tainted<int, TestSandbox> test = testVal;
  auto result = test.copy_and_verify(
    [](int val) { return val > lb && val < ub ? val : -1; });
  REQUIRE(result == 5);
}

// NOLINTNEXTLINE
TEST_CASE("RLBox test enum verification", "[verification]")
{
  using Example_Enum = enum {
    ENUM_UNKNOWN,
    ENUM_FIRST,
    ENUM_SECOND,
    ENUM_THIRD
  };

  tainted<Example_Enum, TestSandbox> ref = ENUM_FIRST;
  auto enumVal = ref.copy_and_verify(
    [](Example_Enum val) { return val <= ENUM_THIRD ? val : ENUM_UNKNOWN; });
  UNUSED(enumVal);
}

// NOLINTNEXTLINE
TEST_CASE("RLBox test pointer verification", "[verification]")
{
  const auto testVal = 5;
  const auto lb = 0;
  const auto ub = 10;

  rlbox::rlbox_sandbox<TestSandbox> sandbox;
  sandbox.create_sandbox();

  tainted<int*, TestSandbox> pa = sandbox.malloc_in_sandbox<int>();
  *pa = testVal;

  auto result1 = pa.copy_and_verify([](std::unique_ptr<int> val) {
    return *val > lb && *val < ub ? std::move(val) : nullptr;
  });
  REQUIRE(result1 != nullptr);
  REQUIRE(*result1 == testVal);

  auto result2 = pa.copy_and_verify_address([](uintptr_t val) { return val; });
  REQUIRE(pa.UNSAFE_unverified() == reinterpret_cast<int*>(result2)); // NOLINT

  sandbox.destroy_sandbox();
}

// NOLINTNEXTLINE
TEST_CASE("RLBox test unverified", "[verification]")
{
  const auto testVal = 5;
  tainted<int, TestSandbox> test = testVal;
  auto result1 = test.UNSAFE_unverified();
  auto result2 = test.copy_and_verify([](int val) { return val; });
  auto result3 = test.unverified_safe_because("Reason: testing");
  REQUIRE(result1 == testVal);
  REQUIRE(result2 == testVal);
  REQUIRE(result3 == testVal);
}