#include "runtime/Value.h"

#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Create new pair with cons", "[Value]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = cons(vmState.get(), Value{5}, Value{false});

  REQUIRE(ValueType::Pair == pair.type());
  REQUIRE(Value{5} == pair.toRawPair()->car);
  REQUIRE(Value{false} == pair.toRawPair()->cdr);
}

TEST_CASE("Get pair car value", "[Value]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = cons(vmState.get(), Value{'x'}, Value{-1025});
  REQUIRE(Value{'x'} == car(pair));
}

TEST_CASE("Get pair car throws exception if value not a pair", "[Value]") {
  auto fn1 = []() { car(Value{}); };
  auto fn2 = []() { car(Value{42}); };

  REQUIRE_THROWS_AS(fn1(), WrongValueTypeException);
  REQUIRE_THROWS_AS(fn2(), WrongValueTypeException);
}

TEST_CASE("Set pair car value", "[Value]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = cons(vmState.get(), Value{'x'}, Value{-1025});
  set_car(pair, Value{2});
  REQUIRE(Value{2} == car(pair));
}

TEST_CASE("Set pair car throws exception if value not a pair", "[Value]") {
  auto fn1 = []() { set_car(Value{}, Value{3}); };
  auto fn2 = []() { set_car(Value{42}, Value{3}); };

  REQUIRE_THROWS_AS(fn1(), WrongValueTypeException);
  REQUIRE_THROWS_AS(fn2(), WrongValueTypeException);
}

TEST_CASE("Get pair cdr value", "[Value]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = cons(vmState.get(), Value{'x'}, Value{-1025});
  REQUIRE(Value{-1025} == cdr(pair));
}

TEST_CASE("Get pair cdr throws exception if value not a pair", "[Value]") {
  auto fn1 = []() { cdr(Value{}); };
  auto fn2 = []() { cdr(Value{42}); };

  REQUIRE_THROWS_AS(fn1(), WrongValueTypeException);
  REQUIRE_THROWS_AS(fn2(), WrongValueTypeException);
}

TEST_CASE("Set pair cdr value", "[Value]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = cons(vmState.get(), Value{'x'}, Value{-1025});
  set_cdr(pair, Value{61});
  REQUIRE(Value{61} == cdr(pair));
}

TEST_CASE("Set pair cdr throws exception if value not a pair", "[Value]") {
  auto fn1 = []() { set_cdr(Value{}, Value{3}); };
  auto fn2 = []() { set_cdr(Value{42}, Value{3}); };

  REQUIRE_THROWS_AS(fn1(), WrongValueTypeException);
  REQUIRE_THROWS_AS(fn2(), WrongValueTypeException);
}

TEST_CASE("Get pair gets both car and cdr values", "[Value]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto pair = cons(vmState.get(), Value{'H'}, Value{1});

  Value a, b;
  get_pair(pair, &a, &b);

  REQUIRE(Value{'H'} == a);
  REQUIRE(Value{1} == b);
}

TEST_CASE("Get pair throws exception if value is not a pair", "[Value]") {
  Value a, b;

  auto fn1 = [&a, &b]() { get_pair(Value{}, &a, &b); };
  auto fn2 = [&a, &b]() { get_pair(Value{42}, &a, &b); };

  REQUIRE_THROWS_AS(fn1(), WrongValueTypeException);
  REQUIRE_THROWS_AS(fn2(), WrongValueTypeException);
}
