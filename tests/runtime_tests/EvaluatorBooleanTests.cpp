#include "support/EvaluatorFixture.h"

#include "runtime/Evaluator.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE_METHOD(EvaluatorFixture, "Is boolean", "[Procedures]") {
  SECTION("true for booleans") {
    REQUIRE(Value::True == evaluate("(boolean? #t)"));
    REQUIRE(Value::True == evaluate("(boolean? #f)"));
  }

  SECTION("false for other types") {
    REQUIRE(Value::False == evaluate("(boolean? 0)"));
    REQUIRE(Value::False == evaluate("(boolean? '(1 2))"));
    REQUIRE(Value::False == evaluate("(boolean? \"0\")"));
    REQUIRE(Value::False == evaluate("(boolean? #\\0)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(boolean?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(boolean? #t #t)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}
