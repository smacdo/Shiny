#include "runtime/CharacterStream.h"
#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Can get one character at a time", "[CharacterStream]") {
  CharacterStream cs("ab321");

  REQUIRE('a' == cs.nextChar());
  REQUIRE('b' == cs.nextChar());
  REQUIRE('3' == cs.nextChar());
  REQUIRE('2' == cs.nextChar());
  REQUIRE('1' == cs.nextChar());
}

TEST_CASE("Convert CRLF to LF when reading chars", "[CharacterStream]") {
  CharacterStream cs("a\r\nb\rc\r\r\n1\r\n");

  REQUIRE('a' == cs.nextChar());
  REQUIRE('\n' == cs.nextChar());
  REQUIRE('b' == cs.nextChar());
  REQUIRE('\r' == cs.nextChar());
  REQUIRE('c' == cs.nextChar());
  REQUIRE('\r' == cs.nextChar());
  REQUIRE('\n' == cs.nextChar());
  REQUIRE('1' == cs.nextChar());
  REQUIRE('\n' == cs.nextChar());
}

TEST_CASE("Track line and column when reading chars", "[CharacterStream]") {
  CharacterStream cs("ab\r\nc\r\r\n");

  REQUIRE(1 == cs.col());
  REQUIRE(1 == cs.lineNumber());
  REQUIRE('a' == cs.nextChar());

  REQUIRE(2 == cs.col());
  REQUIRE(1 == cs.lineNumber());
  REQUIRE('b' == cs.nextChar());

  REQUIRE(3 == cs.col());
  REQUIRE(1 == cs.lineNumber());
  REQUIRE('\n' == cs.nextChar());

  REQUIRE(1 == cs.col());
  REQUIRE(2 == cs.lineNumber());
  REQUIRE('c' == cs.nextChar());

  REQUIRE(2 == cs.col());
  REQUIRE(2 == cs.lineNumber());
  REQUIRE('\r' == cs.nextChar());

  REQUIRE(3 == cs.col());
  REQUIRE(2 == cs.lineNumber());
  REQUIRE('\n' == cs.nextChar());

  REQUIRE(-1 == cs.col());
  REQUIRE(-1 == cs.lineNumber());
}

TEST_CASE("Has next char until reached end of stream", "[CharacterStream]") {
  CharacterStream cs("ab");

  REQUIRE(cs.hasNext());
  REQUIRE('a' == cs.nextChar());

  REQUIRE(cs.hasNext());
  REQUIRE('b' == cs.nextChar());

  REQUIRE_FALSE(cs.hasNext());
}

TEST_CASE(
    "Reading beyond end of stream throws exception",
    "[CharacterStream]") {
  CharacterStream cs("x1");

  CHECK('x' == cs.nextChar());
  CHECK('1' == cs.nextChar());
  REQUIRE_THROWS_AS(
      [&cs]() { cs.nextChar(); }(), CharacterStreamEndOfStreamException);
}

