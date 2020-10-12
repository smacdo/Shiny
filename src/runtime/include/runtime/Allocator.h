#pragma once
#include "runtime/Exception.h"
#include "runtime/Value.h"
#include <string_view>

namespace Shiny {
  /** Allocates Shiny objects. */
  class Allocator {
  public:
    /** Destructor. */
    virtual ~Allocator();

    /** Allocate a C++ object with this allocator. */
    template<typename T, class U> T* construct(U&& args) {
      auto rawptr = allocate(sizeof(T));
      return new (rawptr) T(std::forward<U>(args));
    }

    /** Allocate a C++ object with this allocator. */
    template<typename T, class... U> T* construct(U&&... args) {
      auto rawptr = allocate(sizeof(T));
      return new (rawptr) T(std::forward<U>(args)...);
    }

    /** Allocate a block of memory of at least 'sizeInBytes'. */
    virtual void* allocate(size_t sizeInBytes) = 0;

  public:
    /**
     * Aligns the allocation size to the machine word size.
     * ex: if n = 6, then result is 8.
     *     if n = 4, result is 4 on x86 and 8 on x64.
     */
    static constexpr size_t align(size_t n) {
      return (n + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1);
    }
  };

  /** General allocator exception. */
  class AllocatorException : public Exception {
  public:
    AllocatorException(std::string message, EXCEPTION_CALLSITE_PARAMS);
  };

  /** Allocation failure exception. */
  class OutOfMemoryException : public AllocatorException {
  public:
    OutOfMemoryException(EXCEPTION_CALLSITE_PARAMS);
    OutOfMemoryException(std::string message, EXCEPTION_CALLSITE_PARAMS);
  };

  /** Double free exception. */
  class DoubleFreeException : public AllocatorException {
  public:
    DoubleFreeException(void* userPointer, EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny