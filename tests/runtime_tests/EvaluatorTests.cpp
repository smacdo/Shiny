#include "runtime/Evaluator.h"

#include "runtime/Reader.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

#include <iostream>

using namespace Shiny;

namespace {
  Value read(std::string_view input, std::shared_ptr<VmState> vmState) {
    Reader r{vmState};
    return r.read(input);
  }

  Value evaluate(std::string_view input, std::shared_ptr<VmState> vmState) {
    Evaluator eval(vmState);
    return eval.evaluate(read(input, vmState));
  }

  bool listEquals(Value expected, Value actual) {
    if (!expected.isPair() || !actual.isPair()) {
      return false;
    }

    auto a = expected;
    auto b = actual;

    int index = 0;

    do {
      auto va = car(a);
      auto vb = car(b);

      a = cdr(a);
      b = cdr(b);

      if (va != vb || (a.isEmptyList() != b.isEmptyList())) {
        std::cerr << "List mismatch at index " << index << ": expected "
                  << a.toString() << ", but was " << b.toString() << std::endl;
        return false;
      }

      index++;
    } while (!a.isEmptyList() && !b.isEmptyList());

    return true;
  }
} // namespace

TEST_CASE("Self evaluating values", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("boolean") {
    REQUIRE(Value{true} == evaluate("#t", vmState));
    REQUIRE(Value{false} == evaluate("#f", vmState));
  }

  SECTION("fixnums") {
    REQUIRE(Value{0} == evaluate("0", vmState));
    REQUIRE(Value{1} == evaluate("1", vmState));
    REQUIRE(Value{2} == evaluate("2", vmState));
    REQUIRE(Value{-1} == evaluate("-1", vmState));
    REQUIRE(Value{42} == evaluate("42", vmState));
    REQUIRE(Value{-100} == evaluate("-100", vmState));

    REQUIRE(Value{5} != evaluate("50", vmState));
  }

  SECTION("chars") {
    REQUIRE(Value{'a'} == evaluate("#\\a", vmState));
    REQUIRE(Value{'H'} == evaluate("#\\H", vmState));
    REQUIRE(Value{' '} == evaluate("#\\space", vmState));
  }

  SECTION("strings") {
    REQUIRE(
        "hello world" == evaluate("\"hello world\"", vmState).toStringView());
    REQUIRE("foobar" == evaluate("\"foobar\"", vmState).toStringView());
  }
}

TEST_CASE("Evaluating symbols", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("returns bound variable value") {
    auto varName1 = vmState->makeSymbol("foo");
    vmState->environment().defineVariable(varName1, Value{125});

    auto varName2 = vmState->makeSymbol("bar");
    vmState->environment().defineVariable(varName2, Value{'Y'});

    REQUIRE(Value{125} == evaluate("foo", vmState));
    REQUIRE(Value{'Y'} == evaluate("bar", vmState));
  }

  SECTION("throws exception if variable was not defined") {
    auto fn = [&vmState]() { evaluate("badVar", vmState); };
    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }

  // TODO: When basic functions are added, test that variable in list will be
  // evaluated as a procedure call.
  //  Ex: foo = '+', (foo 1 2) => 3.
}

TEST_CASE("Quoting values", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("quote procedure") {
    REQUIRE(Value{42} == evaluate("(quote 42)", vmState));
    REQUIRE(
        listEquals(read("(8 3)", vmState), evaluate("(quote (8 3))", vmState)));
  }

  SECTION("quote shorthand") {
    REQUIRE(Value{'c'} == evaluate("'#\\c", vmState));
    REQUIRE(
        listEquals(read("(8 3 2)", vmState), evaluate("'(8 3 2)", vmState)));
  }

  // TODO: After converting to SEXP with better argument handling...
  //  1. No args => exception.
  //  3. Two or more args => exception.
}

