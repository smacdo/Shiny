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