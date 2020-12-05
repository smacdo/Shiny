#include "runtime/EnvironmentFrame.h"

#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Can extend an existing frame", "[EnvironmentFrame]") {
  auto root = std::make_unique<EnvironmentFrame>();
  auto env1 = std::make_unique<EnvironmentFrame>(root.get());
  auto env2a = std::make_unique<EnvironmentFrame>(env1.get());
  auto env2b = std::make_unique<EnvironmentFrame>(env1.get());

  SECTION("parent pointer is null if not passed in constructor") {
    REQUIRE(root->parent() == nullptr);
  }

  SECTION("parent pointer is to the frame passed in constructor") {
    REQUIRE(env1->parent() == root.get());
    REQUIRE(env2a->parent() == env1.get());
    REQUIRE(env2b->parent() == env1.get());
  }

  SECTION("get parent pointer for const environment frame") {
    const EnvironmentFrame* cenv1 = env1.get();
    REQUIRE(cenv1->parent() == root.get());
  }

  SECTION("hasParents checks if a parent was passed in constructor") {
    const EnvironmentFrame* cenv1 = env1.get();

    REQUIRE_FALSE(root->hasParent());
    REQUIRE(env1->hasParent());
    REQUIRE(cenv1->hasParent());
    REQUIRE(env2a->hasParent());
    REQUIRE(env2b->hasParent());
  }
}

TEST_CASE("Try set variable", "[EnvironmentFrame]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto env1 = vmState->globalEnvironment();
  const auto cenv1 = env1;

  auto env2 = vmState->extend(env1);
  const auto cenv2 = env2;

  SECTION("updates value if variable was defined") {
    auto name = vmState->makeSymbol("secret");

    env1->define(name, Value{4});
    REQUIRE(Value{4} == env1->lookup(name));

    env1->trySet(name, Value{12});
    REQUIRE(Value{12} == env1->lookup(name));
  }

  SECTION("returns true if variable was defined") {
    auto name = vmState->makeSymbol("secret");
    env1->define(name, Value{4});
    REQUIRE(env1->trySet(name, Value{12}));
  }

  SECTION("returns false if variable was not defined") {
    auto name = vmState->makeSymbol("secret");
    REQUIRE_FALSE(env1->trySet(name, Value{12}));
  }

  SECTION("only updates the matching variable name") {
    auto name0 = vmState->makeSymbol("a");
    auto name1 = vmState->makeSymbol("b");
    auto name2 = vmState->makeSymbol("c");

    env1->define(name0, Value{1});
    env1->define(name1, Value{2});
    env1->define(name2, Value{3});

    REQUIRE(env1->trySet(name1, Value{14}));

    REQUIRE(Value{1} == env1->lookup(name0));
    REQUIRE(Value{14} == env1->lookup(name1));
    REQUIRE(Value{3} == env1->lookup(name2));
  }

  SECTION("updates the current frame instead of parent") {
    auto name = vmState->makeSymbol("foo");

    env1->define(name, Value{22});  // parent environment.
    env2->define(name, Value{888}); // child environment.

    REQUIRE(env2->trySet(name, Value{13}));

    REQUIRE(Value{22} == env1->lookup(name));
    REQUIRE(Value{13} == env2->lookup(name));
  }

  SECTION("updates parent if binding not found") {
    auto name = vmState->makeSymbol("foo");
    env1->define(name, Value{22}); // parent environment.

    REQUIRE(env2->trySet(name, Value{16}));

    REQUIRE(Value{16} == env1->lookup(name));
    REQUIRE_FALSE(env2->tryLookup(
        name, EnvironmentFrame::SearchMode::NoRecurse, nullptr));
  }

  SECTION("throws exception if name is not a symbol") {
    REQUIRE_THROWS_AS(
        [&env1]() { env1->trySet(Value{'x'}, Value{false}); }(),
        VariableNameSymbolRequiredException);
  }
}

