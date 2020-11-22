#include "runtime/RuntimeApi.h"

#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE(
    "Constructing arg list throws exception if not pair",
    "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("with empty list is OK") {
    ArgList args{Value::EmptyList};
    REQUIRE(args.next.isEmptyList());
  }

  SECTION("with pair is OK") {
    ArgList args{vmState->makePair(Value{4}, Value{})};
    REQUIRE(Value{4} == args.next.toRawPair()->car);
  }

  SECTION("with not pair or empty list") {
    auto fn1 = []() { ArgList{Value{42}}; };
    REQUIRE_THROWS_AS(fn1(), WrongValueTypeException);

    auto fn2 = []() { ArgList{Value{true}}; };
    REQUIRE_THROWS_AS(fn2(), WrongValueTypeException);
  }
}

TEST_CASE("Popping empty argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  ArgList args{Value::EmptyList};

  SECTION("returns empty list error") {
    auto status = popArgument(args, nullptr);
    REQUIRE(PopArgResult::EmptyArgList == status);
  }

  SECTION("with type hint returns empty list error") {
    auto status = popArgument(args, nullptr, ValueType::EmptyList);
    REQUIRE(PopArgResult::EmptyArgList == status);

    status = popArgument(args, nullptr, ValueType::Fixnum);
    REQUIRE(PopArgResult::EmptyArgList == status);
  }

  SECTION("tryPop returns false with no exception thrown") {
    auto status = tryPopArgument(args, nullptr);
    REQUIRE_FALSE(status);
  }

  SECTION("with orThrow throws an exception") {
    auto fn = [&args]() { popArgumentOrThrow(args); };
    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
  }
}

TEST_CASE("Popping arguments from single argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  ArgList args{vmState->makePair(Value{4}, Value{})};

  SECTION("sets the argument value in out parameter") {
    Value value;
    auto status = popArgument(args, &value);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{4} == value);
  }

  SECTION("sets the argument value in out parameter when using tryPop") {
    Value value;
    REQUIRE(tryPopArgument(args, &value));
    REQUIRE(Value{4} == value);
  }

  SECTION("returns true when using tryPop") {
    Value value;
    REQUIRE(tryPopArgument(args, &value));
    REQUIRE(Value{4} == value);
  }

  SECTION("argument value out is optional") {
    auto status = popArgument(args, nullptr);
    REQUIRE(PopArgResult::Ok == status);
  }

  SECTION("returns the argument value when using orThrow") {
    Value status = popArgumentOrThrow(args);
    REQUIRE(Value{4} == status);
  }
}

TEST_CASE("Get all arguments from argument list", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  ArgList args{vmState->makePair(
      Value{'h'},
      vmState->makePair(Value{'i'}, vmState->makePair(Value{-2}, Value{})))};

  Value value;

  SECTION("using popArgument") {
    // First argument 'h'.
    auto status = popArgument(args, &value);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{'h'} == value);

    // Second argument 'i'.
    status = popArgument(args, &value);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{'i'} == value);

    // Third (last) argument -2.
    status = popArgument(args, &value);

    REQUIRE(PopArgResult::Ok == status);
    REQUIRE(Value{-2} == value);

    // All future accesses should fail with EmptyArgList. Try twice to make sure
    // no funny business is going on when the end is reached.
    status = popArgument(args, &value);
    REQUIRE(PopArgResult::EmptyArgList == status);

    status = popArgument(args, &value);
    REQUIRE(PopArgResult::EmptyArgList == status);
  }

  SECTION("using tryPopArgument") {
    // First argument 'h'.
    REQUIRE(tryPopArgument(args, &value));
    REQUIRE(Value{'h'} == value);

    // Second argument 'i'.
    REQUIRE(tryPopArgument(args, &value));
    REQUIRE(Value{'i'} == value);

    // Third (last) argument -2.
    REQUIRE(tryPopArgument(args, &value));
    REQUIRE(Value{-2} == value);

    // All future accesses should fail with EmptyArgList. Try twice to make sure
    // no funny business is going on when the end is reached.
    REQUIRE_FALSE(tryPopArgument(args, &value));
    REQUIRE_FALSE(tryPopArgument(args, &value));
  }

  SECTION("with orThrow") {
    // First argument 'h'.
    value = popArgumentOrThrow(args);
    REQUIRE(Value{'h'} == value);

    // Second argument 'i'.
    value = popArgumentOrThrow(args);
    REQUIRE(Value{'i'} == value);

    // Third (last) argument -2.
    value = popArgumentOrThrow(args);
    REQUIRE(Value{-2} == value);

    // All future accesses should fail with EmptyArgList. Try twice to make sure
    // no funny business is going on when the end is reached.
    auto fn = [&args]() { popArgumentOrThrow(args); };

    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
  }
}

