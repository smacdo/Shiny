#include "runtime/Reader.h"

#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

namespace {
  Value read(std::string_view input, std::shared_ptr<VmState> vmState) {
    Reader r{vmState};
    return r.read(input);
  }

  Value read(std::string_view input) {
    auto vmState =
        std::make_shared<VmState>(std::make_unique<MallocAllocator>());
    return read(input, vmState);
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

TEST_CASE("Can read chars", "[Reader]") {
  REQUIRE(Value{'c'} == read("#\\c"));
  REQUIRE(Value{'E'} == read("#\\E"));
  REQUIRE(Value{'3'} == read("#\\3"));
  REQUIRE(Value{'!'} == read("#\\!"));
}

TEST_CASE("Can read special characters", "[Reader]") {
  REQUIRE(Value{(char)0x07} == read("#\\alarm"));
  REQUIRE(Value{(char)0x08} == read("#\\backspace"));
  REQUIRE(Value{(char)0x7F} == read("#\\delete"));
  REQUIRE(Value{(char)0x1B} == read("#\\escape"));
  REQUIRE(Value{(char)0x0A} == read("#\\newline"));
  REQUIRE(Value{(char)0x00} == read("#\\null"));
  REQUIRE(Value{(char)0x0D} == read("#\\return"));
  REQUIRE(Value{(char)0x20} == read("#\\space"));
  REQUIRE(Value{(char)0x09} == read("#\\tab"));
}

TEST_CASE("Rejects invalid chars", "[Reader]") {
  REQUIRE_THROWS_AS([]() { read("#\\ca"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#\\alarms"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#\\"); }(), ReaderException);
  REQUIRE_THROWS_AS([]() { read("#\\ "); }(), ReaderException);
}

TEST_CASE("Can read strings", "[Reader]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("one line string") {
    REQUIRE("\"hello world\"" == read("\"hello world\"", vmState).toString());
  }

  SECTION("one line string with padding") {
    REQUIRE(
        "\"hello world\"" == read("  \"hello world\" ", vmState).toString());
  }

  SECTION("forward slash is escaped") {
    REQUIRE("\"1 \\\\ 2\"" == read("\"1 \\\\ 2\"", vmState).toString());
  }

  SECTION("escaped quotes") {
    REQUIRE("\"hello world\"" == read("\"hello world\"", vmState).toString());
    REQUIRE(
        "\"The word \\\"recursion\\\" has many meanings.\"" ==
        read("\"The word \\\"recursion\\\" has many meanings.\"", vmState)
            .toString());
  }

  SECTION("newline escaped") {
    REQUIRE(
        "\"hello\\nworld\"" == read("\"hello\\nworld\"", vmState).toString());
  }
}

TEST_CASE("Can read pairs", "[Reader]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());

  SECTION("empty pair") {
    REQUIRE(vmState->constants().emptyList == read("()", vmState));
    REQUIRE(vmState->constants().emptyList == read("(  )", vmState));
    REQUIRE(vmState->constants().emptyList == read("(\n)", vmState));
  }

  SECTION("dotted pair") {
    auto first = read("(1 . 2)", vmState);

    REQUIRE(first.isPair());
    REQUIRE(Value{1} == first.toRawPair()->car);
    REQUIRE(Value{2} == first.toRawPair()->cdr);

    auto second = read("(#\\c . #f)", vmState);

    REQUIRE(second.isPair());
    REQUIRE(Value{'c'} == second.toRawPair()->car);
    REQUIRE(Value{false} == second.toRawPair()->cdr);
  }

  SECTION("one list") {
    auto first = read("(10)", vmState);

    REQUIRE(first.isPair());
    REQUIRE(Value{10} == first.toRawPair()->car);
    REQUIRE(first.toRawPair()->cdr.isEmptyList());

    auto second = read("(#f)", vmState);

    REQUIRE(second.isPair());
    REQUIRE(Value{false} == second.toRawPair()->car);
    REQUIRE(second.toRawPair()->cdr.isEmptyList());
  }

  SECTION("two list") {
    auto first = read("(22 42)", vmState);

    REQUIRE(first.isPair());
    auto p = first.toRawPair();

    REQUIRE(Value{22} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{42} == p->car);
    REQUIRE(p->cdr.isEmptyList());

    auto second = read("(22 . (42 . ()))", vmState);

    REQUIRE(second.isPair());
    p = second.toRawPair();

    REQUIRE(Value{22} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{42} == p->car);
    REQUIRE(p->cdr.isEmptyList());
  }

  SECTION("three list") {
    auto first = read("(12 8 13)", vmState);

    REQUIRE(first.isPair());
    auto p = first.toRawPair();

    REQUIRE(Value{12} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{8} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{13} == p->car);
    REQUIRE(p->cdr.isEmptyList());

    auto second = read("(12 . (8 . (13 . ())))", vmState);

    REQUIRE(second.isPair());
    p = second.toRawPair();

    REQUIRE(Value{12} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{8} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{13} == p->car);
    REQUIRE(p->cdr.isEmptyList());
  }

  SECTION("three improper") {
    auto first = read("(12 . (8 . 13))", vmState);

    REQUIRE(first.isPair());
    auto p = first.toRawPair();

    REQUIRE(Value{12} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(Value{8} == p->car);
    REQUIRE(Value{13} == p->cdr);

    auto second = read("((22 . -22) . 13)", vmState);

    REQUIRE(second.isPair());
    p = second.toRawPair();

    REQUIRE(Value{22} == p->car.toRawPair()->car);
    REQUIRE(Value{-22} == p->car.toRawPair()->cdr);
    REQUIRE(Value{13} == p->cdr);
  }

  SECTION("pair string after int") {
    auto first = read("(5 \"bob\")", vmState);

    REQUIRE(first.isPair());
    auto p = first.toRawPair();

    REQUIRE(Value{5} == p->car);
    REQUIRE(p->cdr.isPair());
    p = p->cdr.toRawPair();

    REQUIRE(std::string("bob") == p->car.toStringView());
    REQUIRE(p->cdr.isEmptyList());
  }
}

TEST_CASE("Read symbols", "[Reader]") {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  auto foo = vmState->makeSymbol("foo");
  auto bar = vmState->makeSymbol("bar");

  REQUIRE(foo == read("foo", vmState));
  REQUIRE(bar == read("bar", vmState));
  REQUIRE(bar != read("foo", vmState));
  REQUIRE(foo == read("foo", vmState));

  auto minus = vmState->makeSymbol("-");
  REQUIRE(minus == read("-", vmState));

  auto weird = vmState->makeSymbol("++1");
  REQUIRE(weird == read("++1", vmState));
}
