#include "runtime/RawString.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>
#include <cstring>

using namespace Shiny;

TEST_CASE("Allocate and destroy raw string", "[RawString]") {
  MallocAllocator alloc;
  destroy_string(&alloc, create_string(&alloc, "hello world!"));
}

TEST_CASE("Get raw string length", "[RawString]") {
  MallocAllocator alloc;

  auto s1 = create_string(&alloc, "12345");
  REQUIRE(5 == string_length(s1));

  auto s2 = create_string(&alloc, "123");
  REQUIRE(3 == string_length(s2));
}

TEST_CASE("Get raw string contentrs", "[RawString]") {
  MallocAllocator alloc;

  auto s1 = create_string(&alloc, "hello");
  REQUIRE(0 == strncmp(to_data(s1), "hello", 5));

  auto s2 = create_string(&alloc, "bark");
  REQUIRE(0 == strncmp(to_data(s2), "bark", 4));
}

TEST_CASE("Can allocate and safely destroy empty string", "[RawString]") {
  MallocAllocator alloc;

  auto s1 = create_string(&alloc, "");
  REQUIRE(0 == string_length(s1));

  auto s2 = create_string(&alloc, "");
  REQUIRE(0 == string_length(s2));

  destroy_string(&alloc, s2);
  destroy_string(&alloc, s1);
}

TEST_CASE("Can allocate and safely destroy single chars", "[RawString]") {
  MallocAllocator alloc;

  auto s1 = create_string(&alloc, "a");
  REQUIRE(1 == string_length(s1));

  auto s2 = create_string(&alloc, "!");
  REQUIRE(1 == string_length(s2));

  destroy_string(&alloc, s2);
  destroy_string(&alloc, s1);
}

TEST_CASE("Zero and one sized strings are singletons", "[RawString]") {
  MallocAllocator alloc;
  alloc.setFreeOnReset(false);

  create_string(&alloc, "");
  create_string(&alloc, "o");
  create_string(&alloc, " ");

  // do not need to delete, so there should be no ASAN errors...
}

TEST_CASE("Test if strings are equal", "[RawString]") {
  MallocAllocator alloc;
  alloc.setFreeOnReset(false);

  SECTION("same instance") {
    auto s1 = create_string(&alloc, "hello");
    REQUIRE(string_equal(s1, s1));

    destroy_string(&alloc, s1);
  }

  SECTION("same text") {
    auto s1 = create_string(&alloc, "foo");
    auto s2 = create_string(&alloc, "foo");

    REQUIRE(string_equal(s1, s2));
    REQUIRE(string_equal(s2, s1));

    destroy_string(&alloc, s1);
    destroy_string(&alloc, s2);
  }

  SECTION("different text same size") {
    auto s1 = create_string(&alloc, "foo");
    auto s2 = create_string(&alloc, "bar");

    REQUIRE_FALSE(string_equal(s1, s2));
    REQUIRE_FALSE(string_equal(s2, s1));

    destroy_string(&alloc, s1);
    destroy_string(&alloc, s2);
  }

  SECTION("different text different size") {
    auto s1 = create_string(&alloc, "foo");
    auto s2 = create_string(&alloc, "foobar");

    REQUIRE_FALSE(string_equal(s1, s2));
    REQUIRE_FALSE(string_equal(s2, s1));

    destroy_string(&alloc, s1);
    destroy_string(&alloc, s2);
  }
}