TEST_CASE("Can try peek at characters", "[CharacterStream]") {
  CharacterStream cs("x18");

  SECTION("from index 0") {
    char c;

    REQUIRE(cs.tryPeekChar(0, &c));
    REQUIRE('x' == c);

    REQUIRE(cs.tryPeekChar(1, &c));
    REQUIRE('1' == c);

    REQUIRE(cs.tryPeekChar(2, &c));
    REQUIRE('8' == c);

    REQUIRE_FALSE(cs.tryPeekChar(3, &c));
  }

  SECTION("from index 1") {
    cs.nextChar(); // consume index 0, "x18" => "18"
    char c;

    REQUIRE(cs.tryPeekChar(0, &c));
    REQUIRE('1' == c);

    REQUIRE(cs.tryPeekChar(1, &c));
    REQUIRE('8' == c);

    REQUIRE_FALSE(cs.tryPeekChar(2, &c));
    REQUIRE_FALSE(cs.tryPeekChar(3, &c));
  }

  SECTION("from index 2") {
    cs.nextChar(); // consume index 0, "x18" => "18"
    cs.nextChar(); // consume index 1, "18" => "8"
    char c;

    REQUIRE(cs.tryPeekChar(0, &c));
    REQUIRE('8' == c);

    REQUIRE_FALSE(cs.tryPeekChar(1, &c));
    REQUIRE_FALSE(cs.tryPeekChar(2, &c));
  }

  SECTION("from index 3") {
    cs.nextChar(); // consume index 0, "x18" => "18"
    cs.nextChar(); // consume index 1, "18" => "8"
    cs.nextChar(); // consume index 2, "8" => ""
    char c;

    REQUIRE_FALSE(cs.tryPeekChar(0, &c));
    REQUIRE_FALSE(cs.tryPeekChar(1, &c));
  }

  SECTION("ok to pass nullptr") {
    REQUIRE(cs.tryPeekChar(0, nullptr));
    REQUIRE(cs.tryPeekChar(1, nullptr));
    REQUIRE(cs.tryPeekChar(2, nullptr));
    REQUIRE_FALSE(cs.tryPeekChar(3, nullptr));
  }
}

TEST_CASE("Can peek at characters", "[CharacterStream]") {
  CharacterStream cs("x18");

  SECTION("from index 0") {
    REQUIRE('x' == cs.peekChar(0));
    REQUIRE('1' == cs.peekChar(1));
    REQUIRE('8' == cs.peekChar(2));
    REQUIRE_THROWS_AS(
        [&cs]() { cs.peekChar(3); }(), CharacterStreamEndOfStreamException);
  }

  SECTION("from index 1") {
    cs.nextChar(); // consume index 0, "x18" => "18"

    REQUIRE('1' == cs.peekChar(0));
    REQUIRE('8' == cs.peekChar(1));
    REQUIRE_THROWS_AS(
        [&cs]() { cs.peekChar(2); }(), CharacterStreamEndOfStreamException);
  }

  SECTION("from index 2") {
    cs.nextChar(); // consume index 0, "x18" => "18"
    cs.nextChar(); // consume index 1, "18" => "8"

    REQUIRE('8' == cs.peekChar(0));
    REQUIRE_THROWS_AS(
        [&cs]() { cs.peekChar(1); }(), CharacterStreamEndOfStreamException);
  }

  SECTION("from index 3") {
    cs.nextChar(); // consume index 0, "x18" => "18"
    cs.nextChar(); // consume index 1, "18" => "8"
    cs.nextChar(); // consume index 2, "8" => ""

    REQUIRE_THROWS_AS(
        [&cs]() { cs.peekChar(0); }(), CharacterStreamEndOfStreamException);
    REQUIRE_THROWS_AS(
        [&cs]() { cs.peekChar(1); }(), CharacterStreamEndOfStreamException);
  }
}

TEST_CASE("Check if whitespace", "[CharacterStream]") {
  CharacterStream cs(" a \t\rx\n");

  REQUIRE(cs.peekIsWhitespace(0));       // 0: ' '
  REQUIRE_FALSE(cs.peekIsWhitespace(1)); // 1: 'a'
  REQUIRE(cs.peekIsWhitespace(2));       // 2: ' '
  REQUIRE(cs.peekIsWhitespace(3));       // 3: '\t'
  REQUIRE(cs.peekIsWhitespace(4));       // 4: '\r'
  REQUIRE_FALSE(cs.peekIsWhitespace(5)); // 5: 'x'
  REQUIRE(cs.peekIsWhitespace(6));       // 6: '\n'
  REQUIRE_FALSE(cs.peekIsWhitespace(7)); // 7: EOF
  REQUIRE_FALSE(cs.peekIsWhitespace(25));
}

TEST_CASE("Check if character matches", "[CharacterStream]") {
  CharacterStream cs("12");

  SECTION("with legitimate offset") {
    REQUIRE(cs.peekIsMatch(0, '1'));
    REQUIRE_FALSE(cs.peekIsMatch(0, '2'));

    REQUIRE(cs.peekIsMatch(1, '2'));
    REQUIRE_FALSE(cs.peekIsMatch(1, '1'));
  }

  SECTION("out of bounds is always false") {
    REQUIRE_FALSE(cs.peekIsMatch(3, '\0'));
  }
}

