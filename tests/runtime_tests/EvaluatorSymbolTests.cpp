#include "support/EvaluatorFixture.h"

#include "runtime/Evaluator.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE_METHOD(EvaluatorFixture, "Is symbol", "[Procedures]") {
  SECTION("true for symbols") {
    REQUIRE(Value::True == evaluate("(symbol? 'foo)"));
    REQUIRE(Value::True == evaluate("(symbol? (car '(a b)))"));
    REQUIRE(Value::True == evaluate("(symbol? 'nil)"));
  }

  SECTION("false for other types") {
    REQUIRE(Value::False == evaluate("(symbol? \"bar\")"));
    REQUIRE(Value::False == evaluate("(symbol? '())"));
    REQUIRE(Value::False == evaluate("(symbol? #f)"));

    REQUIRE(Value::False == evaluate("(symbol? 0)"));
    REQUIRE(Value::False == evaluate("(symbol? '(1 2))"));
    REQUIRE(Value::False == evaluate("(symbol? #\\0)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(symbol?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(symbol? 'foo 'bar)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Symbol to string", "[Procedures]") {
  SECTION("returns name") {
    REQUIRE(
        "\"flying-fish\"" ==
        evaluate("(symbol->string 'flying-fish)").toString());
    REQUIRE("\"Martin\"" == evaluate("(symbol->string 'Martin)").toString());
    REQUIRE("\"foo\"" == evaluate("(symbol->string 'foo)").toString());
  }

  SECTION("error if not a symbol") {
    auto fn1 = [this]() { evaluate("(symbol->string \"foo\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(symbol->string #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(symbol->string)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(symbol->string 'foo 'bar)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "String to symbol", "[Procedures]") {
  SECTION("returns symbol") {
    auto v = evaluate("(string->symbol \"foo\")");
    REQUIRE(ValueType::Symbol == v.type());
    REQUIRE("foo" == v.toStringView());

    v = evaluate("(string->symbol \"flying-fish\")");
    REQUIRE(ValueType::Symbol == v.type());
    REQUIRE("flying-fish" == v.toStringView());

    v = evaluate("(string->symbol \"Marvin\")");
    REQUIRE(ValueType::Symbol == v.type());
    REQUIRE("Marvin" == v.toStringView());
  }

  SECTION("error if not a string") {
    auto fn1 = [this]() { evaluate("(string->symbol 'foo)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(string->symbol #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(string->symbol)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(string->symbol \"foo\" \"bar\")"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}
