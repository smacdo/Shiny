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
  assert(allocator_ != nullptr);
  return Value{create_string(allocator_.get(), value)};
}

//------------------------------------------------------------------------------
Value VmState::makePair(Value car, Value cdr) {
  assert(allocator_ != nullptr);

  auto rawPair = allocator_->construct<RawPair>();
  rawPair->car = car;
  rawPair->cdr = cdr;

  return Value{rawPair};
}
