#include "runtime/Reader.h"
#include <catch2/catch.hpp>

using namespace Shiny;

namespace {
  Value read(std::string_view input) {
    Reader r;
    return r.read(input);
  }
} // namespace

TEST_CASE("Can read fixnums", "[Reader]") {
  REQUIRE(Value{0} == read("0"));
  REQUIRE(Value{1} == read("1"));
  REQUIRE(Value{2} == read("2"));
  REQUIRE(Value{-1} == read("-1"));
  REQUIRE(Value{42} == read("42"));
  REQUIRE(Value{500} == read("500"));

  // supports at least int32 sized integers.
  REQUIRE(Value{2147483647} == read("2147483647"));
  REQUIRE(Value{-2147483647} == read("-2147483647"));

  // numbers can be start with zero.
  REQUIRE(Value{7} == read("007"));
  REQUIRE(Value{7} == read("0000000000000000000007"));
}

TEST_CASE("Rejects invalid fixnums", "[Reader]") {
  REQUIRE_THROWS_AS([]() { read("1e"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("2+31"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("2.4"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("+1"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("--1"); }(), ReaderException);
}

TEST_CASE("Can read boolean", "[Reader]") {
  REQUIRE(Value{true} == read("#t"));
  REQUIRE(Value{false} == read("#f"));
}

TEST_CASE("Rejects invalid booleans", "[Reader]") {
  REQUIRE_THROWS_AS([]() { read("#true"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#T"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#false"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#F"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#nope"); }(), ReaderException);
}