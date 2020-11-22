#include "support/EvaluatorFixture.h"

#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE_METHOD(EvaluatorFixture, "Add procedure", "[Evaluator]") {
  SECTION("sums all passed fixnums") {
    REQUIRE(Value{0} == evaluate("(+)"));
    REQUIRE(Value{4} == evaluate("(+ 1 3)"));
    REQUIRE(Value{12} == evaluate("(+ 10 5 1 -4)"));
  }

  SECTION("error if any argument is not a fixnum") {
    auto fn1 = [this]() { evaluate("(+ 10 1 #t)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(+ 10 1 #\\c)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Sub procedure", "[Evaluator]") {
  SECTION("error with no arguments") {
    auto fn = [this]() { evaluate("(-)"); };
    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
  }

  SECTION("with one argument negates") {
    REQUIRE(Value{-25} == evaluate("(- 25)"));
    REQUIRE(Value{3} == evaluate("(- -3)"));
    REQUIRE(Value{0} == evaluate("(- 0)"));
  }

  SECTION("with two or more arguments subtracts all") {
    REQUIRE(Value{2} == evaluate("(- 5 3)"));
    REQUIRE(Value{3} == evaluate("(- 10 4 3)"));
    REQUIRE(Value{-2} == evaluate("(- 5 6 -2 3)"));
  }

  SECTION("error if any argument is not a fixnum") {
    auto fn1 = [this]() { evaluate("(- \"3\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(- 2 \"3\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }
}
