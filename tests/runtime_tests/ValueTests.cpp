#include "runtime/Value.h"

#include "runtime/RawString.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Empty List", "[Value]") {
  const Value empty; // Default constructor constructs empty list.

  SECTION("is always of type EmptyList") {
    REQUIRE(ValueType::EmptyList == empty.type());
  }

  SECTION("can be printed") { REQUIRE(std::string("()") == empty.toString()); }

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

  SECTION("can be printed") {
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

  SECTION("can be printed") {
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

TEST_CASE("Character values", "[Value]") {
  const Value a{'a'};
  Value b{'b'};
  Value x{'x'};

  SECTION("are always of type char") {
    REQUIRE(ValueType::Character == a.type());
    REQUIRE(ValueType::Character == b.type());
    REQUIRE(ValueType::Character == x.type());
  }

  SECTION("can be printed") {
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

  SECTION("can be printed") {
    REQUIRE(std::string("\"a\"") == a.toString());
    REQUIRE(std::string("\"foo\"") == foo.toString());
    REQUIRE(std::string("\"foobar\"") == foobar.toString());
  }

  SECTION("can be printed_view") {
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
