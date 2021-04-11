#include "support/EvaluatorFixture.h"

#include "runtime/Evaluator.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE_METHOD(EvaluatorFixture, "Is empty list", "[Procedures]") {
  SECTION("true for empty list") {
    REQUIRE(Value::True == evaluate("(empty-list? '())"));
  }

  SECTION("false for other types") {
    REQUIRE(Value::False == evaluate("(empty-list? '(1 2))"));
    REQUIRE(Value::False == evaluate("(empty-list? \"0\")"));
    REQUIRE(Value::False == evaluate("(empty-list? #\\0)"));
    REQUIRE(Value::False == evaluate("(empty-list? #t)"));
    REQUIRE(Value::False == evaluate("(empty-list? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(empty-list?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(empty-list? '() 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is null", "[Procedures]") {
  SECTION("true for empty list") {
    REQUIRE(Value::True == evaluate("(null? '())"));
  }

  SECTION("false for other types") {
    REQUIRE(Value::False == evaluate("(null? '(1 2))"));
    REQUIRE(Value::False == evaluate("(null? \"0\")"));
    REQUIRE(Value::False == evaluate("(null? #\\0)"));
    REQUIRE(Value::False == evaluate("(null? #t)"));
    REQUIRE(Value::False == evaluate("(null? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(null?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(null? '() 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is list", "[Procedures]") {
  SECTION("true for lists") {
    REQUIRE(Value::True == evaluate("(list? '())"));
    REQUIRE(Value::True == evaluate("(list? '(a b))"));
    REQUIRE(Value::True == evaluate("(list? '(a b c))"));
  }

  SECTION("false for other types") {
    REQUIRE(Value::False == evaluate("(list? '(a . b))"));
    REQUIRE(Value::False == evaluate("(list? \"0\")"));
    REQUIRE(Value::False == evaluate("(list? #\\0)"));
    REQUIRE(Value::False == evaluate("(list? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(list?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(list? '() 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "New list", "[Procedures]") {
  // (list) => ().
  REQUIRE(Value::EmptyList == evaluate("(list)"));

  // (list 1) => (1).
  auto v = evaluate("(list 1)");
  REQUIRE(Value{1} == car(v));
  REQUIRE(Value::EmptyList == cdr(v));

  // (list #\c '(2 3)) => (#\c (2 3)).
  v = evaluate("(list #\\c '(2 3))");
  REQUIRE(Value{'c'} == car(v));
  REQUIRE(Value{2} == car(cadr(v)));
  REQUIRE(Value{3} == cadr(cadr(v)));
  REQUIRE(Value::EmptyList == cddr(cadr(v)));
  REQUIRE(Value::EmptyList == cddr(v));

  // (list 'a (+ 3 4) 'c) => (a 7 c).
  v = evaluate("(list 'a (+ 3 4) 'c)");
  REQUIRE("a" == car(v).toStringView());
  REQUIRE(Value{7} == cadr(v));
  REQUIRE("c" == caddr(v).toStringView());
  REQUIRE(Value::EmptyList == cdddr(v));
}
