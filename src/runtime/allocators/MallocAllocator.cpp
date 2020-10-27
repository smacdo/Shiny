#include "runtime/allocators/MallocAllocator.h"

#include <fmt/format.h>

#include <cassert>
#include <unistd.h>

using namespace Shiny;
using namespace Shiny::MallocImpl;

//------------------------------------------------------------------------------
MallocAllocator::~MallocAllocator() { reset(); }

//------------------------------------------------------------------------------
void* MallocAllocator::allocate(size_t requestedSizeInBytes) {
  // Support zero sized byte allocations by rounding up to one.
  auto sizeInBytes = (requestedSizeInBytes > 0 ? requestedSizeInBytes : 1);

  // Ensure the requested allocation size is rounded up to the nearest aligned
  // size before allocating the block.
  auto alignedSizeInBytes = align(sizeInBytes);

  auto block = allocateBlock(alignedSizeInBytes);
  byteCount_ +=
      requestedSizeInBytes; // Track the original requested byte count.

  // Set the start of the heap to this block if the heap hasn't been
  // initialized (ie this is the first allocation).
  if (heapRoot_ == nullptr) {
    heapRoot_ = block;
  } else {
    // Append the block to the back of the heap so it can be tracked.
    heapBack_->next = block;
    block->prev = heapBack_;
  }

  heapBack_ = block;

  // Returning the payload poriton back to the caller.
  return reinterpret_cast<void*>(block->data);
}

//------------------------------------------------------------------------------
Block* MallocAllocator::allocateBlock(size_t sizeInBytes) {
  assert(sizeInBytes > 0);

  // Allocate enough space to store an allocation header block alongside the
  // memory space requested by the caller.
  auto actualSizeInBytes = getAllocationSizeInBytes(sizeInBytes);
  auto block = reinterpret_cast<Block*>(malloc(actualSizeInBytes));

  if (block == nullptr) {
    // Honestly if we're out of memory this is going to work...
    // TODO: Handle reporting out of memory for malloc.
    throw OutOfMemoryException(
        fmt::format(
            "Allocation {} bytes failed because out of memory", sizeInBytes),
        EXCEPTION_CALLSITE_ARGS);
  }

  // Initialize header.
  block->sizeInBytes = sizeInBytes;
  block->next = nullptr;
  block->prev = nullptr;

  // Update statistics before returning the block.
  blockCount_++;
  actualByteCount_ += actualSizeInBytes;

  return block;
}

//------------------------------------------------------------------------------
void MallocAllocator::destroy(void* userPointer) {
  // Nothing needs to be done when destroying a null pointer.
  if (userPointer == nullptr) {
    return;
  }

  freeBlock(getHeader(userPointer));
}

//------------------------------------------------------------------------------
void MallocAllocator::reset() {
  // Make sure any still alive allocations are freed.
  // TODO: Log warnings for allocations that are found here.
  if (freeOnReset_) {
    freeHeap();
  }

  // Reset state to initial.
  heapRoot_ = nullptr;
  heapBack_ = nullptr;

  blockCount_ = 0;
  byteCount_ = 0;
  actualByteCount_ = 0;
}

//------------------------------------------------------------------------------
void MallocAllocator::freeHeap() {
  auto block = heapRoot_;

  while (block != nullptr) {
    auto nextBlock = block->next;
    freeBlock(block);

    block = nextBlock;
  }
}

//------------------------------------------------------------------------------
void MallocAllocator::freeBlock(Block* block) {
  assert(block != nullptr);

  // Remove the block from the list of allocations. Make sure to keep the head
  // and tail fields up to date.
  if (heapRoot_ == heapBack_) {
    assert(heapRoot_->prev == nullptr);
    assert(heapRoot_->next == nullptr);

    heapRoot_ = nullptr;
    heapBack_ = nullptr;
  } else {
    if (block->prev == nullptr) {
      assert(heapRoot_ == block && "this must be the head node");
      assert(blockCount_ > 1);
      assert(block->next != nullptr);

      heapRoot_ = block->next;
    } else {
      assert(heapRoot_ != block && "this cannot be head node");
      assert(block->prev->next == block);

      block->prev->next = block->next;
    }

    if (block->next == nullptr) {
      assert(heapBack_ == block && "this must be the tail node");
      assert(blockCount_ > 1);
      assert(block->prev != nullptr);

      heapBack_ = block->prev;
    } else {
      assert(heapBack_ != nullptr && "this cannot be the tail node");
      assert(block->next->prev == block);

      block->next->prev = block->prev;
    }
  }

  // Clear memory contents if requested.
  if (clearOnFree_) {
    block->next = block->prev = nullptr;
    block->sizeInBytes = 0;

    memset(reinterpret_cast<void*>(block->data), 0xC0FEFE, block->sizeInBytes);
  }

  // Now free the block.
  free(block);
}

//------------------------------------------------------------------------------
const Block* MallocAllocator::getHeader(void* userPointer) const {
  return reinterpret_cast<const Block*>(
      reinterpret_cast<char*>(userPointer) +
      sizeof(std::declval<Block>().data) - sizeof(Block));
}

//------------------------------------------------------------------------------
Block* MallocAllocator::getHeader(void* userPointer) {
  return reinterpret_cast<Block*>(
      reinterpret_cast<char*>(userPointer) +
      sizeof(std::declval<Block>().data) - sizeof(Block));
}

//------------------------------------------------------------------------------
size_t MallocAllocator::getAllocationSizeInBytes(size_t userSizeInBytes) {
  // Since the block structure includes the first word of user data (C++
  // doesn't allow zero sized arrays), we subtract it from the size request.
  return userSizeInBytes + sizeof(Block) - sizeof(std::declval<Block>().data);
}
