#include "runtime/Evaluator.h"

#include "runtime/Reader.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

namespace {
  Value read(std::string_view input, std::shared_ptr<VmState> vmState) {
    Reader r{vmState};
    return r.read(input);
  }

  Value evaluate(std::string_view input, std::shared_ptr<VmState> vmState) {
    Evaluator eval;
    return eval.evaluate(read(input, vmState));
  }
} // namespace

TEST_CASE("Self evaluating values", "[]") {
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
