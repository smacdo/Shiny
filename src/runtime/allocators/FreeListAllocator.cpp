#include "runtime/allocators/FreeListAllocator.h"

#include <fmt/format.h>

#include <cassert>
#include <unistd.h>

using namespace Shiny;
using namespace Shiny::FreeListImpl;

//------------------------------------------------------------------------------
FreeListAllocator::~FreeListAllocator() { reset(); }

//------------------------------------------------------------------------------
void* FreeListAllocator::allocate(size_t requestedSizeInBytes) {
  // Support zero sized byte allocations by rounding up to one.
  auto sizeInBytes = (requestedSizeInBytes > 0 ? requestedSizeInBytes : 1);

  // Ensure the requested allocation size is rounded up to the nearest aligned
  // size.
  auto alignedSizeInBytes = align(sizeInBytes);

  // Search for an available free block to reuse before allocating a new block.
  auto block = findFreeBlock(alignedSizeInBytes);

  if (block == nullptr) {
    // No free block found - allocate a new block for this request.
    block = allocateBlock(alignedSizeInBytes);
    byteCount_ +=
        requestedSizeInBytes; // Track the original requested byte count.

    // Set the start of the heap to this block if the heap hasn't been
    // initialized (ie this is the first allocation).
    if (heapRoot_ == nullptr) {
      heapRoot_ = block;
    }

    // Append the block to the back of the heap so it can be tracked.
    if (heapBack_ != nullptr) {
      heapBack_->next = block;
    }

    heapBack_ = block;
  }

  // Mark block as being used before returning the payload poriton back to the
  // caller.
  block->isUsed = true;
  return reinterpret_cast<void*>(block->data);
}

//------------------------------------------------------------------------------
Block* FreeListAllocator::allocateBlock(size_t sizeInBytes) {
  assert(sizeInBytes > 0);

  // Get a pointer to the current heap break (end of the heap). This will be the
  // beginning position of our newly allocated block.
  auto block = reinterpret_cast<Block*>(sbrk(0));

  // Bump the heap break by the number of bytes required for this allocation,
  // and check for a failed allocation after doing this.
  auto actualSizeInBytes = getAllocationSizeInBytes(sizeInBytes);

  if (sbrk(actualSizeInBytes) == reinterpret_cast<void*>(-1)) {
    throw OutOfMemoryException(
        fmt::format(
            "Allocation {} bytes failed because out of memory", sizeInBytes),
        EXCEPTION_CALLSITE_ARGS);
  }

  // Initialize header.
  block->sizeInBytes = sizeInBytes;
  block->isUsed = true;
  block->next = nullptr;

  // Update statistics before returning the block.
  blockCount_++;
  actualByteCount_ += actualSizeInBytes;

  return block;
}

//------------------------------------------------------------------------------
Block* FreeListAllocator::findFreeBlock(size_t sizeInBytes) {
  return findFirstFreeFitBlock(sizeInBytes);
}

//------------------------------------------------------------------------------
Block* FreeListAllocator::findFirstFreeFitBlock(size_t sizeInBytes) {
  // Search begins where the last block was allocated, or at the start of the
  // heap if this is the first find.
  if (findStart_ == nullptr) {
    findStart_ = heapRoot_;
  }

  // Look for the first block that can fit the request.
  // O(n) linear search.
  auto block = findStart_;

  while (block != nullptr) {
    // Check if this block is free and of the right size to be re-used.
    if (!block->isUsed && block->sizeInBytes >= sizeInBytes) {
      findStart_ = block;
      return block;
    }

    // Break out once we reach the block where the start began.
    if (block->next == findStart_ ||
        (block->next == nullptr && findStart_ == heapRoot_)) {
      break;
    }

    // Move to the next block, or go back to the start if we've reached the end
    // of the heap.
    if (block->next == nullptr) {
      block = heapRoot_;
    } else {
      block = block->next;
    }
  }

  // Failed to find a block.
  return nullptr;
}

//------------------------------------------------------------------------------
void FreeListAllocator::destroy(void* userPointer) {
  // Nothing needs to be done when destroying a null pointer.
  if (userPointer == nullptr) {
    return;
  }

  freeBlock(getHeader(userPointer));
}

//------------------------------------------------------------------------------
void FreeListAllocator::reset() {
  // Nothing to do if already reset.
  if (heapRoot_ == nullptr) {
    return;
  }

  // Check if blocks should be freed before reset.
  if (freeBeforeReset_) {
    freeHeap();
  }

  // Reset the program data segment pointer back to where it was prior to any
  // allocation by this allocator.
  // TODO: Switch to mmap to enable multiple concurrent heaps.
  brk(heapRoot_);

  heapRoot_ = nullptr;
  heapBack_ = nullptr;
  findStart_ = nullptr;

  blockCount_ = 0;
  byteCount_ = 0;
  actualByteCount_ = 0;
}

//------------------------------------------------------------------------------
void FreeListAllocator::freeHeap() {
  auto block = heapRoot_;

  while (block != nullptr) {
    auto nextBlock = block->next;

    if (block->isUsed) {
      freeBlock(block);
    }

    block = nextBlock;
  }

  // TODO: Return the used space back to the operating system.
  // (probably once we switch to mmap).
}

//------------------------------------------------------------------------------
void FreeListAllocator::freeBlock(Block* block) {
  assert(block != nullptr);

  // Don't let a block be freed more than once.
  if (!block->isUsed) {
    throw DoubleFreeException(
        reinterpret_cast<void*>(block->data), EXCEPTION_CALLSITE_ARGS);
  }

  // Mark as free to let allocator reuse the block.
  block->isUsed = false;

  // Clear memory contents if requested.
  if (clearAfterFree_) {
    memset(reinterpret_cast<void*>(block->data), 0xC0FEFE, block->sizeInBytes);
  }
}

//------------------------------------------------------------------------------
const Block* FreeListAllocator::getHeader(void* userPointer) const {
  return reinterpret_cast<const Block*>(
      reinterpret_cast<char*>(userPointer) +
      sizeof(std::declval<Block>().data) - sizeof(Block));
}

//------------------------------------------------------------------------------
Block* FreeListAllocator::getHeader(void* userPointer) {
  return reinterpret_cast<Block*>(
      reinterpret_cast<char*>(userPointer) +
      sizeof(std::declval<Block>().data) - sizeof(Block));
}

//------------------------------------------------------------------------------
size_t FreeListAllocator::getAllocationSizeInBytes(size_t userSizeInBytes) {
  // Since the block structure includes the first word of user data (C++
  // doesn't allow zero sized arrays), we subtract it from the size request.
  return userSizeInBytes + sizeof(Block) - sizeof(std::declval<Block>().data);
}