TEST_CASE("Check if alphabet character", "[CharacterStream]") {
  CharacterStream cs("+aXz! mZaAb");

  REQUIRE_FALSE(cs.peekIsAlpha(0)); // +
  REQUIRE(cs.peekIsAlpha(1));       // a
  REQUIRE(cs.peekIsAlpha(2));       // X
  REQUIRE(cs.peekIsAlpha(3));       // z
  REQUIRE_FALSE(cs.peekIsAlpha(4)); // !
  REQUIRE_FALSE(cs.peekIsAlpha(5)); //
  REQUIRE(cs.peekIsAlpha(6));       // m
  REQUIRE(cs.peekIsAlpha(7));       // Z
  REQUIRE(cs.peekIsAlpha(8));       // a
  REQUIRE(cs.peekIsAlpha(9));       // A
  REQUIRE(cs.peekIsAlpha(10));      // b
  REQUIRE_FALSE(cs.peekIsAlpha(11));
}

TEST_CASE("Check if digit", "[CharacterStream]") {
  CharacterStream cs("+012345678x9");

  REQUIRE_FALSE(cs.peekIsDigit(0));
  REQUIRE(cs.peekIsDigit(1));
  REQUIRE(cs.peekIsDigit(2));
  REQUIRE(cs.peekIsDigit(3));
  REQUIRE(cs.peekIsDigit(4));
  REQUIRE(cs.peekIsDigit(5));
  REQUIRE(cs.peekIsDigit(6));
  REQUIRE(cs.peekIsDigit(7));
  REQUIRE(cs.peekIsDigit(8));
  REQUIRE(cs.peekIsDigit(9));
  REQUIRE_FALSE(cs.peekIsDigit(10));
  REQUIRE(cs.peekIsDigit(11));
}

TEST_CASE("Can skip whitespace", "[CharacterStream]") {
  SECTION("one at a time") {
    CharacterStream cs("aA b c");

    REQUIRE(0 == cs.skipWhitespace());
    REQUIRE('a' == cs.nextChar());

    REQUIRE(0 == cs.skipWhitespace());
    REQUIRE('A' == cs.nextChar());

    REQUIRE(1 == cs.skipWhitespace());
    REQUIRE('b' == cs.nextChar());

    REQUIRE(1 == cs.skipWhitespace());
    REQUIRE('c' == cs.nextChar());

    REQUIRE(0 == cs.skipWhitespace());
  }

  SECTION("multiple at a time") {
    CharacterStream cs("  a b\t\r\nc");

    REQUIRE(2 == cs.skipWhitespace());
    REQUIRE('a' == cs.nextChar());

    REQUIRE(1 == cs.skipWhitespace());
    REQUIRE('b' == cs.nextChar());

    REQUIRE(3 == cs.skipWhitespace());
    REQUIRE('c' == cs.nextChar());
  }
}

TEST_CASE("Move to the end of the line", "[CharacterStream]") {
  SECTION("empty string") {
    CharacterStream cs("");
    REQUIRE(0 == cs.skipToNextLine());
    REQUIRE_FALSE(cs.hasNext());
  }

  SECTION("one line string") {
    CharacterStream cs("foobar");
    REQUIRE(6 == cs.skipToNextLine());
    REQUIRE_FALSE(cs.hasNext());
  }

  SECTION("multiple lines") {
    CharacterStream cs("1\n34\r\n5");

    REQUIRE(2 == cs.skipToNextLine());
    REQUIRE('3' == cs.peekChar(0));

    REQUIRE(4 == cs.skipToNextLine());
    REQUIRE('5' == cs.peekChar(0));

    REQUIRE(1 == cs.skipToNextLine());
    REQUIRE_FALSE(cs.hasNext());
  }
}