TEST_CASE("Tracks the number of arguments popped", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  ArgList args{vmState->makePair(
      Value{'h'},
      vmState->makePair(Value{'i'}, vmState->makePair(Value{-2}, Value{})))};

  SECTION("when using pop") {
    REQUIRE(0 == args.popCount);

    REQUIRE(PopArgResult::Ok == popArgument(args, nullptr));
    REQUIRE(1 == args.popCount);

    REQUIRE(PopArgResult::Ok == popArgument(args, nullptr));
    REQUIRE(2 == args.popCount);

    REQUIRE(PopArgResult::Ok == popArgument(args, nullptr));
    REQUIRE(3 == args.popCount);
  }

  SECTION("when using tryPop") {
    REQUIRE(0 == args.popCount);

    REQUIRE(tryPopArgument(args, nullptr));
    REQUIRE(1 == args.popCount);

    REQUIRE(tryPopArgument(args, nullptr));
    REQUIRE(2 == args.popCount);

    REQUIRE(tryPopArgument(args, nullptr));
    REQUIRE(3 == args.popCount);
  }

  SECTION("when using orThrow") {
    REQUIRE(0 == args.popCount);

    REQUIRE(Value{'h'} == popArgumentOrThrow(args));
    REQUIRE(1 == args.popCount);

    REQUIRE(Value{'i'} == popArgumentOrThrow(args));
    REQUIRE(2 == args.popCount);

    REQUIRE(Value{-2} == popArgumentOrThrow(args));
    REQUIRE(3 == args.popCount);
  }
}

TEST_CASE("Pops at end of argument list are not counted", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  ArgList args{vmState->makePair(Value{'!'}, Value::EmptyList)};

  SECTION("when using pop") {
    REQUIRE(0 == args.popCount);

    REQUIRE(PopArgResult::Ok == popArgument(args, nullptr));
    REQUIRE(1 == args.popCount);

    REQUIRE(PopArgResult::EmptyArgList == popArgument(args, nullptr));
    REQUIRE(1 == args.popCount);

    REQUIRE(PopArgResult::EmptyArgList == popArgument(args, nullptr));
    REQUIRE(1 == args.popCount);
  }

  SECTION("when using tryPop") {
    REQUIRE(0 == args.popCount);

    REQUIRE(tryPopArgument(args, nullptr));
    REQUIRE(1 == args.popCount);

    REQUIRE_FALSE(tryPopArgument(args, nullptr));
    REQUIRE(1 == args.popCount);

    REQUIRE_FALSE(tryPopArgument(args, nullptr));
    REQUIRE(1 == args.popCount);
  }

  SECTION("when using orThrow") {
    REQUIRE(0 == args.popCount);

    REQUIRE(Value{'!'} == popArgumentOrThrow(args));
    REQUIRE(1 == args.popCount);

    auto fn = [&args]() { popArgumentOrThrow(args); };

    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
    REQUIRE(1 == args.popCount);

    REQUIRE_THROWS_AS(fn(), ArgumentMissingException);
    REQUIRE(1 == args.popCount);
  }
}

