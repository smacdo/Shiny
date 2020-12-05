#include "runtime/VmState.h"

#include "runtime/EnvironmentFrame.h"
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

TEST_CASE("Extend an existing environment", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  auto env1 = vmState->extend(vmState->globalEnvironment());
  REQUIRE(env1->parent() == vmState->globalEnvironment());

  auto env2a = vmState->extend(env1);
  REQUIRE(env2a->parent() == env1);

  auto env2b = vmState->extend(env1);
  REQUIRE(env2b->parent() == env1);

  REQUIRE(env2a != env2b);
}

TEST_CASE("Extend an existing environment with new bindings", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto a = vmState->makeSymbol("a");
  auto b = vmState->makeSymbol("b");
  auto names = vmState->makePair(a, vmState->makePair(b, Value::EmptyList));
  auto values = vmState->makePair(
      Value{8}, vmState->makePair(Value{-4}, Value::EmptyList));

  auto env = vmState->extend(vmState->globalEnvironment(), names, values);
  REQUIRE(env->parent() == vmState->globalEnvironment());

  REQUIRE(Value{8} == env->lookup(a, EnvironmentFrame::SearchMode::NoRecurse));
  REQUIRE(Value{-4} == env->lookup(b, EnvironmentFrame::SearchMode::NoRecurse));
}

TEST_CASE("Create a compound procedure", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto env1 = vmState->extend(vmState->globalEnvironment());

  SECTION("with an empty parameter list") {
    auto procV =
        vmState->makeCompoundProcedure(Value::EmptyList, Value{5}, env1);
    auto proc = procV.toCompoundProcedure();

    REQUIRE(0 == proc->parameterCount);
    REQUIRE(proc->parameters.isEmptyList());
  }

  SECTION("with an two parameter list") {
    auto params = vmState->makePair(
        vmState->makeSymbol("x"),
        vmState->makePair(vmState->makeSymbol("y"), Value::EmptyList));
    auto procV = vmState->makeCompoundProcedure(params, Value{5}, env1);
    auto proc = procV.toCompoundProcedure();

    REQUIRE(2 == proc->parameterCount);
    REQUIRE(vmState->makeSymbol("x") == proc->parameters.toRawPair()->car);
    REQUIRE(
        vmState->makeSymbol("y") ==
        proc->parameters.toRawPair()->cdr.toRawPair()->car);
    REQUIRE(
        Value::EmptyList == proc->parameters.toRawPair()->cdr.toRawPair()->cdr);
  }

  SECTION("stores body") {
    auto procV =
        vmState->makeCompoundProcedure(Value::EmptyList, Value{5}, env1);
    auto proc = procV.toCompoundProcedure();

    REQUIRE(Value{5} == proc->body);
  }

  SECTION("stores enclosing frame") {
    auto procV =
        vmState->makeCompoundProcedure(Value::EmptyList, Value{5}, env1);
    auto proc = procV.toCompoundProcedure();

    REQUIRE(env1 == proc->enclosingFrame);
  }

  SECTION("throws error if parameter is not a symbol") {
    auto fn = [&vmState, &env1]() {
      vmState->makeCompoundProcedure(
          vmState->makePair(Value{'h'}, Value::EmptyList), Value{5}, env1);
    };

    REQUIRE_THROWS_AS(fn(), WrongValueTypeException);
  }
}

TEST_CASE("Make a pair", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = vmState->makePair(Value{4}, Value{'!'});
  REQUIRE(ValueType::Pair == pair.type());
  REQUIRE(Value{4} == car(pair));
  REQUIRE(Value{'!'} == cdr(pair));
}

TEST_CASE("Make a list", "[VmState]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("empty list") {
    auto list = vmState->makeList({});
    REQUIRE(ValueType::EmptyList == list.type());
  }

  SECTION("one item list") {
    auto list = vmState->makeList({Value{'^'}});
    REQUIRE(ValueType::Pair == list.type());

    REQUIRE(Value{'^'} == car(list));
    REQUIRE(Value::EmptyList == cdr(list));
  }

  SECTION("two item list") {
    auto list = vmState->makeList({Value{'H'}, Value{1}});
    REQUIRE(ValueType::Pair == list.type());

    REQUIRE(Value{'H'} == car(list));
    REQUIRE(Value{1} == cadr(list));
    REQUIRE(Value::EmptyList == cddr(list));
  }

  SECTION("three item list") {
    auto list = vmState->makeList({Value{2}, Value{-13}, Value{22}});
    REQUIRE(ValueType::Pair == list.type());

    REQUIRE(Value{2} == car(list));
    REQUIRE(Value{-13} == cadr(list));
    REQUIRE(Value{22} == caddr(list));
    REQUIRE(Value::EmptyList == cdddr(list));
  }
}
