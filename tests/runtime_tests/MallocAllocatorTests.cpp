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

  alloc.destroy(a2);
  alloc.destroy(a1);
}

TEST_CASE("Can destroy allocation", "[MallocAllocator]") {
  MallocTestableAllocator alloc;

  auto a1 = alloc.allocate(2);
  auto a2 = alloc.allocate(5);

  alloc.destroy(a1);
  alloc.destroy(a2);
}
