#include "runtime/RuntimeApi.h"

#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Popping empty argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("returns empty list error") {
    auto status = popArgument({}, nullptr, nullptr);
    REQUIRE(PopArgResult::EmptyArgList == status);
  }

  SECTION("with type hint returns empty list error") {
    auto status = popArgument({}, nullptr, nullptr, ValueType::EmptyList);
    REQUIRE(PopArgResult::EmptyArgList == status);

    status = popArgument({}, nullptr, nullptr, ValueType::Fixnum);
    REQUIRE(PopArgResult::EmptyArgList == status);
  }

  SECTION("tryPop returns false with no exception thrown") {
    Value v;
    auto status = tryPopArgument({}, &v, nullptr);
    REQUIRE_FALSE(status);
  }

  SECTION("with orThrow throws an exception") {
    auto fn = []() { popArgumentOrThrow({}, nullptr); };
    REQUIRE_THROWS_AS(fn(), ArgumentListEmptyException);
  }
}

TEST_CASE("Popping arguments from single argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto argList = vmState->makePair(Value{4}, Value{});

  SECTION("sets the argument value in out parameter") {
    Value value;
    auto status = popArgument(argList, &value, nullptr);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{4} == value);
  }

  SECTION("sets the argument value in out parameter when using tryPop") {
    Value value;
    REQUIRE(tryPopArgument(argList, &value, nullptr));
    REQUIRE(Value{4} == value);
  }

  SECTION("returns true when using tryPop") {
    Value value;
    REQUIRE(tryPopArgument(argList, &value, nullptr));
    REQUIRE(Value{4} == value);
  }

  SECTION("argument value out is optional") {
    auto status = popArgument(argList, nullptr, nullptr);
    REQUIRE(PopArgResult::Ok == status);
  }

  SECTION("returns the argument value when using orThrow") {
    Value status = popArgumentOrThrow(argList, nullptr);
    REQUIRE(Value{4} == status);
  }
}

TEST_CASE("Get the argument tail from a single argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto argList = vmState->makePair(Value{4}, Value{});

  SECTION("sets the argument tail in out parameter") {
    Value argTail;
    auto status = popArgument(argList, nullptr, &argTail);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value::EmptyList == argTail);

    Value v;
    REQUIRE(tryPopArgument(argList, &v, &argTail));
    REQUIRE(Value::EmptyList == argTail);
  }

  SECTION("argument tail out is optional") {
    auto status = popArgument(argList, nullptr, nullptr);
    REQUIRE(PopArgResult::Ok == status);

    Value v;
    REQUIRE(tryPopArgument(argList, &v, nullptr));
  }

  SECTION("sets the argument tail in out parameter when using orThrow") {
    Value argTail;
    popArgumentOrThrow(argList, &argTail);
    REQUIRE(Value::EmptyList == argTail);
  }

  SECTION("argument tail out is optional when using orThrow") {
    popArgumentOrThrow(argList, nullptr);
  }
}

TEST_CASE("Get all arguments from argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto argList = vmState->makePair(
      Value{'h'},
      vmState->makePair(Value{'i'}, vmState->makePair(Value{-2}, Value{})));

  Value value;
  Value argTail;

  SECTION("using popArgument") {
    // First argument 'h'.
    auto status = popArgument(argList, &value, &argTail);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{'h'} == value);

    // Second argument 'i'.
    status = popArgument(argTail, &value, &argTail);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{'i'} == value);

    // Third (last) argument -2.
    status = popArgument(argTail, &value, &argTail);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{-2} == value);

    // All future accesses should fail with EmptyArgList. Try twice to make sure
    // no funny business is going on when the end is reached.
    status = popArgument(argTail, &value, &argTail);
    REQUIRE(PopArgResult::EmptyArgList == status);

    status = popArgument(argTail, &value, &argTail);
    REQUIRE(PopArgResult::EmptyArgList == status);
  }

  SECTION("using tryPopArgument") {
    // First argument 'h'.
    REQUIRE(tryPopArgument(argList, &value, &argTail));
    REQUIRE(Value{'h'} == value);

    // Second argument 'i'.
    REQUIRE(tryPopArgument(argTail, &value, &argTail));
    REQUIRE(Value{'i'} == value);

    // Third (last) argument -2.
    REQUIRE(tryPopArgument(argTail, &value, &argTail));
    REQUIRE(Value{-2} == value);

    // All future accesses should fail with EmptyArgList. Try twice to make sure
    // no funny business is going on when the end is reached.
    REQUIRE_FALSE(tryPopArgument(argTail, &value, &argTail));
    REQUIRE_FALSE(tryPopArgument(argTail, &value, &argTail));
  }

  SECTION("with orThrow") {
    // First argument 'h'.
    value = popArgumentOrThrow(argList, &argTail);
    REQUIRE(Value{'h'} == value);

    // Second argument 'i'.
    value = popArgumentOrThrow(argTail, &argTail);
    REQUIRE(Value{'i'} == value);

    // Third (last) argument -2.
    value = popArgumentOrThrow(argTail, &argTail);
    REQUIRE(Value{-2} == value);

    // All future accesses should fail with EmptyArgList. Try twice to make sure
    // no funny business is going on when the end is reached.
    auto fn = [&argTail]() { popArgumentOrThrow(argTail, &argTail); };

    REQUIRE_THROWS_AS(fn(), ArgumentListEmptyException);
    REQUIRE(argTail.isEmptyList());

    REQUIRE_THROWS_AS(fn(), ArgumentListEmptyException);
    REQUIRE(argTail.isEmptyList());
  }
}

