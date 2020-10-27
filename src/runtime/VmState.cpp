#include "runtime/VmState.h"

#include "runtime/Allocator.h"
#include "runtime/RawString.h"
#include "runtime/Value.h"

#include <cassert>

using namespace Shiny;

//------------------------------------------------------------------------------
VmState::VmState(std::unique_ptr<Allocator> allocator)
    : allocator_(std::move(allocator)) {
  assert(allocator_ != nullptr);
}

//------------------------------------------------------------------------------
VmState::~VmState() = default;

//------------------------------------------------------------------------------
Value VmState::makeString(std::string_view value) {
  return Value{create_string(allocator_.get(), value)};
}
