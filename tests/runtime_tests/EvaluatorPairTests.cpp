#include "support/EvaluatorFixture.h"

#include "support/TestHelpers.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/Evaluator.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE_METHOD(EvaluatorFixture, "Is pair", "[Procedures]") {
  SECTION("true for pairs") {
    vmState_->globalEnvironment()->define(
        vmState_->makeSymbol("foo"), vmState_->makePair(Value{5}, Value{13}));
    vmState_->globalEnvironment()->define(
        vmState_->makeSymbol("bar"), vmState_->makePair(Value{1}, Value{-2}));

    REQUIRE(Value::True == evaluate("(pair? foo)"));
    REQUIRE(Value::True == evaluate("(pair? bar)"));

    REQUIRE(Value::True == evaluate("(pair? '(a . b))"));
    REQUIRE(Value::True == evaluate("(pair? '(a b c))"));
  }

  SECTION("false for other types") {
    REQUIRE(Value::False == evaluate("(pair? 13)"));
    REQUIRE(Value::False == evaluate("(pair? \"0\")"));
    REQUIRE(Value::False == evaluate("(pair? #\\0)"));
    REQUIRE(Value::False == evaluate("(pair? '())"));
    REQUIRE(Value::False == evaluate("(pair? #t)"));
    REQUIRE(Value::False == evaluate("(pair? #f)"));
    // REQUIRE(Value::True == evaluate("(pair? #(a b))"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(pair?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(pair? 3 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "cons", "[Procedures]") {
  SECTION("creates new pairs") {
    auto v = evaluate("(cons 'a '())");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("a" == car(v).toStringView());
    REQUIRE(Value::EmptyList == cdr(v));
  }

  SECTION("adds value to front of a list") {
    auto v = evaluate("(cons 'a '(b c d))");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("a" == car(v).toStringView());
    REQUIRE("b" == cadr(v).toStringView());
    REQUIRE("c" == caddr(v).toStringView());
    REQUIRE("d" == cadddr(v).toStringView());
    REQUIRE(Value::EmptyList == cddddr(v));
  }

  SECTION("different values in list") {
    auto v = evaluate("(cons \"a\" '(b c))");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("a" == car(v).toStringView());
    REQUIRE("b" == cadr(v).toStringView());
    REQUIRE("c" == caddr(v).toStringView());
    REQUIRE(Value::EmptyList == cdddr(v));
  }

  SECTION("improper list") {
    auto v = evaluate("(cons 'a 3)");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("a" == car(v).toStringView());
    REQUIRE(Value{3} == cdr(v));
  }

  SECTION("improper list 2") {
    auto v = evaluate("(cons '(a b) 'c)");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("a" == caar(v).toStringView());
    REQUIRE("b" == cadar(v).toStringView());
    REQUIRE(Value::EmptyList == cddar(v));
    REQUIRE("c" == cdr(v).toStringView());
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(cons)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(cons 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(cons 5 6 87)"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }

  // TODO: Test that returned pair is in fact unique.
}

TEST_CASE_METHOD(EvaluatorFixture, "car", "[Procedures]") {
  SECTION("list") {
    auto v = evaluate("(car '(a b c))");
    REQUIRE(ValueType::Symbol == v.type());
    REQUIRE("a" == v.toStringView());
  }

  SECTION("list with car being a pair") {
    auto v = evaluate("(car '((a) b c))");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("a" == car(v).toStringView());
    REQUIRE(Value::EmptyList == cdr(v));
  }

  SECTION("improper list") {
    auto v = evaluate("(car '(1 . 2))");
    REQUIRE(Value{1} == v);
  }

  SECTION("empty list is an error") {
    auto fn = [this]() { evaluate("(car '())"); };
    REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
  }

  SECTION("non pair type is an error") {
    auto fn1 = [this]() { evaluate("(car 1)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(car #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(car)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(car '(1) '(2))"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "cdr", "[Procedures]") {
  SECTION("list with car being a pair") {
    auto v = evaluate("(cdr '((a) b c d))");
    REQUIRE(ValueType::Pair == v.type());
    REQUIRE("b" == car(v).toStringView());
    REQUIRE("c" == cadr(v).toStringView());
    REQUIRE("d" == caddr(v).toStringView());
    REQUIRE(Value::EmptyList == cdddr(v));
  }

  SECTION("improper list") {
    auto v = evaluate("(cdr '(1 . 2))");
    REQUIRE(Value{2} == v);
  }

  SECTION("empty list is an error") {
    auto fn = [this]() { evaluate("(cdr '())"); };
    REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
  }

  SECTION("non pair type is an error") {
    auto fn1 = [this]() { evaluate("(cdr 1)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(cdr #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(cdr)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(cdr '(1) '(2))"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "set car", "[Procedures]") {
  SECTION("for improper pair") {
    evaluate("(define foo (1 . 2))");
    evaluate("(set-car! foo 3)");
    auto v = evaluate("foo");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{3} == car(v));
    REQUIRE(Value{2} == cdr(v));
  }

  SECTION("for one element list") {
    evaluate("(define fox (1))");
    evaluate("(set-car! fox 22)");
    auto v = evaluate("fox");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{22} == car(v));
    REQUIRE(Value::EmptyList == cdr(v));
  }

  SECTION("for two element list") {
    evaluate("(define fox (1 2))");
    evaluate("(set-car! fox #\\c)");
    auto v = evaluate("fox");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{'c'} == car(v));
    REQUIRE(Value{2} == cadr(v));
    REQUIRE(Value::EmptyList == cddr(v));
  }

  SECTION("for three element list") {
    evaluate("(define fox (1 2 3))");
    evaluate("(set-car! fox #\\F)");
    auto v = evaluate("fox");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{'F'} == car(v));
    REQUIRE(Value{2} == cadr(v));
    REQUIRE(Value{3} == caddr(v));
    REQUIRE(Value::EmptyList == cdddr(v));
  }

  SECTION("middle of list") {
    evaluate("(define hola (1 2 3))");
    evaluate("(set-car! (cdr hola) -5)");
    auto v = evaluate("hola");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{1} == car(v));
    REQUIRE(Value{-5} == cadr(v));
    REQUIRE(Value{3} == caddr(v));
    REQUIRE(Value::EmptyList == cdddr(v));
  }

  SECTION("empty list is an error") {
    auto fn = [this]() { evaluate("(set-car! '() 22)"); };
    REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
  }

  SECTION("non pair type is an error") {
    auto fn1 = [this]() { evaluate("(set-car! 1 22)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(set-car! #t 366)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(set-car!)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(set-car! '(1 2))"); };
    REQUIRE_THROWS_AS(fn2(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(set-car! '(1 2 3) 4 5)"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "set cdr", "[Procedures]") {
  SECTION("for improper pair") {
    evaluate("(define foo (1 . 2))");
    evaluate("(set-cdr! foo 3)");
    auto v = evaluate("foo");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{1} == car(v));
    REQUIRE(Value{3} == cdr(v));
  }

  SECTION("for one element list") {
    evaluate("(define fox (1))");
    evaluate("(set-cdr! fox 22)");
    auto v = evaluate("fox");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{1} == car(v));
    REQUIRE(Value{22} == cdr(v));
  }

  SECTION("for two element list") {
    evaluate("(define fox (1 2))");
    evaluate("(set-cdr! fox #\\c)");
    auto v = evaluate("fox");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{1} == car(v));
    REQUIRE(Value{'c'} == cdr(v));
  }

  SECTION("for three element list") {
    evaluate("(define fox (1 2 3))");
    evaluate("(set-cdr! fox #\\F)");
    auto v = evaluate("fox");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{1} == car(v));
    REQUIRE(Value{'F'} == cdr(v));
  }

  SECTION("middle of list") {
    evaluate("(define hola (1 2 3))");
    evaluate("(set-cdr! (cdr hola) -5)");
    auto v = evaluate("hola");

    REQUIRE(ValueType::Pair == v.type());
    REQUIRE(Value{1} == car(v));
    REQUIRE(Value{2} == cadr(v));
    REQUIRE(Value{-5} == cddr(v));
  }

  SECTION("empty list is an error") {
    auto fn = [this]() { evaluate("(set-cdr! '() 22)"); };
    REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
  }

  SECTION("non pair type is an error") {
    auto fn1 = [this]() { evaluate("(set-cdr! 1 22)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(set-cdr! #t 366)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(set-cdr!)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(set-cdr! '(1 2))"); };
    REQUIRE_THROWS_AS(fn2(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(set-cdr! '(1 2 3) 4 5)"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}
