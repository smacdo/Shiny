#include "runtime/Value.h"

#include "runtime/RawString.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Value static property EmptyList is in fact a EmptyList", "[Value]") {
  REQUIRE(ValueType::EmptyList == Value::EmptyList.type());
}

TEST_CASE("Empty List", "[Value]") {
  const Value empty; // Default constructor constructs empty list.

  SECTION("is always of type EmptyList") {
    REQUIRE(ValueType::EmptyList == empty.type());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("()") == empty.toString());
  }

  SECTION("is only equal to another empty list") {
    Value secondEmpty{};
    Value emptyCopy{empty};
    Value notEmpty{0};

    REQUIRE(empty == empty);
    REQUIRE_FALSE(empty != empty);

    REQUIRE(secondEmpty == empty);
    REQUIRE_FALSE(secondEmpty != empty);

    REQUIRE(emptyCopy == empty);
    REQUIRE_FALSE(emptyCopy != empty);

    REQUIRE_FALSE(notEmpty == empty);
    REQUIRE(notEmpty != empty);
  }
}

TEST_CASE("Fixnum value", "[Value]") {
  const Value zero{0};

  Value a{22};
  Value b{-5};

  SECTION("are always of type fixnum") {
    REQUIRE(ValueType::Fixnum == zero.type());
    REQUIRE(ValueType::Fixnum == a.type());
    REQUIRE(ValueType::Fixnum == b.type());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("0") == zero.toString());
    REQUIRE(std::string("22") == a.toString());
    REQUIRE(std::string("-5") == b.toString());
  }

  SECTION("can be converted to an integer") {
    REQUIRE(0 == zero.toFixnum());
    REQUIRE(22 == a.toFixnum());
    REQUIRE(-5 == b.toFixnum());
  }

  SECTION("support equality testing with other fixnums") {
    REQUIRE(zero == zero);
    REQUIRE_FALSE(zero == a);
    REQUIRE_FALSE(zero == b);
    REQUIRE_FALSE(a == b);

    Value copy{a};
    REQUIRE(copy == a);
  }

  SECTION("support inequality testing with other fixnums") {
    REQUIRE_FALSE(zero != zero);
    REQUIRE(zero != a);
    REQUIRE(zero != b);
    REQUIRE(a != b);

    Value copy{a};
    REQUIRE_FALSE(copy != a);
  }

  SECTION("are not equal to any other type") {
    REQUIRE_FALSE(zero == Value{});
    REQUIRE_FALSE(zero == Value{true});
  }
}

TEST_CASE("Boolean values", "[Value]") {
  const Value t{true};
  const Value f{false};

  SECTION("are always of type boolean") {
    REQUIRE(ValueType::Boolean == t.type());
    REQUIRE(ValueType::Boolean == f.type());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("#t") == t.toString());
    REQUIRE(std::string("#f") == f.toString());
  }

  SECTION("can be converted to an boolean") {
    REQUIRE(t.toBool());
    REQUIRE_FALSE(f.toBool());
  }

  SECTION("support equality testing with other bools") {
    REQUIRE(t == t);
    REQUIRE(f == f);
    REQUIRE_FALSE(t == f);
  }

  SECTION("support inequality testing with other bools") {
    REQUIRE_FALSE(t != t);
    REQUIRE_FALSE(f != f);
    REQUIRE(t != f);
  }

  SECTION("are not equal to any other type") {
    REQUIRE_FALSE(t == Value{});
    REQUIRE_FALSE(t == Value{22});
  }
}

TEST_CASE("Symbol values", "[Value]") {
  MallocAllocator alloc;

  SECTION("are always of type string") {
    const Value s0{create_string(&alloc, "sym_0"), ValueType::Symbol};
    REQUIRE(ValueType::Symbol == s0.type());
    REQUIRE(s0.isSymbol());

    const Value s1{create_string(&alloc, "sym_1"), ValueType::Symbol};
    REQUIRE(ValueType::Symbol == s1.type());
    REQUIRE(s1.isSymbol());
  }

  SECTION("convert to a string") {
    const Value s0{create_string(&alloc, "sym_0"), ValueType::Symbol};
    REQUIRE(std::string("sym_0") == s0.toString());

    const Value s1{create_string(&alloc, "foobar"), ValueType::Symbol};
    REQUIRE(std::string("foobar") == s1.toString());
  }

  SECTION("can convert to a string_view") {
    const Value s0{create_string(&alloc, "sym_0"), ValueType::Symbol};
    REQUIRE(std::string_view{"sym_0"} == s0.toStringView());

    const Value s1{create_string(&alloc, "foobar"), ValueType::Symbol};
    REQUIRE(std::string_view{"foobar"} == s1.toStringView());
  }

  SECTION("is only equal to itself") {
    const Value s0{create_string(&alloc, "sym_0"), ValueType::Symbol};
    Value s0_copy{create_string(&alloc, "sym_0"), ValueType::Symbol};
    const Value s1{create_string(&alloc, "foobar"), ValueType::Symbol};

    REQUIRE(s0 == s0);
    REQUIRE_FALSE(s0 == s0_copy);
    REQUIRE_FALSE(s0 == s1);

    REQUIRE_FALSE(s0 != s0);
    REQUIRE(s0 != s0_copy);
    REQUIRE(s0 != s1);
  }
}