TEST_CASE("Set variable", "[EnvironmentFrame]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto env1 = vmState->globalEnvironment();
  const auto cenv1 = env1;

  auto env2 = vmState->extend(env1);
  const auto cenv2 = env2;

  SECTION("updates value if variable was defined") {
    auto name = vmState->makeSymbol("secret");

    env1->define(name, Value{4});
    REQUIRE(Value{4} == env1->lookup(name));

    env1->set(name, Value{12});
    REQUIRE(Value{12} == env1->lookup(name));
  }

  SECTION("only updates the matching variable name") {
    auto name0 = vmState->makeSymbol("a");
    auto name1 = vmState->makeSymbol("b");
    auto name2 = vmState->makeSymbol("c");

    env1->define(name0, Value{1});
    env1->define(name1, Value{2});
    env1->define(name2, Value{3});

    env1->set(name1, Value{14});

    REQUIRE(Value{1} == env1->lookup(name0));
    REQUIRE(Value{14} == env1->lookup(name1));
    REQUIRE(Value{3} == env1->lookup(name2));
  }

  SECTION("updates the current frame instead of parent") {
    auto name = vmState->makeSymbol("foo");

    env1->define(name, Value{22});  // parent environment.
    env2->define(name, Value{888}); // child environment.

    env2->set(name, Value{13});

    REQUIRE(Value{22} == env1->lookup(name));
    REQUIRE(Value{13} == env2->lookup(name));
  }

  SECTION("updates parent if binding not found") {
    auto name = vmState->makeSymbol("foo");
    env1->define(name, Value{22}); // parent environment.

    env2->set(name, Value{16});

    REQUIRE(Value{16} == env1->lookup(name));
    REQUIRE_FALSE(env2->tryLookup(
        name, EnvironmentFrame::SearchMode::NoRecurse, nullptr));
  }

  SECTION("throws exception if variable was not defined") {
    auto name = vmState->makeSymbol("secret");
    auto fn = [&env1, &name]() { env1->set(name, Value{false}); };

    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }

  SECTION("throws exception if name is not a symbol") {
    REQUIRE_THROWS_AS(
        [&env1]() { env1->set(Value{'x'}, Value{false}); }(),
        VariableNameSymbolRequiredException);
  }
}

TEST_CASE("Lookup variable", "[EnvironmentFrame]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto env1 = vmState->globalEnvironment();
  const auto cenv1 = env1;

  auto env2 = vmState->extend(env1);
  const auto cenv2 = env2;

  SECTION("searches self before parent when looking up name") {
    auto name = vmState->makeSymbol("deer");

    REQUIRE(env1->define(name, Value{1}));
    REQUIRE(env2->define(name, Value{2}));

    REQUIRE(Value{1} == cenv1->lookup(name));
    REQUIRE(Value{2} == cenv2->lookup(name));
  }

  SECTION("searches parent if name not in self") {
    auto name = vmState->makeSymbol("fishy");

    REQUIRE(env1->define(name, Value{'f'}));

    REQUIRE(Value{'f'} == cenv1->lookup(name));
    REQUIRE(Value{'f'} == cenv2->lookup(name));
  }

  SECTION("does not search parent if told not to recurse") {
    auto name = vmState->makeSymbol("fishy");

    REQUIRE(env1->define(name, Value{'f'}));

    REQUIRE(
        Value{'f'} ==
        cenv1->lookup(name, EnvironmentFrame::SearchMode::NoRecurse));

    auto fn = [&cenv2, &name]() {
      cenv2->lookup(name, EnvironmentFrame::SearchMode::NoRecurse);
    };
    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }

  SECTION("throws exception if variable not defined") {
    auto name = vmState->makeSymbol("your_name_here");
    auto fn = [&cenv1, &name]() { cenv1->lookup(name); };

    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }
}