TEST_CASE("Popped arguments can be typed checked", "[RuntimeApi]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  ArgList args{
      vmState->makePair(Value{22}, vmState->makePair(Value{'i'}, Value{}))};

  SECTION("succeeds if type matches argument value") {
    Value value;

    SECTION("with pop") {
      // First argument 22.
      auto status = popArgument(args, &value, ValueType::Fixnum);

      REQUIRE(PopArgResult::Ok == status);
      REQUIRE(Value{22} == value);

      // Second argument 'i'.
      status = popArgument(args, &value, ValueType::Character);

      REQUIRE(PopArgResult::Ok == status);
      REQUIRE(Value{'i'} == value);
    }

    SECTION("with tryPop") {
      // First argument 22.
      REQUIRE(tryPopArgument(args, &value, ValueType::Fixnum));
      REQUIRE(Value{22} == value);

      // Second argument 'i'.
      REQUIRE(tryPopArgument(args, &value, ValueType::Character));
      REQUIRE(Value{'i'} == value);
    }

    SECTION("with orThrow") {
      // First argument 22.
      value = popArgumentOrThrow(args, ValueType::Fixnum);
      REQUIRE(Value{22} == value);

      // Second argument 'i'.
      value = popArgumentOrThrow(args, ValueType::Character);
      REQUIRE(Value{'i'} == value);
    }
  }

  SECTION("returns error code if type does not match argument value") {
    auto status = popArgument(args, nullptr, ValueType::Character);
    REQUIRE(PopArgResult::WrongArgValueType == status);
  }

  SECTION("throws exception if type does not match argument value") {
    SECTION("with tryPop") {
      auto fn = [&args]() {
        Value v;
        tryPopArgument(args, &v, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
    }

    SECTION("with orThorw") {
      auto fn = [&args]() { popArgumentOrThrow(args, ValueType::Character); };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
    }
  }

  SECTION("sets arg value out param even if type check fails") {
    Value value;

    SECTION("with pop") {
      auto status = popArgument(args, &value, ValueType::Character);
      REQUIRE(PopArgResult::WrongArgValueType == status);
      REQUIRE(Value{22} == value);
    }

    SECTION("with tryPop") {
      auto fn = [&args, &value]() {
        tryPopArgument(args, &value, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn(), WrongArgTypeException);
      REQUIRE(Value{22} == value);
    }
  }

  SECTION("pops argument even if type check fails") {
    Value value;

    SECTION("with pop") {
      auto status = popArgument(args, &value, ValueType::Character);
      REQUIRE(PopArgResult::WrongArgValueType == status);
      REQUIRE(Value{22} == value);

      status = popArgument(args, &value, ValueType::Fixnum);
      REQUIRE(PopArgResult::WrongArgValueType == status);
      REQUIRE(Value{'i'} == value);
    }

    SECTION("with tryPop") {
      Value value;

      auto fn1 = [&args, &value]() {
        tryPopArgument(args, &value, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
      REQUIRE(Value{22} == value);

      auto fn2 = [&args, &value]() {
        tryPopArgument(args, &value, ValueType::Fixnum);
      };

      REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
      REQUIRE(Value{'i'} == value);
    }

    SECTION("with orThrow") {
      auto fn1 = [&args, &value]() {
        popArgumentOrThrow(args, ValueType::Character);
      };

      REQUIRE_THROWS_AS(fn1(), WrongArgTypeException);
      REQUIRE(args.popCount == 1);

      auto fn2 = [&args, &value]() {
        popArgumentOrThrow(args, ValueType::Fixnum);
      };

      REQUIRE_THROWS_AS(fn2(), WrongArgTypeException);
      REQUIRE(args.popCount == 2);
    }
  }

  SECTION("empty list is not type checked") {
    ArgList a;

    SECTION("with pop") {
      auto status = popArgument(a, nullptr, ValueType::Fixnum);
      REQUIRE(PopArgResult::EmptyArgList == status);

      status = popArgument(a, nullptr, ValueType::EmptyList);
      REQUIRE(PopArgResult::EmptyArgList == status);
    }

    SECTION("with tryPop") {
      Value v;
      REQUIRE_FALSE(tryPopArgument(a, &v, ValueType::Fixnum));
      REQUIRE_FALSE(tryPopArgument(a, &v, ValueType::EmptyList));
    }
  }
}