TEST_CASE("Character values", "[Value]") {
  const Value a{'a'};
  Value b{'b'};
  Value x{'x'};

  SECTION("are always of type char") {
    REQUIRE(ValueType::Character == a.type());
    REQUIRE(ValueType::Character == b.type());
    REQUIRE(ValueType::Character == x.type());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("#\\a") == a.toString());
    REQUIRE(std::string("#\\b") == b.toString());
    REQUIRE(std::string("#\\x") == x.toString());
  }

  SECTION("special characters are printed to spec") {
    REQUIRE(std::string("#\\alarm") == Value{(char)0x07}.toString());
    REQUIRE(std::string("#\\backspace") == Value{(char)0x08}.toString());
    REQUIRE(std::string("#\\delete") == Value{(char)0x7F}.toString());
    REQUIRE(std::string("#\\escape") == Value{(char)0x1B}.toString());
    REQUIRE(std::string("#\\newline") == Value{(char)0x0A}.toString());
    REQUIRE(std::string("#\\null") == Value{(char)0x00}.toString());
    REQUIRE(std::string("#\\return") == Value{(char)0x0D}.toString());
    REQUIRE(std::string("#\\space") == Value{(char)0x20}.toString());
    REQUIRE(std::string("#\\tab") == Value{(char)0x09}.toString());
  }

  SECTION("can be converted to a char") {
    REQUIRE('a' == a.toChar());
    REQUIRE('b' == b.toChar());
    REQUIRE('x' == x.toChar());
  }

  SECTION("support equality testing with other chars") {
    REQUIRE(a == a);
    REQUIRE(b == b);
    REQUIRE_FALSE(a == b);
  }

  SECTION("support inequality testing with other chars") {
    REQUIRE_FALSE(a != a);
    REQUIRE_FALSE(x != x);
    REQUIRE(x != a);
  }

  SECTION("are not equal to any other type") {
    REQUIRE_FALSE(a == Value{});
    REQUIRE_FALSE(b == Value{22});
  }
}

TEST_CASE("String values", "[Value]") {
  MallocAllocator alloc;

  const Value a{create_string(&alloc, "a")};
  Value foo{create_string(&alloc, "foo")};
  Value foobar{create_string(&alloc, "foobar")};

  SECTION("are always of type string") {
    REQUIRE(ValueType::String == a.type());
    REQUIRE(ValueType::String == foo.type());
    REQUIRE(ValueType::String == foobar.type());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("\"a\"") == a.toString());
    REQUIRE(std::string("\"foo\"") == foo.toString());
    REQUIRE(std::string("\"foobar\"") == foobar.toString());
  }

  SECTION("convert to a string_view") {
    REQUIRE(std::string_view{"a"} == a.toStringView());
    REQUIRE(std::string_view{"foo"} == foo.toStringView());
    REQUIRE(std::string_view{"foobar"} == foobar.toStringView());
  }

  SECTION("are only equal to strings of the same instance") {
    REQUIRE(foo == foo);

    const Value foo2{create_string(&alloc, foo.toStringView())};
    REQUIRE_FALSE(foo == foo2);
    REQUIRE_FALSE(foo2 == foo);

    REQUIRE_FALSE(a == foo);
    REQUIRE_FALSE(a == foobar);
  }

  SECTION("support inequality testing with other strings") {
    REQUIRE_FALSE(foo != foo);

    const Value foo2{create_string(&alloc, foo.toStringView())};
    REQUIRE(foo != foo2);
    REQUIRE(foo2 != foo);

    REQUIRE(a != foo);
    REQUIRE(a != foobar);
  }

  SECTION("are not equal to any other type") {
    REQUIRE_FALSE(a == Value{});
    REQUIRE_FALSE(a == Value{22});
  }
}

