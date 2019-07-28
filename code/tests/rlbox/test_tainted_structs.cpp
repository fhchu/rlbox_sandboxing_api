#include <array>
#include <cstring>
#include <type_traits>

#include "test_include.hpp"
#include "test_tainted_structs.hpp"

using rlbox::sandbox_reinterpret_cast;
using rlbox::tainted;

// NOLINTNEXTLINE
TEST_CASE("Tainted struct assignment", "[tainted_struct]")
{
  rlbox::rlbox_sandbox<TestSandbox> sandbox;
  sandbox.create_sandbox();

  const auto fieldLong = 7;
  const auto strSize = 10;
  auto fieldString = sandbox.malloc_in_sandbox<char>(strSize);
  std::strncpy(fieldString.UNSAFE_unverified(), "Hello", strSize);
  const auto fieldBool = 1;

  auto ps = sandbox.malloc_in_sandbox<testVarietyStruct>();
  ps->fieldLong = fieldLong;
  ps->fieldString = sandbox_reinterpret_cast<const char*>(fieldString);
  ps->fieldBool = fieldBool;
  char* temp = (&(ps->fieldFixedArr[0])).UNSAFE_unverified();
  std::strncpy(temp, "Bye", sizeof(ps->fieldFixedArr));
  ps->voidPtr = nullptr;

  REQUIRE(ps->fieldLong.UNSAFE_unverified() == fieldLong);
  REQUIRE(std::strcmp(ps->fieldString.UNSAFE_unverified(), "Hello") == 0);
  REQUIRE(ps->fieldBool.UNSAFE_unverified() == fieldBool);
  auto fixedArr = ps->fieldFixedArr.UNSAFE_unverified();
  REQUIRE(std::strcmp(&fixedArr[0], "Bye") == 0);

  tainted<void*, TestSandbox> voidPtr = ps->voidPtr;
  REQUIRE(voidPtr == nullptr);
  REQUIRE(ps->voidPtr.UNSAFE_unverified() == nullptr);
  REQUIRE(voidPtr.UNSAFE_unverified() == nullptr);

  sandbox.destroy_sandbox();
}

// NOLINTNEXTLINE
TEST_CASE("Tainted const structs", "[tainted_struct]")
{
  rlbox::rlbox_sandbox<TestSandbox> sandbox;
  sandbox.create_sandbox();

  const auto fieldLong = 7;

  auto ps = sandbox.malloc_in_sandbox<testVarietyStruct>();
  ps->fieldLong = fieldLong;
  auto cps = rlbox::sandbox_const_cast<const testVarietyStruct*>(ps);
  REQUIRE(std::is_same_v<decltype(cps),
                         tainted<const testVarietyStruct*, TestSandbox>>);
  REQUIRE(cps->fieldLong.UNSAFE_unverified() == fieldLong);

  sandbox.destroy_sandbox();
}