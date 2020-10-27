#pragma once
#include "runtime/Allocator.h"

#include <cstdint>

namespace Shiny {
  namespace FreeListImpl {
    /**
     * An allocated block of memory. Contains a header that describes the
     * memory block followed by the memory payload.
     */
    struct Block {
      /** Size of the user data portion of this block, in bytes. */
      size_t sizeInBytes;

      /** True if this block is currently in use, false otherwise. */
      bool isUsed;

      /** Pointer to the next block in the list. */
      Block* next;

      /** First byte of user data; remaining bytes follow immediately after. */
      uintptr_t data[1];
    };
  } // namespace FreeListImpl

  /**
   * Allocates and deallocates objects using the program break pointer.
   */
  class FreeListAllocator : public Allocator {
  public:
  public:
    /** Destructor. */
    ~FreeListAllocator();

    /** Allocate a block of memory of at least 'sizeInBytes'. */
    void* allocate(size_t sizeInBytes) override;

    /** Free pointer allocated by this allocator. */
    void destroy(void* userPointer) override;

    /** Reset heap (releases all memory allocated). */
    void reset();

    /** Get the header for a memory block. */
    const FreeListImpl::Block* getHeader(void* userPointer) const;

  private:
    /** Get the header for a memory block. */
    FreeListImpl::Block* getHeader(void* userPointer);

  public:
    /** Get the number of allocations performed by this allocator. */
    size_t allocationCount() const noexcept { return blockCount_; }

    /** Get the number of bytes requested to be allocated. */
    size_t requestedByteCount() const noexcept { return byteCount_; }

  private:
    /** Allocate a chunk of memory. */
    FreeListImpl::Block* allocateBlock(size_t sizeInBytes);

    /** Find a free block at least sizeInBytes large. */
    FreeListImpl::Block* findFreeBlock(size_t sizeInBytes);

    /** Find a free block at least sizeInBytes large. */
    FreeListImpl::Block* findFirstFreeFitBlock(size_t sizeInBytes);

    /** Release all allocations. */
    void freeHeap();

    /** Release single block. */
    void freeBlock(FreeListImpl::Block* block);

    /**
     * Get the total allocation size (in bytes) including the additional space
     * required for the block header and first data word.
     */
    static size_t getAllocationSizeInBytes(size_t userSizeInBytes);

  private:
    FreeListImpl::Block* heapRoot_ = nullptr;
    FreeListImpl::Block* heapBack_ = nullptr;
    FreeListImpl::Block* findStart_ = nullptr;
    size_t blockCount_ = 0;
    size_t byteCount_ = 0;
    size_t actualByteCount_ = 0;
    bool clearAfterFree_ = true;
    bool freeBeforeReset_ = true;
  };
} // namespace Shiny