TEST_CASE("Try lookup variable", "[EnvironmentFrame]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto env1 = vmState->globalEnvironment();
  const auto cenv1 = env1;

  auto env2 = vmState->extend(env1);
  const auto cenv2 = env2;

  SECTION("searches self before parent when looking up name") {
    auto name = vmState->makeSymbol("deer");

    REQUIRE(env1->define(name, Value{1}));
    REQUIRE(env2->define(name, Value{2}));

    Value v;
    REQUIRE(cenv1->tryLookup(name, &v));
    REQUIRE(Value{1} == v);

    REQUIRE(cenv2->tryLookup(name, &v));
    REQUIRE(Value{2} == v);
  }

  SECTION("searches parent if name not in self") {
    auto name = vmState->makeSymbol("fishy");

    REQUIRE(env1->define(name, Value{'f'}));

    Value v;
    REQUIRE(cenv1->tryLookup(name, &v));
    REQUIRE(Value{'f'} == v);

    REQUIRE(cenv2->tryLookup(name, &v));
    REQUIRE(Value{'f'} == v);
  }

  SECTION("does not search parent if told not to recurse") {
    auto name = vmState->makeSymbol("fishy");

    REQUIRE(env1->define(name, Value{'f'}));

    Value v;

    REQUIRE(
        cenv1->tryLookup(name, EnvironmentFrame::SearchMode::NoRecurse, &v));
    REQUIRE(Value{'f'} == v);

    REQUIRE_FALSE(
        cenv2->tryLookup(name, EnvironmentFrame::SearchMode::NoRecurse, &v));
  }

  SECTION("returns false if variable is not defined") {
    auto name = vmState->makeSymbol("your_name_here");
    Value result;

    REQUIRE_FALSE(cenv1->tryLookup(name, &result));
  }
}

TEST_CASE("Define new variables", "[EnvironmentFrame]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto env1 = vmState->globalEnvironment();
  const auto cenv1 = env1;

  auto env2 = vmState->extend(env1);
  const auto cenv2 = env2;

  SECTION("returns true if new variable") {
    auto name1 = vmState->makeSymbol("foobar");
    REQUIRE(env1->define(name1, Value{42}));

    auto name2 = vmState->makeSymbol("testing");
    REQUIRE(env1->define(name2, Value{42}));
  }

  SECTION("returns false if updating existing variable") {
    auto name = vmState->makeSymbol("foobar");
    REQUIRE(env1->define(name, Value{42}));
    REQUIRE_FALSE(env1->define(name, Value{false}));
  }

  SECTION("does not search parent value when defining") {
    auto name = vmState->makeSymbol("barfoo");
    REQUIRE(env2->define(name, Value{22}));

    REQUIRE_FALSE(env1->tryLookup(
        name, EnvironmentFrame::SearchMode::NoRecurse, nullptr));
    REQUIRE(
        Value{22} ==
        env2->lookup(name, EnvironmentFrame::SearchMode::NoRecurse));
  }

  SECTION("does not update parent value when defining") {
    auto name = vmState->makeSymbol("barfoo");
    REQUIRE(env1->define(name, Value{42}));
    REQUIRE(env2->define(name, Value{22})); // True because creating.

    REQUIRE(
        Value{42} ==
        env1->lookup(name, EnvironmentFrame::SearchMode::NoRecurse));

    REQUIRE(
        Value{22} ==
        env2->lookup(name, EnvironmentFrame::SearchMode::NoRecurse));
  }

  SECTION("defining a second time updates the value") {
    auto name = vmState->makeSymbol("age");
    REQUIRE(env1->define(name, Value{22}));
    REQUIRE(Value{22} == env1->lookup(name));

    REQUIRE_FALSE(env1->define(name, Value{'c'}));
    REQUIRE(Value{'c'} == env1->lookup(name));
  }

  SECTION("throws exception if name is not a symbol") {
    REQUIRE_THROWS_AS(
        [&env1]() { env1->define(Value{'x'}, Value{false}); }(),
        VariableNameSymbolRequiredException);
  }
}