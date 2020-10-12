#include "runtime/allocators/MallocAllocator.h"
#include <catch2/catch.hpp>

using namespace Shiny;

class MallocTestableAllocator : public MallocAllocator {
public:
  MallocTestableAllocator() {
    setFreeOnReset(false); // Catches leaks.
  }
};

TEST_CASE("Can allocate blocks of memory 2", "[MallocAllocator]") {
  MallocTestableAllocator alloc;
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

TEST_CASE("Can destroy allocation", "[MallocAllocator]") {
  MallocTestableAllocator alloc;

  auto a1 = alloc.allocate(2);
  auto a2 = alloc.allocate(5);

  alloc.destroy(a1);
  alloc.destroy(a2);
}

#if _WIN64 || __x86_64__ || __ppc64__
TEST_CASE("Platform specific alignment (x64) 2", "[MallocAllocator]") {
  MallocTestableAllocator alloc;
  const MallocTestableAllocator& ca = alloc;

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
TEST_CASE("Platform specific alignment (x86) 2", "[MallocAllocator]") {
  MallocTestableAllocator alloc;
  const MallocTestableAllocator& ca = alloc;

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