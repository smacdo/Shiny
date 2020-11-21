#include "runtime/Evaluator.h"

#include "runtime/Reader.h"
#include "runtime/RuntimeApi.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include "support/TestHelpers.h"

#include <catch2/catch.hpp>

#include <iostream>

#include <map>
#include <string>

using namespace Shiny;

namespace {
  constexpr const char* kTestCounterProc = "test-counter";
  constexpr const char* kTestUpProc = "test-up";
  constexpr const char* kTestAdd2Proc = "test-add2";
} // namespace

class EvaluatorTestFixture {
protected:
  static std::map<std::string, int> procCallCounter;
  std::shared_ptr<VmState> vmState_;

public:
  EvaluatorTestFixture()
      : vmState_(
            std::make_shared<VmState>(std::make_unique<MallocAllocator>())) {
    procCallCounter.clear();

    defineProc(kTestCounterProc, &testCounter_proc);
    defineProc(kTestUpProc, &testUp_proc);
    defineProc(kTestAdd2Proc, &testAdd2_proc);
  }

protected:
  VmState& vmState() { return *vmState_.get(); }

  Value evaluate(std::string_view code) {
    return Shiny::evaluate(code, vmState_);
  }

  void defineProc(const std::string& name, procedure_t proc) {
    REQUIRE(nullptr != proc);

    // define call counter.
    REQUIRE(procCallCounter.find(name) == procCallCounter.end());
    procCallCounter[name] = 0;

    // register.
    vmState_->environment().defineVariable(
        vmState_->makeSymbol(name), Value{proc});
  }

protected:
  static int getProcCallCount(const std::string& name) {
    return procCallCounter.at(name);
  }

private:
  static void incProcCallCount(const std::string& name) {
    procCallCounter.at(name) += 1;
  }

protected:
  static Value testCounter_proc(Value, VmState&, Environment&) {
    incProcCallCount(kTestCounterProc);
    return Value{42};
  }

  static Value testUp_proc(Value args, VmState&, Environment&) {
    incProcCallCount(kTestUpProc);
    auto arg = popArgumentOrThrow(args, nullptr, ValueType::Character);
    char c = std::toupper(arg.toChar());
    auto result = Value{c};
    return result;
  }

  static Value testAdd2_proc(Value args, VmState&, Environment&) {
    incProcCallCount(kTestAdd2Proc);
    auto a = popArgumentOrThrow(args, &args, ValueType::Fixnum);
    auto b = popArgumentOrThrow(args, &args, ValueType::Fixnum);
    return Value{a.toFixnum() + b.toFixnum()};
  }
};

std::map<std::string, int> EvaluatorTestFixture::procCallCounter;

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Can call simple no argument procedure",
    "[Evaluator]") {
  REQUIRE(0 == getProcCallCount(kTestCounterProc));

  evaluate("(test-counter)");
  REQUIRE(1 == getProcCallCount(kTestCounterProc));

  evaluate("(test-counter)");
  REQUIRE(2 == getProcCallCount(kTestCounterProc));
}

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Can call procedure with single argument and receive return value",
    "[Evaluator]") {
  REQUIRE(0 == getProcCallCount(kTestUpProc));

  REQUIRE(Value{'J'} == evaluate("(test-up #\\j)"));
  REQUIRE(1 == getProcCallCount(kTestUpProc));

  REQUIRE(Value{'K'} == evaluate("(test-up #\\K)"));
  REQUIRE(2 == getProcCallCount(kTestUpProc));
}

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Can call procedure with two arguments and receive return value",
    "[Evaluator]") {
  REQUIRE(0 == getProcCallCount(kTestAdd2Proc));

  REQUIRE(Value{6} == evaluate("(test-add2 5 1)"));
  REQUIRE(1 == getProcCallCount(kTestAdd2Proc));

  REQUIRE(Value{22} == evaluate("(test-add2 25 -3)"));
  REQUIRE(2 == getProcCallCount(kTestAdd2Proc));
}

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Throws exception if procedure does not exist",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(does-not-exit)"); };
  REQUIRE_THROWS_AS(fn(), UnboundVariableException);
}

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Throws exception if operator not a procedure",
    "[Evaluator]") {
  vmState_->environment().defineVariable(
      vmState_->makeSymbol("foo"), Value{42});
  auto fn = [this]() { evaluate("(foo)"); };
  REQUIRE_THROWS_AS(fn(), Exception);
}

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Throws exception if argument type is not expected",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(test-up 2)"); };
  REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
}

TEST_CASE_METHOD(
    EvaluatorTestFixture,
    "Throws exception if too few arguments provided",
    "[Evaluator]") {
  auto fn = [this]() { evaluate("(test-add2 2)"); };
  REQUIRE_THROWS_AS(fn(), ArgumentListEmptyException);
}

// TODO: Too few arguments => error.
// TODO: Demonstrate that argument cannot be modified.
// TODO: Demonstrate that argument pair car/cdr could be changed.

TEST_CASE_METHOD(EvaluatorTestFixture, "Add procedure", "[Evaluator]") {
  SECTION("sums all passed fixnums") {
    REQUIRE(Value{0} == evaluate("(+)"));
    REQUIRE(Value{4} == evaluate("(+ 1 3)"));
    REQUIRE(Value{12} == evaluate("(+ 10 5 1 -4)"));
  }

  SECTION("error if any argument is not a fixnum") {
    auto fn = [this]() { evaluate("(+ 10 1 #t)"); };
    REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
  }
}