TEST_CASE("Defining values", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("single value") {
    evaluate("(define a 813)", vmState);
    REQUIRE(Value{813} == evaluate("a", vmState));
  }

  SECTION("returns the new value") {
    REQUIRE(Value{813} == evaluate("(define a 813)", vmState));
  }

  SECTION("overwrites previous value") {
    evaluate("(define a 813)", vmState);
    REQUIRE(Value{813} == evaluate("a", vmState));

    evaluate("(define a 12)", vmState);
    REQUIRE(Value{12} == evaluate("a", vmState));
  }

  SECTION("multiple values") {
    evaluate("(define a 10)", vmState);
    evaluate("(define b 88)", vmState);
    REQUIRE(Value{10} == evaluate("a", vmState));
    REQUIRE(Value{88} == evaluate("b", vmState));
  }

  SECTION("throws exception if variable name is not a symbol") {
    auto fn1 = [&vmState]() { evaluate("(define 2 5)", vmState); };
    auto fn2 = [&vmState]() { evaluate("(define #\\c 3)", vmState); };
    auto fn3 = [&vmState]() { evaluate("(define \"hello\" \"!\")", vmState); };

    REQUIRE_THROWS_AS(fn1(), VariableNameSymbolRequiredException);
    REQUIRE_THROWS_AS(fn2(), VariableNameSymbolRequiredException);
    REQUIRE_THROWS_AS(fn3(), VariableNameSymbolRequiredException);
  }

  // TODO: After converting to SEXP with better argument handling...
  //  1. No args => exception.
  //  2. One arg => exception.
  //  3. Three or more args => exception.

  // TODO: When nested environments are added...
  //  1. Defines new variable in current scope when current environment has an
  // enclosing env (not in any earlier env).
  //  2. Defines new variable in current scope and does not overwrite variable
  // binding in an enclosing scope.
}

TEST_CASE("Setting values", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("throws exception if variable was not defined") {
    auto fn = [&vmState]() { evaluate("(set! notDefinedVar 22)", vmState); };
    REQUIRE_THROWS_AS(fn(), UnboundVariableException);
  }

  SECTION("changes variable value after definition") {
    evaluate("(define a 813)", vmState);
    REQUIRE(Value{813} == evaluate("a", vmState));

    evaluate("(set! a #\\$)", vmState);
    REQUIRE(Value{'$'} == evaluate("a", vmState));
  }

  SECTION("returns the new value") {
    evaluate("(define c #t)", vmState);
    REQUIRE(Value{2} == evaluate("(set! c 2)", vmState));
  }

  SECTION("multiple values") {
    evaluate("(define a 10)", vmState);
    evaluate("(define b 88)", vmState);

    evaluate("(set! b 3)", vmState);
    evaluate("(set! a 5)", vmState);

    REQUIRE(Value{5} == evaluate("a", vmState));
    REQUIRE(Value{3} == evaluate("b", vmState));
  }

  // TODO: After converting to SEXP with better argument handling...
  //  1. No args => exception.
  //  2. One arg => exception.
  //  3. Three or more args => exception.

  // TODO: When nested environments are added...
  //  1. Set! will still throw exception in presence of enclosing env if not
  //  defined in any
  //  2. Set! will modify variable in enclosing env.
  //  3. Set! will modify variable in enclosing env that is more than one away.
  //  4. Set! will modify the closest variable when nested env has more than one
  //     definition for variable.
  //  5. Set! will not modify a sibling scope.
  //  6. Set! will not modify a child scope.
}

TEST_CASE("If procedure", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("when predicate is true it evaluates consequent") {
    REQUIRE(Value{5} == evaluate("(if #t 5 10)", vmState));
    REQUIRE(Value{'t'} == evaluate("(if #t #\\t 6)", vmState));
  }

  SECTION("when predicate is false it evaluates alternative") {
    REQUIRE(Value{10} == evaluate("(if #f 5 10)", vmState));
    REQUIRE(Value{'f'} == evaluate("(if #f 0 #\\f)", vmState));
  }

  SECTION("when predicate is false with missing alternative it returns false") {
    REQUIRE(Value{false} == evaluate("(if #f 5)", vmState));
    REQUIRE(Value{false} == evaluate("(if #f 0)", vmState));
  }

  // TODO: Make sure predicate is evaluated.

  SECTION("when predicate is true it evaluates consequent") {
    evaluate("(define maybeTrue #t)", vmState);
    REQUIRE(Value{5} == evaluate("(if maybeTrue 5 10)", vmState));

    evaluate("(set! maybeTrue #f)", vmState);
    REQUIRE(Value{6} == evaluate("(if maybeTrue #\\t 6)", vmState));
  }

  SECTION("any non-false values are always true") {
    REQUIRE(Value{'T'} == evaluate("(if 0 #\\T #\\F)", vmState));
    REQUIRE(Value{'T'} == evaluate("(if 1 #\\T #\\F)", vmState));
    REQUIRE(Value{'T'} == evaluate("(if #\\f #\\T #\\F)", vmState));
  }

  // TODO: After converting to SEXP with better argument handling...
  //  1. No args => exception.
  //  2. One arg => exception.
  //  3. Three or more args => exception.
}
