#include "support/EvaluatorFixture.h"

#include "runtime/Evaluator.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

using namespace Shiny;

namespace {
  constexpr const char* kTestCounterProc = "test-counter";
  constexpr const char* kTestUpProc = "test-up";
  constexpr const char* kTestAdd2Proc = "test-add2";
  constexpr const char* kTestTryModParamProc = "test-mod-param";
  constexpr const char* kTestTryModPairProc = "test-mod-pair";
} // namespace

class EvaluatorProcedureFixture : public EvaluatorFixture {
protected:
public:
  EvaluatorProcedureFixture() : EvaluatorFixture() {
    defineProc(kTestCounterProc, &testCounter_proc);
    defineProc(kTestUpProc, &testUp_proc);
    defineProc(kTestAdd2Proc, &testAdd2_proc);
    defineProc(kTestTryModParamProc, &testTryModParam_proc);
    defineProc(kTestTryModPairProc, &testTryModPair_proc);
  }

protected:
  static Value testCounter_proc(ArgList&, VmState&, Environment&) {
    incProcCallCount(kTestCounterProc);
    return Value{42};
  }

  static Value testUp_proc(ArgList& args, VmState&, Environment&) {
    incProcCallCount(kTestUpProc);
    auto arg = popArgumentOrThrow(args, ValueType::Character);
    char c = std::toupper(arg.toChar());
    auto result = Value{c};
    return result;
  }

  static Value testAdd2_proc(ArgList& args, VmState&, Environment&) {
    incProcCallCount(kTestAdd2Proc);
    auto a = popArgumentOrThrow(args, ValueType::Fixnum);
    auto b = popArgumentOrThrow(args, ValueType::Fixnum);
    return Value{a.toFixnum() + b.toFixnum()};
  }

  static Value testTryModParam_proc(ArgList& args, VmState&, Environment&) {
    incProcCallCount(kTestTryModParamProc);
    auto a = popArgumentOrThrow(args, ValueType::Fixnum);
    a = Value{a.toFixnum() + 1};
    return a;
  }

  static Value testTryModPair_proc(ArgList& args, VmState&, Environment&) {
    incProcCallCount(kTestTryModPairProc);
    auto a = popArgumentOrThrow(args, ValueType::Pair);
    set_car(a, Value{2222});
    return Value{1};
  }
};

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Can call simple no argument procedure",
    "[Evaluator]") {
  REQUIRE(0 == getProcCallCount(kTestCounterProc));

  evaluate("(test-counter)");
  REQUIRE(1 == getProcCallCount(kTestCounterProc));

  evaluate("(test-counter)");
  REQUIRE(2 == getProcCallCount(kTestCounterProc));
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Can call procedure with single argument and receive return value",
    "[Evaluator]") {
  REQUIRE(0 == getProcCallCount(kTestUpProc));

  REQUIRE(Value{'J'} == evaluate("(test-up #\\j)"));
  REQUIRE(1 == getProcCallCount(kTestUpProc));

  REQUIRE(Value{'K'} == evaluate("(test-up #\\K)"));
  REQUIRE(2 == getProcCallCount(kTestUpProc));
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Can call procedure with two arguments and receive return value",
    "[Evaluator]") {
  REQUIRE(0 == getProcCallCount(kTestAdd2Proc));

  REQUIRE(Value{6} == evaluate("(test-add2 5 1)"));
  REQUIRE(1 == getProcCallCount(kTestAdd2Proc));

  REQUIRE(Value{22} == evaluate("(test-add2 25 -3)"));
  REQUIRE(2 == getProcCallCount(kTestAdd2Proc));
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Arguments are evaluated before calling procedure",
    "[Evaluator]") {
  vmState_->environment().defineVariable(vmState_->makeSymbol("a"), Value{10});
  vmState_->environment().defineVariable(vmState_->makeSymbol("b"), Value{2});

  REQUIRE(Value{12} == evaluate("(test-add2 a b)"));
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Throws exception if procedure does not exist",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(does-not-exit)"); };
  REQUIRE_THROWS_AS(fn(), UnboundVariableException);
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Throws exception if operator not a procedure",
    "[Evaluator]") {
  vmState_->environment().defineVariable(
      vmState_->makeSymbol("foo"), Value{42});
  auto fn = [this]() { evaluate("(foo)"); };
  REQUIRE_THROWS_AS(fn(), Exception);
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Throws exception if argument type is not expected",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(test-up 2)"); };
  REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Throws exception if too few arguments provided",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(test-add2 2)"); };
  REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Throws exception if too too many arguments provided",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(test-add2 2 1 5)"); };
  REQUIRE_THROWS_AS(fn(), ArgCountMismatch);
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Cannot modify atomic values when passed to a function",
    "[Evaluator]") {
  vmState_->environment().defineVariable(vmState_->makeSymbol("a"), Value{10});
  evaluate("(test-mod-param a)");
  auto a = vmState_->environment().lookupVariable(vmState_->makeSymbol("a"));
  REQUIRE(10 == a.toFixnum());
}

TEST_CASE_METHOD(
    EvaluatorProcedureFixture,
    "Arguments are shallow copied when evaluated",
    "[Evaluator]") {
  vmState_->environment().defineVariable(
      vmState_->makeSymbol("a"), vmState_->makePair(Value{-5}, Value{6}));
  evaluate("(test-mod-pair a)");
  auto a = vmState_->environment().lookupVariable(vmState_->makeSymbol("a"));
  REQUIRE(2222 == a.toRawPair()->car.toFixnum());
}
