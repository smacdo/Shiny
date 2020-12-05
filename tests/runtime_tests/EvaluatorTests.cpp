#include "runtime/Evaluator.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/Reader.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

// TODO: Rewrite using the test fixture in EvaluatorProcedureTests.cpp
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
    vmState->globalEnvironment()->define(varName1, Value{125});

    auto varName2 = vmState->makeSymbol("bar");
    vmState->globalEnvironment()->define(varName2, Value{'Y'});

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

TEST_CASE("Lambda form is parsed into a CompoundProcedure", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  // TODO: Check that enclosingFrame is correctly set (eg doesnt just take the
  // global environment).

  SECTION("with lambda that returns a procedure") {
    auto proc = evaluate("(lambda (x y) (+ x y))", vmState);
    REQUIRE(proc.isCompoundProcedure());
  }

  SECTION("with a no argument lambda") {
    auto proc = evaluate("(lambda () 22)", vmState);
    auto p = proc.toCompoundProcedure();

    REQUIRE(0 == p->parameterCount);
    REQUIRE(Value::EmptyList == p->parameters);
    REQUIRE(ValueType::Pair == p->body.type());
    REQUIRE(Value{22} == car(p->body));
    REQUIRE(vmState->globalEnvironment() == p->enclosingFrame);
  }

  SECTION("with a simple adder") {
    auto proc = evaluate("(lambda (x y) (+ x y))", vmState);
    auto p = proc.toCompoundProcedure();

    auto x = vmState->makeSymbol("x");
    auto y = vmState->makeSymbol("y");
    auto plus = vmState->makeSymbol("+");

    REQUIRE(2 == p->parameterCount);
    REQUIRE(x == car(p->parameters));
    REQUIRE(y == cadr(p->parameters));

    auto s0 = car(p->body);
    REQUIRE(Value::EmptyList == cdr(p->body));

    REQUIRE(plus == car(s0));
    REQUIRE(x == cadr(s0));
    REQUIRE(y == caddr(s0));
  }

  SECTION("with a multi line body") {
    auto proc = evaluate("(lambda (x) (x 2) (+ x 1))", vmState);
    auto p = proc.toCompoundProcedure();

    auto x = vmState->makeSymbol("x");
    auto plus = vmState->makeSymbol("+");

    REQUIRE(1 == p->parameterCount);
    REQUIRE(x == car(p->parameters));

    auto s0 = car(p->body);
    auto s1 = cadr(p->body);
    REQUIRE(Value::EmptyList == cddr(p->body));

    REQUIRE(x == car(s0));
    REQUIRE(Value{2} == cadr(s0));
    REQUIRE(Value::EmptyList == cddr(s0));

    REQUIRE(plus == car(s1));
    REQUIRE(x == cadr(s1));
    REQUIRE(Value{1} == caddr(s1));
    REQUIRE(Value::EmptyList == cdddr(s1));
  }

  SECTION("throws error if parameter is not a symbol") {
    auto fn = [&vmState]() { evaluate("(lambda (x 2) 33)", vmState); };
    REQUIRE_THROWS_AS(fn(), WrongValueTypeException);
  }

  // TODO: After converting to SEXP with better argument handling...
  //  1. No args => exception.
  //  2. One arg => exception.
  //  3. Three or more args => exception.
}

TEST_CASE("Lambda evaluation", "[Evaluator]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  // TODO: Verify tail call.

  SECTION("no argument lambda that returns atom") {
    REQUIRE(Value{42} == evaluate("((lambda () 42))", vmState));
  }

  SECTION("one argument lambda returns self") {
    REQUIRE(Value{5} == evaluate("((lambda (x) x) 5)", vmState));
    REQUIRE(Value{'z'} == evaluate("((lambda (x) x) #\\z)", vmState));
  }

  SECTION("two argument lambda returns added arugments") {
    REQUIRE(Value{8} == evaluate("((lambda (x y) (+ x y)) 5 3)", vmState));
    REQUIRE(Value{11} == evaluate("((lambda (x y) (+ x y)) 15 -4)", vmState));
  }

  // TODO: Verify all expressions in body except the last one do not return
  // a value.
  // TODO: Write some more complicated lambdas that utilize scope to verify.

  // TODO: After converting to SEXP with better argument handling...
  //  1. No args => exception.
  //  2. One arg => exception.
  //  3. Three or more args => exception.
}
