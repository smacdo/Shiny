#include "runtime/Allocator.h"
#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
Allocator::~Allocator() = default;

//------------------------------------------------------------------------------
AllocatorException::AllocatorException(
    std::string message,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(message, EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
OutOfMemoryException::OutOfMemoryException(EXCEPTION_CALLSITE_PARAMS)
    : AllocatorException(
          "Allocation failed because not enough memory left to allocate",
          EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
OutOfMemoryException::OutOfMemoryException(
    std::string message,
    EXCEPTION_CALLSITE_PARAMS)
    : AllocatorException(message, EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
DoubleFreeException::DoubleFreeException(
    void* userPointer,
    EXCEPTION_CALLSITE_PARAMS)
    : AllocatorException(
          fmt::format("Attempted to free {} more than once", userPointer),
          EXCEPTION_INIT_BASE_ARGS) {}