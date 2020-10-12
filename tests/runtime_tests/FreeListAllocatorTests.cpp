#include "runtime/allocators/FreeListAllocator.h"
#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Can allocate blocks of memory", "[FreeListAllocator]") {
  FreeListAllocator alloc;
  auto a1 = alloc.allocate(2);

  REQUIRE(nullptr != a1);
  REQUIRE(1 == alloc.allocationCount());
  REQUIRE(2 == alloc.requestedByteCount());

  auto a2 = alloc.allocate(5);

  REQUIRE(nullptr != a2);
  REQUIRE(a1 != a2);
  REQUIRE(2 == alloc.allocationCount());
  REQUIRE(7 == alloc.requestedByteCount());
}

TEST_CASE("Can allocate a zero number of bytes", "[FreeListAllocator]") {
  FreeListAllocator alloc;
  auto a1 = alloc.allocate(0);

  REQUIRE(nullptr != a1);
  REQUIRE(1 == alloc.allocationCount());
  REQUIRE(0 == alloc.requestedByteCount());
}

TEST_CASE("Freeing a pointer marks block as free", "[FreeListAllocator]") {
  FreeListAllocator alloc;
  const FreeListAllocator& ca = alloc;

  auto a1 = alloc.allocate(2);
  auto a2 = alloc.allocate(5);

  REQUIRE(ca.getHeader(a1)->isUsed);
  REQUIRE(ca.getHeader(a2)->isUsed);

  alloc.destroy(a1);

  REQUIRE_FALSE(ca.getHeader(a1)->isUsed);
  REQUIRE(ca.getHeader(a2)->isUsed);

  alloc.destroy(a2);

  REQUIRE_FALSE(ca.getHeader(a1)->isUsed);
  REQUIRE_FALSE(ca.getHeader(a2)->isUsed);
}

TEST_CASE("Can destroy a null pointer", "[FreeListAllocator]") {
  FreeListAllocator alloc;
  alloc.destroy(nullptr);
}

TEST_CASE("Free more than once throws exception", "[FreeListAllocator]") {
  FreeListAllocator alloc;

  auto a1 = alloc.allocate(2);
  auto destroyFunc = [&alloc, &a1]() { alloc.destroy(a1); };

  REQUIRE_NOTHROW(destroyFunc());
  REQUIRE_THROWS_AS(destroyFunc(), DoubleFreeException);
}

TEST_CASE("Blocks are reused when free", "[FreeListAllocator]") {
  FreeListAllocator alloc;

  auto a1 = alloc.allocate(8);
  alloc.destroy(a1);

  auto a2 = alloc.allocate(8);
  REQUIRE(a1 == a2);
}

#if _WIN64 || __x86_64__ || __ppc64__
TEST_CASE("Platform specific alignment (x64)", "[FreeListAllocator]") {
  FreeListAllocator alloc;
  const FreeListAllocator& ca = alloc;

  REQUIRE(8 == ca.getHeader(alloc.allocate(0))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(1))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(2))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(3))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(4))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(5))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(8))->sizeInBytes);
  REQUIRE(16 == ca.getHeader(alloc.allocate(12))->sizeInBytes);
  REQUIRE(16 == ca.getHeader(alloc.allocate(13))->sizeInBytes);
  REQUIRE(16 == ca.getHeader(alloc.allocate(16))->sizeInBytes);
}
#else
TEST_CASE("Platform specific alignment (x86)", "[FreeListAllocator]") {
  FreeListAllocator alloc;
  const FreeListAllocator& ca = alloc;

  REQUIRE(4 == ca.getHeader(alloc.allocate(0))->sizeInBytes);
  REQUIRE(4 == ca.getHeader(alloc.allocate(1))->sizeInBytes);
  REQUIRE(4 == ca.getHeader(alloc.allocate(2))->sizeInBytes);
  REQUIRE(4 == ca.getHeader(alloc.allocate(3))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(4))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(5))->sizeInBytes);
  REQUIRE(8 == ca.getHeader(alloc.allocate(8))->sizeInBytes);
  REQUIRE(12 == ca.getHeader(alloc.allocate(12))->sizeInBytes);
  REQUIRE(16 == ca.getHeader(alloc.allocate(13))->sizeInBytes);
  REQUIRE(16 == ca.getHeader(alloc.allocate(16))->sizeInBytes);
}
#endif