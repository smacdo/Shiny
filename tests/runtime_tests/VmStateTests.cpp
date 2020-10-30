#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Create new strings", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  auto foo = vmState->makeString("foo");
  REQUIRE(ValueType::String == foo.type());
  REQUIRE("\"foo\"" == foo.toString());

  auto bar = vmState->makeString("bar");
  REQUIRE(ValueType::String == bar.type());
  REQUIRE("\"bar\"" == bar.toString());
}

TEST_CASE("Create new symbol values", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  auto foo = vmState->makeSymbol("foo");
  REQUIRE(ValueType::Symbol == foo.type());
  REQUIRE("foo" == foo.toString());

  auto bar = vmState->makeSymbol("bar");
  REQUIRE(ValueType::Symbol == bar.type());
  REQUIRE("bar" == bar.toString());
}

TEST_CASE("Reuse symbol instance for same name", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  auto foo1 = vmState->makeSymbol("foo");
  auto foo2 = vmState->makeSymbol("foo");
  auto bar = vmState->makeSymbol("bar");

  REQUIRE(foo1 == foo2);
  REQUIRE(foo1 != bar);
  REQUIRE(foo2 != bar);
}
