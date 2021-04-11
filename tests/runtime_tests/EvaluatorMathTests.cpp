#include "support/EvaluatorFixture.h"

#include "runtime/Evaluator.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE_METHOD(EvaluatorFixture, "Test is number", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(number? 0)"));
    REQUIRE(Value::True == evaluate("(number? 1)"));
    REQUIRE(Value::True == evaluate("(number? -1)"));
    REQUIRE(Value::True == evaluate("(number? 1000)"));
    REQUIRE(Value::True == evaluate("(number? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(number? \"0\")"));
    REQUIRE(Value::False == evaluate("(number? #\\0)"));
    REQUIRE(Value::False == evaluate("(number? '())"));
    REQUIRE(Value::False == evaluate("(number? #t)"));
    REQUIRE(Value::False == evaluate("(number? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(number?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(number? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is complex", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(complex? 0)"));
    REQUIRE(Value::True == evaluate("(complex? 1)"));
    REQUIRE(Value::True == evaluate("(complex? -1)"));
    REQUIRE(Value::True == evaluate("(complex? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(complex? \"0\")"));
    REQUIRE(Value::False == evaluate("(complex? #\\0)"));
    REQUIRE(Value::False == evaluate("(complex? '())"));
    REQUIRE(Value::False == evaluate("(complex? #t)"));
    REQUIRE(Value::False == evaluate("(complex? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(complex?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(complex? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is real", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(real? 0)"));
    REQUIRE(Value::True == evaluate("(real? 1)"));
    REQUIRE(Value::True == evaluate("(real? -1)"));
    REQUIRE(Value::True == evaluate("(real? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(real? \"0\")"));
    REQUIRE(Value::False == evaluate("(real? #\\0)"));
    REQUIRE(Value::False == evaluate("(real? '())"));
    REQUIRE(Value::False == evaluate("(real? #t)"));
    REQUIRE(Value::False == evaluate("(real? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(real?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(real? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is rational", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(rational? 0)"));
    REQUIRE(Value::True == evaluate("(rational? 1)"));
    REQUIRE(Value::True == evaluate("(rational? -1)"));
    REQUIRE(Value::True == evaluate("(rational? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(rational? \"0\")"));
    REQUIRE(Value::False == evaluate("(rational? #\\0)"));
    REQUIRE(Value::False == evaluate("(rational? '())"));
    REQUIRE(Value::False == evaluate("(rational? #t)"));
    REQUIRE(Value::False == evaluate("(rational? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(rational?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(rational? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is integer", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(integer? 0)"));
    REQUIRE(Value::True == evaluate("(integer? 1)"));
    REQUIRE(Value::True == evaluate("(integer? -1)"));
    REQUIRE(Value::True == evaluate("(integer? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(integer? \"0\")"));
    REQUIRE(Value::False == evaluate("(integer? #\\0)"));
    REQUIRE(Value::False == evaluate("(integer? '())"));
    REQUIRE(Value::False == evaluate("(integer? #t)"));
    REQUIRE(Value::False == evaluate("(integer? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(integer?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(integer? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is Exact", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(exact? 0)"));
    REQUIRE(Value::True == evaluate("(exact? 1000000)"));
    REQUIRE(Value::True == evaluate("(exact? -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(exact? #\\3)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(exact? \"12\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(exact?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(exact? 1 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is Inexact", "[Procedures]") {
  SECTION("returns false for fixnums") {
    REQUIRE(Value::False == evaluate("(inexact? 0)"));
    REQUIRE(Value::False == evaluate("(inexact? 1000000)"));
    REQUIRE(Value::False == evaluate("(inexact? -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(inexact? #\\3)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(inexact? \"12\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(inexact?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(inexact? 1 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is Exact Integer", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(exact-integer? 0)"));
    REQUIRE(Value::True == evaluate("(exact-integer? 1000000)"));
    REQUIRE(Value::True == evaluate("(exact-integer? -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(exact-integer? #\\3)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(exact-integer? \"12\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(exact-integer?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(exact-integer? 1 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is Finite", "[Procedures]") {
  SECTION("returns true for fixnums") {
    REQUIRE(Value::True == evaluate("(finite? 0)"));
    REQUIRE(Value::True == evaluate("(finite? 1000000)"));
    REQUIRE(Value::True == evaluate("(finite? -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(finite? #\\3)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(finite? \"12\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(finite?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(finite? 1 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is Infinite", "[Procedures]") {
  SECTION("returns false for fixnums") {
    REQUIRE(Value::False == evaluate("(infinite? 0)"));
    REQUIRE(Value::False == evaluate("(infinite? 1000000)"));
    REQUIRE(Value::False == evaluate("(infinite? -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(infinite? #\\3)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(infinite? \"12\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(infinite?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(infinite? 1 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Is NaN", "[Procedures]") {
  SECTION("returns false for fixnums") {
    REQUIRE(Value::False == evaluate("(nan? 0)"));
    REQUIRE(Value::False == evaluate("(nan? 1000000)"));
    REQUIRE(Value::False == evaluate("(nan? -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(nan? #\\3)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(nan? \"12\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(nan?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(nan? 1 2)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is zero", "[Procedures]") {
  SECTION("returns true for zero fixnums") {
    REQUIRE(Value::True == evaluate("(zero? 0)"));
  }

  SECTION("returns false for non-zero fixnums") {
    REQUIRE(Value::False == evaluate("(zero? 1)"));
    REQUIRE(Value::False == evaluate("(zero? -10000)"));
    REQUIRE(Value::False == evaluate("(zero? 512)"));
  }

  SECTION("returns false for any non fixnum type") {
    REQUIRE(Value::False == evaluate("(zero? \"0\")"));
    REQUIRE(Value::False == evaluate("(zero? #\\0)"));
    REQUIRE(Value::False == evaluate("(zero? '())"));
    REQUIRE(Value::False == evaluate("(zero? #t)"));
    REQUIRE(Value::False == evaluate("(zero? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(zero?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(zero? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is positive", "[Procedures]") {
  SECTION("returns true for positive fixnums") {
    REQUIRE(Value::True == evaluate("(positive? 1)"));
    REQUIRE(Value::True == evaluate("(positive? 2)"));
    REQUIRE(Value::True == evaluate("(positive? 1000000)"));
  }

  SECTION("returns false for non positive fixnums") {
    REQUIRE(Value::False == evaluate("(positive? 0)"));
    REQUIRE(Value::False == evaluate("(positive? -1)"));
    REQUIRE(Value::False == evaluate("(positive? -1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(positive? \"0\")"));
    REQUIRE(Value::False == evaluate("(positive? #\\0)"));
    REQUIRE(Value::False == evaluate("(positive? '())"));
    REQUIRE(Value::False == evaluate("(positive? #t)"));
    REQUIRE(Value::False == evaluate("(positive? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(positive?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(positive? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is negative", "[Procedures]") {
  SECTION("returns true for negative fixnums") {
    REQUIRE(Value::True == evaluate("(negative? -1)"));
    REQUIRE(Value::True == evaluate("(negative? -2)"));
    REQUIRE(Value::True == evaluate("(negative? -1000000)"));
  }

  SECTION("returns true for non negative fixnums") {
    REQUIRE(Value::False == evaluate("(negative? 0)"));
    REQUIRE(Value::False == evaluate("(negative? 1)"));
    REQUIRE(Value::False == evaluate("(negative? 2)"));
    REQUIRE(Value::False == evaluate("(negative? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(negative? \"0\")"));
    REQUIRE(Value::False == evaluate("(negative? #\\0)"));
    REQUIRE(Value::False == evaluate("(negative? '())"));
    REQUIRE(Value::False == evaluate("(negative? #t)"));
    REQUIRE(Value::False == evaluate("(negative? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(negative?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(negative? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is even", "[Procedures]") {
  SECTION("returns true for even fixnums") {
    REQUIRE(Value::True == evaluate("(even? 0)"));
    REQUIRE(Value::True == evaluate("(even? 2)"));
    REQUIRE(Value::True == evaluate("(even? -2)"));
    REQUIRE(Value::True == evaluate("(even? -36)"));
    REQUIRE(Value::True == evaluate("(even? 1000000)"));
  }

  SECTION("returns true for odd fixnums") {
    REQUIRE(Value::False == evaluate("(even? 1)"));
    REQUIRE(Value::False == evaluate("(even? 3)"));
    REQUIRE(Value::False == evaluate("(even? -1)"));
    REQUIRE(Value::False == evaluate("(even? -5)"));
    REQUIRE(Value::False == evaluate("(even? 1000007)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(even? \"0\")"));
    REQUIRE(Value::False == evaluate("(even? #\\0)"));
    REQUIRE(Value::False == evaluate("(even? '())"));
    REQUIRE(Value::False == evaluate("(even? #t)"));
    REQUIRE(Value::False == evaluate("(even? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(even?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(even? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test is odd", "[Procedures]") {
  SECTION("returns true for odd fixnums") {
    REQUIRE(Value::True == evaluate("(odd? 1)"));
    REQUIRE(Value::True == evaluate("(odd? 3)"));
    REQUIRE(Value::True == evaluate("(odd? -1)"));
    REQUIRE(Value::True == evaluate("(odd? -5)"));
    REQUIRE(Value::True == evaluate("(odd? 1000007)"));
  }

  SECTION("returns true for even fixnums") {
    REQUIRE(Value::False == evaluate("(odd? 0)"));
    REQUIRE(Value::False == evaluate("(odd? 2)"));
    REQUIRE(Value::False == evaluate("(odd? -2)"));
    REQUIRE(Value::False == evaluate("(odd? -36)"));
    REQUIRE(Value::False == evaluate("(odd? 1000000)"));
  }

  SECTION("returns false for any non numeric type") {
    REQUIRE(Value::False == evaluate("(odd? \"0\")"));
    REQUIRE(Value::False == evaluate("(odd? #\\0)"));
    REQUIRE(Value::False == evaluate("(odd? '())"));
    REQUIRE(Value::False == evaluate("(odd? #t)"));
    REQUIRE(Value::False == evaluate("(odd? #f)"));
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(odd?)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(odd? 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test number equal", "[Procedures]") {
  SECTION("returns true for fixnums equal in value") {
    REQUIRE(Value::True == evaluate("(=)"));
    REQUIRE(Value::True == evaluate("(= 1)"));
    REQUIRE(Value::True == evaluate("(= 1)"));
    REQUIRE(Value::True == evaluate("(= 5 5)"));
    REQUIRE(Value::True == evaluate("(= 4 4 (* 2 2))"));
  }

  SECTION("returns false for fixnums not equal in value") {
    REQUIRE(Value::False == evaluate("(= 5 2)"));
    REQUIRE(Value::False == evaluate("(= 8 8 8 8 1)"));
  }

  SECTION("throws exception for non numeric types") {
    auto fn1 = [this]() { evaluate("(= \"0\")"); };
    auto fn2 = [this]() { evaluate("(= #\\0 0)"); };
    auto fn3 = [this]() { evaluate("(= #f #f)"); };

    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn3(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test number less", "[Procedures]") {
  SECTION("returns true for fixnums less in value") {
    REQUIRE(Value::True == evaluate("(<)"));
    REQUIRE(Value::True == evaluate("(< 1)"));
    REQUIRE(Value::True == evaluate("(< 3 5)"));
    REQUIRE(Value::True == evaluate("(< -2 4 9)"));
  }

  SECTION("returns false for fixnums not less in value") {
    REQUIRE(Value::False == evaluate("(< 5 3)"));
    REQUIRE(Value::False == evaluate("(< 4 4 5)"));
    REQUIRE(Value::False == evaluate("(< 4 3 4)"));
  }

  SECTION("throws exception for non numeric types") {
    auto fn1 = [this]() { evaluate("(< \"0\")"); };
    auto fn2 = [this]() { evaluate("(< #\\0 0)"); };
    auto fn3 = [this]() { evaluate("(< #f #f)"); };

    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn3(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test number less equal", "[Procedures]") {
  SECTION("returns true for fixnums less equal in value") {
    REQUIRE(Value::True == evaluate("(<=)"));
    REQUIRE(Value::True == evaluate("(<= 1)"));
    REQUIRE(Value::True == evaluate("(<= 3 3)"));
    REQUIRE(Value::True == evaluate("(<= -2 1 1)"));
  }

  SECTION("returns false for fixnums not less equal in value") {
    REQUIRE(Value::False == evaluate("(<= 6 0)"));
    REQUIRE(Value::False == evaluate("(<= 3 4 3)"));
    REQUIRE(Value::False == evaluate("(<= 3 6 5)"));
  }

  SECTION("throws exception for non numeric types") {
    auto fn1 = [this]() { evaluate("(<= \"0\")"); };
    auto fn2 = [this]() { evaluate("(<= #\\0 0)"); };
    auto fn3 = [this]() { evaluate("(<= #f #f)"); };

    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn3(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Test number greater", "[Procedures]") {
  SECTION("returns true for fixnums greater in value") {
    REQUIRE(Value::True == evaluate("(>)"));
    REQUIRE(Value::True == evaluate("(> 1)"));
    REQUIRE(Value::True == evaluate("(> 10 5)"));
    REQUIRE(Value::True == evaluate("(> 10 3 -1)"));
  }

  SECTION("returns false for fixnums not greater in value") {
    REQUIRE(Value::False == evaluate("(> 2 3)"));
    REQUIRE(Value::False == evaluate("(> 3 2 2)"));
    REQUIRE(Value::False == evaluate("(> 3 1 2)"));
  }

  SECTION("throws exception for non numeric types") {
    auto fn1 = [this]() { evaluate("(> \"0\")"); };
    auto fn2 = [this]() { evaluate("(> #\\0 0)"); };
    auto fn3 = [this]() { evaluate("(> #f #f)"); };

    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn3(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(
    EvaluatorFixture,
    "Test number greater equal",
    "[Procedures]") {
  SECTION("returns true for fixnums greater or equal in value") {
    REQUIRE(Value::True == evaluate("(>=)"));
    REQUIRE(Value::True == evaluate("(>= 1)"));
    REQUIRE(Value::True == evaluate("(>= 10 5)"));
    REQUIRE(Value::True == evaluate("(>= 8 8 3)"));
  }

  SECTION("returns false for fixnums not greater or equal in value") {
    REQUIRE(Value::False == evaluate("(>= 2 3)"));
    REQUIRE(Value::False == evaluate("(>= 3 2 3)"));
    REQUIRE(Value::False == evaluate("(>= 3 1 2)"));
  }

  SECTION("throws exception for non numeric types") {
    auto fn1 = [this]() { evaluate("(>= \"0\")"); };
    auto fn2 = [this]() { evaluate("(>= #\\0 0)"); };
    auto fn3 = [this]() { evaluate("(>= #f #f)"); };

    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
    REQUIRE_THROWS_AS(fn3(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Addition procedure", "[Procedures]") {
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

TEST_CASE_METHOD(EvaluatorFixture, "Subtract procedure", "[Procedures]") {
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

  SECTION("error if zero arguments") {
    auto fn1 = [this]() { evaluate("(-)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Multiply procedure", "[Procedures]") {
  SECTION("multiplies all passed fixnums") {
    REQUIRE(Value{1} == evaluate("(*)"));
    REQUIRE(Value{3} == evaluate("(* 1 3)"));
    REQUIRE(Value{-6} == evaluate("(* 2 -3 1)"));
  }

  SECTION("error if any argument is not a fixnum") {
    auto fn1 = [this]() { evaluate("(* \"33\" 1 2)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(* 10 1 #\\1)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Divide procedure", "[Procedures]") {
  SECTION("error with no arguments") {
    auto fn = [this]() { evaluate("(/)"); };
    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
  }

  SECTION("with one argument inverts") {
    REQUIRE(Value{1} == evaluate("(/ 1)"));
    REQUIRE(Value{1 / 2} == evaluate("(/ 2)"));
    REQUIRE(Value{1 / -5} == evaluate("(/ -5)"));
  }

  SECTION("with two or more arguments divides all") {
    REQUIRE(Value{2} == evaluate("(/ 4 2)"));
    REQUIRE(Value{-8} == evaluate("(/ 24 -3 1)"));
    REQUIRE(Value{5} == evaluate("(/ 16 3)"));
    REQUIRE(Value{0} == evaluate("(/ 0 3)"));
  }

  SECTION("with zero") {
    // TODO: Handle exception when its added to div.
    REQUIRE(Value{0} == evaluate("(/ 0)"));
    REQUIRE(Value{0} == evaluate("(/ 2 0)"));
    REQUIRE(Value{0} == evaluate("(/ 3 2 0 1)"));
  }

  SECTION("error if any argument is not a fixnum") {
    auto fn1 = [this]() { evaluate("(/ \"3\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(/ 2 \"3\")"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if zero arguments") {
    auto fn1 = [this]() { evaluate("(/)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Absolute value", "[Procedures]") {
  SECTION("returns same fixnum for zero and positive integers") {
    REQUIRE(Value{0} == evaluate("(abs 0)"));
    REQUIRE(Value{1} == evaluate("(abs 1)"));
    REQUIRE(Value{2} == evaluate("(abs 2)"));
    REQUIRE(Value{1000000} == evaluate("(abs 1000000)"));
  }

  SECTION("returns positive fixnum for negative integers") {
    REQUIRE(Value{1} == evaluate("(abs -1)"));
    REQUIRE(Value{2} == evaluate("(abs -2)"));
    REQUIRE(Value{1000000} == evaluate("(abs -1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(abs \"33\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(abs #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(abs)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(abs 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Floor quotient", "[Procedures]") {
  // TODO: Test error divide by zero.

  SECTION("returns floored quotient for fixnums with correct sign") {
    REQUIRE(Value{2} == evaluate("(floor-quotient 5 2)"));
    REQUIRE(Value{-3} == evaluate("(floor-quotient -5 2)"));
    REQUIRE(Value{-3} == evaluate("(floor-quotient 5 -2)"));
    REQUIRE(Value{2} == evaluate("(floor-quotient -5 -2)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(floor-quotient #\\3 \"33\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(floor-quotient 4 #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(floor-quotient)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(floor-quotient 1 2)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(floor-quotient 0 1 2)"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Truncate quotient", "[Procedures]") {
  // Also tests for aliased procedure "quotient".
  // TODO: Test error divide by zero.

  SECTION("returns truncated quotient for fixnums with correct sign") {
    REQUIRE(Value{2} == evaluate("(truncate-quotient 5 2)"));
    REQUIRE(Value{2} == evaluate("(quotient 5 2)"));

    REQUIRE(Value{-2} == evaluate("(truncate-quotient -5 2)"));
    REQUIRE(Value{-2} == evaluate("(quotient -5 2)"));

    REQUIRE(Value{-2} == evaluate("(truncate-quotient 5 -2)"));
    REQUIRE(Value{-2} == evaluate("(quotient 5 -2)"));

    REQUIRE(Value{2} == evaluate("(truncate-quotient -5 -2)"));
    REQUIRE(Value{2} == evaluate("(quotient -5 -2)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(truncate-quotient #\\3 \"33\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(truncate-quotient 4 #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(truncate-quotient)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(truncate-quotient 1 2)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(truncate-quotient 0 1 2)"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Truncate remainder", "[Procedures]") {
  // Also tests for aliased procedure "remainder".
  // TODO: Test error divide by zero.

  SECTION("returns truncated quotient for fixnums with correct sign") {
    REQUIRE(Value{1} == evaluate("(truncate-remainder 5 2)"));
    REQUIRE(Value{1} == evaluate("(remainder 5 2)"));

    REQUIRE(Value{-1} == evaluate("(truncate-remainder -5 2)"));
    REQUIRE(Value{-1} == evaluate("(remainder -5 2)"));

    REQUIRE(Value{1} == evaluate("(truncate-remainder 5 -2)"));
    REQUIRE(Value{1} == evaluate("(remainder 5 -2)"));

    REQUIRE(Value{-1} == evaluate("(truncate-remainder -5 -2)"));
    REQUIRE(Value{-1} == evaluate("(remainder -5 -2)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(truncate-remainder #\\3 \"33\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(truncate-remainder 4 #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(truncate-remainder)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(truncate-remainder 1 2)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn3 = [this]() { evaluate("(truncate-remainder 0 1 2)"); };
    REQUIRE_THROWS_AS(fn3(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Floor", "[Procedures]") {
  SECTION("returns the same value for all fixnums") {
    REQUIRE(Value{0} == evaluate("(floor 0)"));
    REQUIRE(Value{1} == evaluate("(floor 1)"));
    REQUIRE(Value{-2} == evaluate("(floor -2)"));
    REQUIRE(Value{1000000} == evaluate("(floor 1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(floor \"42\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(floor #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(floor)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(floor 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Ceiling", "[Procedures]") {
  SECTION("returns the same value for all fixnums") {
    REQUIRE(Value{0} == evaluate("(ceiling 0)"));
    REQUIRE(Value{1} == evaluate("(ceiling 1)"));
    REQUIRE(Value{-2} == evaluate("(ceiling -2)"));
    REQUIRE(Value{1000000} == evaluate("(ceiling 1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(ceiling \"42\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(ceiling #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(ceiling)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(ceiling 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Truncate", "[Procedures]") {
  SECTION("returns the same value for all fixnums") {
    REQUIRE(Value{0} == evaluate("(truncate 0)"));
    REQUIRE(Value{1} == evaluate("(truncate 1)"));
    REQUIRE(Value{-2} == evaluate("(truncate -2)"));
    REQUIRE(Value{1000000} == evaluate("(truncate 1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(truncate \"42\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(truncate #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(truncate)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(truncate 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "Round", "[Procedures]") {
  SECTION("returns the same value for all fixnums") {
    REQUIRE(Value{0} == evaluate("(round 0)"));
    REQUIRE(Value{1} == evaluate("(round 1)"));
    REQUIRE(Value{-2} == evaluate("(round -2)"));
    REQUIRE(Value{1000000} == evaluate("(round 1000000)"));
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(round \"42\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(round #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(round)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(round 0 1)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "number to string", "[Procedures]") {
  SECTION("returns the equiv printable fixnum") {
    REQUIRE("\"0\"" == evaluate("(number->string 0)").toString());
    REQUIRE("\"10\"" == evaluate("(number->string 10)").toString());
    REQUIRE("\"-3\"" == evaluate("(number->string -3)").toString());
    REQUIRE("\"67921\"" == evaluate("(number->string 67921)").toString());
    REQUIRE(
        "\"2147483647\"" == evaluate("(number->string 2147483647)").toString());
    REQUIRE(
        "\"-2147483648\"" ==
        evaluate("(number->string -2147483648)").toString());
  }

  SECTION("error if argument is not a numeric type") {
    auto fn1 = [this]() { evaluate("(number->string \"42\")"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(number->string #t)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(number->string)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(number->string 4 5 6)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}

TEST_CASE_METHOD(EvaluatorFixture, "string to number", "[Procedures]") {
  SECTION("returns a fixnum for a integer in a string") {
    REQUIRE(Value{0} == evaluate("(string->number \"0\")"));
    REQUIRE(Value{10} == evaluate("(string->number \"10\")"));
    REQUIRE(Value{-3} == evaluate("(string->number \"-3\")"));
    REQUIRE(Value{67921} == evaluate("(string->number \"67921\")"));
    REQUIRE(Value{2147483647} == evaluate("(string->number \"2147483647\")"));
    REQUIRE(
        Value{(fixnum_t)-2147483648} ==
        evaluate("(string->number \"-2147483648\")"));
  }

  SECTION("returns false for strings that are not numbers") {
    REQUIRE(Value::False == evaluate("(string->number \"\")"));
    REQUIRE(Value::False == evaluate("(string->number \"hello\")"));
    REQUIRE(Value::False == evaluate("(string->number \"z3\")"));
    REQUIRE(Value::False == evaluate("(string->number \" -4\")"));
  }

  SECTION("error if argument is not a string type") {
    auto fn1 = [this]() { evaluate("(string->number 42)"); };
    REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);

    auto fn2 = [this]() { evaluate("(string->number #f)"); };
    REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
  }

  SECTION("error if wrong argument count") {
    auto fn1 = [this]() { evaluate("(string->number)"); };
    REQUIRE_THROWS_AS(fn1(), ArgumentMissingException);

    auto fn2 = [this]() { evaluate("(string->number \"4\" 5 6)"); };
    REQUIRE_THROWS_AS(fn2(), ArgCountMismatch);
  }
}
