#include "runtime/Environment.h"

#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Try set variable", "[Environment]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto& env = vmState->environment();
  const auto& cenv = env;

  SECTION("updates value if variable was defined") {
    auto name = vmState->makeSymbol("secret");

    env.defineVariable(name, Value{4});
    REQUIRE(Value{4} == env.lookupVariable(name));

    env.trySetVariable(name, Value{12});
    REQUIRE(Value{12} == env.lookupVariable(name));
  }

  SECTION("returns true if variable was defined") {
    auto name = vmState->makeSymbol("secret");
    env.defineVariable(name, Value{4});
    REQUIRE(env.trySetVariable(name, Value{12}));
  }

  SECTION("returns false if variable was not defined") {
    auto name = vmState->makeSymbol("secret");
    REQUIRE_FALSE(env.trySetVariable(name, Value{12}));
  }

  SECTION("only updates the matching variable name") {
    auto name0 = vmState->makeSymbol("a");
    auto name1 = vmState->makeSymbol("b");
    auto name2 = vmState->makeSymbol("c");

    env.defineVariable(name0, Value{1});
    env.defineVariable(name1, Value{2});
    env.defineVariable(name2, Value{3});

    REQUIRE(env.trySetVariable(name1, Value{14}));

    REQUIRE(Value{1} == env.lookupVariable(name0));
    REQUIRE(Value{14} == env.lookupVariable(name1));
    REQUIRE(Value{3} == env.lookupVariable(name2));
  }

  SECTION("throws exception if name is not a symbol") {
    REQUIRE_THROWS_AS(
        [&env]() { env.trySetVariable(Value{'x'}, Value{false}); }(),
        VariableNameSymbolRequiredException);
  }
}

TEST_CASE("Set variable", "[Environment]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto& env = vmState->environment();
  const auto& cenv = env;

  SECTION("updates value if variable was defined") {
    auto name = vmState->makeSymbol("secret");

    env.defineVariable(name, Value{4});
    REQUIRE(Value{4} == env.lookupVariable(name));

    env.setVariable(name, Value{12});
    REQUIRE(Value{12} == env.lookupVariable(name));
  }

  SECTION("only updates the matching variable name") {
    auto name0 = vmState->makeSymbol("a");
    auto name1 = vmState->makeSymbol("b");
    auto name2 = vmState->makeSymbol("c");

    env.defineVariable(name0, Value{1});
    env.defineVariable(name1, Value{2});
    env.defineVariable(name2, Value{3});

    env.setVariable(name1, Value{14});

    REQUIRE(Value{1} == env.lookupVariable(name0));
    REQUIRE(Value{14} == env.lookupVariable(name1));
    REQUIRE(Value{3} == env.lookupVariable(name2));
  }

  SECTION("throws exception if variable was not defined") {
    auto name = vmState->makeSymbol("secret");
    auto fn = [&env, &name]() { env.setVariable(name, Value{false}); };

    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }

  SECTION("throws exception if name is not a symbol") {
    REQUIRE_THROWS_AS(
        [&env]() { env.trySetVariable(Value{'x'}, Value{false}); }(),
        VariableNameSymbolRequiredException);
  }
}

TEST_CASE("Lookup variable", "[Environment]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto& env = vmState->environment();
  const auto& cenv = env;

  SECTION("throws exception if variable not defined") {
    auto name = vmState->makeSymbol("your_name_here");
    auto fn = [&cenv, &name]() { cenv.lookupVariable(name); };

    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }
}

TEST_CASE("Try lookup variable", "[Environment]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto& env = vmState->environment();
  const auto& cenv = env;

  SECTION("returns false if variable is not defined") {
    auto name = vmState->makeSymbol("your_name_here");
    Value result;

    REQUIRE_FALSE(cenv.tryLookupVariable(name, &result));
  }
}

TEST_CASE("Define new variables", "[Environment]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto& env = vmState->environment();

  SECTION("returns true if new variable") {
    auto name1 = vmState->makeSymbol("foobar");
    REQUIRE(env.defineVariable(name1, Value{42}));

    auto name2 = vmState->makeSymbol("testing");
    REQUIRE(env.defineVariable(name2, Value{42}));
  }

  SECTION("returns false if updating existing variable") {
    auto name = vmState->makeSymbol("foobar");
    REQUIRE(env.defineVariable(name, Value{42}));
    REQUIRE_FALSE(env.defineVariable(name, Value{false}));
  }

  SECTION("defining a second time updates the value") {
    auto name = vmState->makeSymbol("age");
    REQUIRE(env.defineVariable(name, Value{22}));
    REQUIRE(Value{22} == env.lookupVariable(name));

    REQUIRE_FALSE(env.defineVariable(name, Value{'c'}));
    REQUIRE(Value{'c'} == env.lookupVariable(name));
  }

  SECTION("throws exception if name is not a symbol") {
    REQUIRE_THROWS_AS(
        [&env]() { env.defineVariable(Value{'x'}, Value{false}); }(),
        VariableNameSymbolRequiredException);
  }
}