#include "runtime/Exception.h"
#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Throwing exception has callsite details", "[Exception]") {
  bool didCatchExpectedException = false;

  try {
    throw Exception(
        "something went horribly wrong", "foobar", "hello.cpp", 9123);
  } catch (const Exception& e) {
    CHECK(std::string("something went horribly wrong") == e.message());
    CHECK(std::string("foobar") == e.function());
    CHECK(std::string("hello.cpp") == e.fileName());
    CHECK(9123 == e.lineNumber());

    didCatchExpectedException = true;
  }

  REQUIRE(didCatchExpectedException);
}

TEST_CASE("Throwing an exception generates a rich message", "[Exception]") {
  SECTION("with function and filename") {
    bool didCatchExpectedException = false;

    try {
      throw Exception("beep boop", "doSomething", "blah.cpp", 2242);
    } catch (const Exception& e) {
      CHECK(std::string("blah.cpp:2242 (doSomething): beep boop") == e.what());

      didCatchExpectedException = true;
    }

    REQUIRE(didCatchExpectedException);
  }

  SECTION("with file name only") {
    bool didCatchExpectedException = false;

    try {
      throw Exception("beep boop", nullptr, "blah.cpp", 2242);
    } catch (const Exception& e) {
      CHECK(std::string("blah.cpp:2242: beep boop") == e.what());

      didCatchExpectedException = true;
    }

    REQUIRE(didCatchExpectedException);
  }

  SECTION("with function only") {
    bool didCatchExpectedException = false;

    try {
      throw Exception("beep boop", "doSomething", nullptr, 2242);
    } catch (const Exception& e) {
      CHECK(std::string("beep boop") == e.what());

      didCatchExpectedException = true;
    }

    REQUIRE(didCatchExpectedException);
  }

  SECTION("with neither function nor filename") {
    bool didCatchExpectedException = false;

    try {
      throw Exception("beep boop", nullptr, nullptr, 2242);
    } catch (const Exception& e) {
      CHECK(std::string("beep boop") == e.what());

      didCatchExpectedException = true;
    }

    REQUIRE(didCatchExpectedException);
  }

  SECTION("with an empty message") {
    bool didCatchExpectedException = false;

    try {
      throw Exception("", "doSomething", "blah.cpp", 2242);
    } catch (const Exception& e) {
      CHECK(
          std::string(
              "blah.cpp:2242 (doSomething): No exception message provided") ==
          e.what());

      didCatchExpectedException = true;
    }

    REQUIRE(didCatchExpectedException);
  }

  SECTION("with an empty message, null function and null filename") {
    bool didCatchExpectedException = false;

    try {
      throw Exception("", nullptr, nullptr, 1);
    } catch (const Exception& e) {
      CHECK(std::string("No exception message provided") == e.what());

      didCatchExpectedException = true;
    }

    REQUIRE(didCatchExpectedException);
  }
}