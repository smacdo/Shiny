#include "runtime/Value.h"
#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Null", "[Value]") {
  const Value n;

  SECTION("is always of type null") { REQUIRE(ValueType::Null == n.type()); }

  SECTION("can be converted to a string") {
    REQUIRE(std::string("()") == n.toString());
  }

  SECTION("support equality testing with other nulls") {
    Value n2{n};

    REQUIRE(n == n);
    REQUIRE(n2 == n);
  }

  SECTION("support inequality testing with other nulls") {
    Value n2{n};

    REQUIRE_FALSE(n != n);
    REQUIRE_FALSE(n2 != n);
  }

  SECTION("are not equal to any other type") {}
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

  SECTION("can be converted to a string") {
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

  SECTION("can be converted to a string") {
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

  SECTION("can be converted to a string") {
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