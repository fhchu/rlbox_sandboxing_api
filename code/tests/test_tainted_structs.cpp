#include <cstring>

#include "test_include.hpp"
#include "test_tainted_structs.hpp"

using rlbox::sandbox_reinterpret_cast;
using rlbox::tainted;

// NOLINTNEXTLINE
TEST_CASE("Tainted struct assignment", "[tainted_struct]")
{
  rlbox::RLBoxSandbox<TestSandbox> sandbox;
  sandbox.create_sandbox();

  const auto fieldLong = 7;
  const auto strSize = 10;
  auto fieldString = sandbox.malloc_in_sandbox<char>(strSize);
  std::strncpy(fieldString.UNSAFE_Unverified(), "Hello", strSize);
  const auto fieldBool = 1;

  auto ps = sandbox.malloc_in_sandbox<testStruct>();
  ps->fieldLong = fieldLong;
  ps->fieldString = sandbox_reinterpret_cast<const char*>(fieldString);
  ps->fieldBool = fieldBool;
  // char* temp = ps->fieldFixedArr.UNSAFE_Unverified();
  // std::strncpy(temp, "Bye", sizeof(ps->fieldFixedArr));
  ps->voidPtr = nullptr;

  REQUIRE(ps->fieldLong.UNSAFE_Unverified() == fieldLong);
  REQUIRE(std::strcmp(ps->fieldString.UNSAFE_Unverified(), "Hello") == 0);
  REQUIRE(ps->fieldBool.UNSAFE_Unverified() == fieldBool);

  // check that we can't test a tainted_volatile directly
  REQUIRE_COMPILE_ERR(ps->voidPtr == nullptr);
  tainted<void*, TestSandbox> voidPtr = ps->voidPtr;
  REQUIRE(voidPtr == nullptr);
  REQUIRE(ps->voidPtr.UNSAFE_Unverified() == nullptr);
  REQUIRE(voidPtr.UNSAFE_Unverified() == nullptr);

  sandbox.destroy_sandbox();
}