TEST_CASE("Popped arguments can be typed checked", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto argList =
      vmState->makePair(Value{22}, vmState->makePair(Value{'i'}, Value{}));

  SECTION("succeeds if type matches argument value") {
    Value value;
    Value argTail;

    SECTION("with pop") {
      // First argument 'h'.
      auto status = popArgument(argList, &value, &argTail, ValueType::Fixnum);

      REQUIRE(PopArgResult::Ok == status);
      REQUIRE(Value{22} == value);

      // Second argument 'i'.
      status = popArgument(argTail, &value, &argTail, ValueType::Character);

      REQUIRE(PopArgResult::Ok == status);
      REQUIRE(Value{'i'} == value);
    }

    SECTION("with tryPop") {
      // First argument 'h'.
      REQUIRE(tryPopArgument(argList, &value, &argTail, ValueType::Fixnum));
      REQUIRE(Value{22} == value);

      // Second argument 'i'.
      REQUIRE(tryPopArgument(argTail, &value, &argTail, ValueType::Character));
      REQUIRE(Value{'i'} == value);
    }

    SECTION("with orThrow") {
      // First argument 'h'.
      value = popArgumentOrThrow(argList, &argTail, ValueType::Fixnum);
      REQUIRE(Value{22} == value);

      // Second argument 'i'.
      value = popArgumentOrThrow(argTail, &argTail, ValueType::Character);
      REQUIRE(Value{'i'} == value);
    }
  }

  SECTION("returns error code if type does not match argument value") {
    auto status = popArgument(argList, nullptr, nullptr, ValueType::Character);
    REQUIRE(PopArgResult::WrongArgValueType == status);
  }

  SECTION("throws exception if type does not match argument value") {
    SECTION("with tryPop") {
      auto fn = [&argList]() {
        Value v;
        tryPopArgument(argList, &v, nullptr, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
    }

    SECTION("with orThorw") {
      auto fn = [&argList]() {
        popArgumentOrThrow(argList, nullptr, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
    }
  }

  SECTION("sets arg value out param even if type check fails") {
    Value value;

    SECTION("with pop") {
      auto status = popArgument(argList, &value, nullptr, ValueType::Character);
      REQUIRE(PopArgResult::WrongArgValueType == status);
      REQUIRE(Value{22} == value);
    }

    SECTION("with tryPop") {
      auto fn = [&argList, &value]() {
        tryPopArgument(argList, &value, nullptr, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
      REQUIRE(Value{22} == value);
    }
  }

  SECTION("sets arg tail out param even if type check fails") {
    Value argTail;

    SECTION("with pop") {
      auto status =
          popArgument(argList, nullptr, &argTail, ValueType::Character);
      REQUIRE(PopArgResult::WrongArgValueType == status);
      REQUIRE(cdr(argList).toRawPair() == argTail.toRawPair());
    }

    SECTION("with tryPop") {
      Value value;

      auto fn = [&argList, &argTail, &value]() {
        tryPopArgument(argList, &value, &argTail, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
      REQUIRE(cdr(argList).toRawPair() == argTail.toRawPair());
    }

    SECTION("with orThrow") {
      auto fn = [&argList, &argTail]() {
        popArgumentOrThrow(argList, &argTail, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
      REQUIRE(cdr(argList).toRawPair() == argTail.toRawPair());
    }
  }

  SECTION("empty list is not type checked") {
    SECTION("with pop") {
      auto status =
          popArgument(Value::EmptyList, nullptr, nullptr, ValueType::Fixnum);
      REQUIRE(PopArgResult::EmptyArgList == status);

      status =
          popArgument(Value::EmptyList, nullptr, nullptr, ValueType::EmptyList);
      REQUIRE(PopArgResult::EmptyArgList == status);
    }

    SECTION("with tryPop") {
      Value v;
      REQUIRE_FALSE(
          tryPopArgument(Value::EmptyList, &v, nullptr, ValueType::Fixnum));
      REQUIRE_FALSE(
          tryPopArgument(Value::EmptyList, &v, nullptr, ValueType::EmptyList));
    }
  }
}