TEST_CASE("Pairs", "[Value]") {
  VmState vm(std::make_unique<MallocAllocator>());
  const auto empty = Value{vm.makePair(Value{}, Value{})};
  const auto leaf = Value{vm.makePair(Value{42}, Value{'c'})};

  const auto tail = Value{vm.makePair(Value{100}, Value{})};
  auto mid = Value{vm.makePair(Value{2020}, tail)};
  auto head = Value{vm.makePair(Value{22}, mid)};

  SECTION("are always of type Pair") {
    REQUIRE(ValueType::Pair == empty.type());
    REQUIRE(empty.isPair());

    REQUIRE(ValueType::Pair == leaf.type());
    REQUIRE(leaf.isPair());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("(())") == empty.toString());
    REQUIRE(std::string("(42 . #\\c)") == leaf.toString());

    REQUIRE(std::string("(22 2020 100)") == head.toString());
    REQUIRE(std::string("(2020 100)") == mid.toString());
    REQUIRE(std::string("(100)") == tail.toString());
  }

  SECTION("are only equal to self") {
    const auto a = Value{vm.makePair(Value{1}, Value{2})};
    auto b = Value{vm.makePair(Value{1}, Value{2})};

    REQUIRE(a == a);
    REQUIRE(b == b);
    REQUIRE_FALSE(a == b);
    REQUIRE_FALSE(b == a);

    REQUIRE(head == head);
    REQUIRE(mid == mid);
    REQUIRE(tail == tail);
  }

  SECTION("supports not equal") {
    const auto a = Value{vm.makePair(Value{1}, Value{2})};
    auto b = Value{vm.makePair(Value{1}, Value{2})};

    REQUIRE_FALSE(a != a);
    REQUIRE_FALSE(b != b);
    REQUIRE(a != b);
    REQUIRE(b != a);

    REQUIRE_FALSE(head != head);
    REQUIRE_FALSE(mid != mid);
    REQUIRE_FALSE(tail != tail);
  }

  SECTION("can be created with cons") {
    auto pair = cons(&vm, Value{36}, Value{112});

    REQUIRE(pair.isPair());
    REQUIRE(Value{36} == pair.toRawPair()->car);
    REQUIRE(Value{112} == pair.toRawPair()->cdr);
  }

  SECTION("get car value") {
    auto pair = cons(&vm, Value{'d'}, Value{-5});
    REQUIRE(Value{'d'} == car(pair));
  }

  SECTION("get car throws exception if value is not pair") {
    auto notPair = Value{};
    REQUIRE_THROWS_AS(
        [&notPair]() { car(notPair); }(), WrongValueTypeException);
  }

  SECTION("set car value") {
    auto pair = cons(&vm, Value{'d'}, Value{-5});
    set_car(pair, Value{-1024});
    REQUIRE(Value{-1024} == car(pair));
  }

  SECTION("set car throws exception if value is not pair") {
    auto notPair = Value{};
    REQUIRE_THROWS_AS(
        [&notPair]() { set_car(notPair, Value{}); }(), WrongValueTypeException);
  }

  SECTION("get cdr value") {
    auto pair = cons(&vm, Value{'d'}, Value{-5});
    REQUIRE(Value{-5} == cdr(pair));
  }

  SECTION("get cdr throws exception if value is not pair") {
    auto notPair = Value{false};
    REQUIRE_THROWS_AS(
        [&notPair]() { cdr(notPair); }(), WrongValueTypeException);
  }

  SECTION("set cdr value") {
    auto pair = cons(&vm, Value{'d'}, Value{-5});
    set_cdr(pair, Value{'*'});
    REQUIRE(Value{'*'} == cdr(pair));
  }

  SECTION("set cdr throws exception if value is not pair") {
    auto notPair = Value{};
    REQUIRE_THROWS_AS(
        [&notPair]() { set_cdr(notPair, Value{}); }(), WrongValueTypeException);
  }
}

Value testProc1(ArgList&, VmState&, Environment&) { return Value{2}; }
Value testProc2(ArgList&, VmState&, Environment&) { return Value{2}; }

TEST_CASE("Primitive procedure values", "[Value]") {
  Value a{&testProc1};
  Value b{&testProc2};

  SECTION("are always of type primitive procedure") {
    REQUIRE(ValueType::PrimitiveProcedure == a.type());
    REQUIRE(ValueType::PrimitiveProcedure == b.type());
  }

  SECTION("convert to a string") {
    REQUIRE(std::string("#<procedure>") == a.toString());
    REQUIRE(std::string("#<procedure>") == b.toString());
  }

  SECTION("convert to a function pointer") {
    auto procA = a.toPrimitiveProcedure();
    auto procB = b.toPrimitiveProcedure();

    REQUIRE(&testProc1 == procA);
    REQUIRE(&testProc2 == procB);
  }

  SECTION("are only equal to values with same function pointer") {
    Value a2{&testProc1};

    REQUIRE(a == a);
    REQUIRE(a == a2);
    REQUIRE_FALSE(a == b);
  }

  SECTION("support inequality testing with other primtive procedures") {
    Value a2{&testProc1};

    REQUIRE_FALSE(a != a);
    REQUIRE_FALSE(a != a2);
    REQUIRE(a != b);
  }

  SECTION("are not equal to any other type") {
    REQUIRE_FALSE(a == Value{});
    REQUIRE_FALSE(a == Value{22});
    REQUIRE_FALSE(b == Value{'x'});
  }
}