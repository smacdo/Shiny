#pragma once
#include "runtime/Allocator.h"

#include <cstdint>
#include <string_view>

namespace Shiny {
  namespace MallocImpl {
    /**
     * An allocated block of memory. Contains a header that describes the
     * memory block followed by the memory payload.
     */
    struct Block {
      /** Size of the user data portion of this block, in bytes. */
      size_t sizeInBytes;

      /** Pointer to the next block in the allocation list. */
      Block* next;

      /** Pointer to the previous block in the list. */
      Block* prev;

      /** First byte of user data; remaining bytes follow immediately after. */
      uintptr_t data[1];
    };
  } // namespace MallocImpl

  /**
   * A simple memory allocator that uses malloc for allocation.
   */
  class MallocAllocator : public Allocator {
  public:
  public:
    /** Destructor. */
    ~MallocAllocator();

    /** Allocate a block of memory of at least 'sizeInBytes'. */
    void* allocate(size_t sizeInBytes);

    /** Free pointer allocated by this allocator. */
    void destroy(void* userPointer);

    /** Get the header for a memory block. */
    const MallocImpl::Block* getHeader(void* userPointer) const;

  private:
    /** Get the header for a memory block. */
    MallocImpl::Block* getHeader(void* userPointer);

  public:
    /** Get the number of allocations performed by this allocator. */
    size_t allocationCount() const noexcept { return blockCount_; }

    /** Get the number of bytes requested to be allocated. */
    size_t requestedByteCount() const noexcept { return byteCount_; }

    /**
     * Set if the resetting the allocator should also free any still allocated
     * blocks of memory.
     */
    void setFreeOnReset(bool shouldFree = true) { freeOnReset_ = shouldFree; }

  private:
    /** Reset all allocations made by this allocator. */
    void reset();

    /** Allocate a chunk of memory. */
    MallocImpl::Block* allocateBlock(size_t sizeInBytes);

    /** Release all allocations. */
    void freeHeap();

    /** Release single block. */
    void freeBlock(MallocImpl::Block* block);

    /**
     * Get the total allocation size (in bytes) including the additional space
     * required for the block header and first data word.
     */
    static size_t getAllocationSizeInBytes(size_t userSizeInBytes);

  private:
    MallocImpl::Block* heapRoot_ = nullptr;
    MallocImpl::Block* heapBack_ = nullptr;
    size_t blockCount_ = 0;
    size_t byteCount_ = 0;
    size_t actualByteCount_ = 0;
    bool freeOnReset_ = true;
    bool clearOnFree_ = true;
  };
} // namespace Shiny