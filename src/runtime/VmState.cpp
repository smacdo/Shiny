#include "runtime/VmState.h"

#include "runtime/Allocator.h"
#include "runtime/Builtins.h"
#include "runtime/RawString.h"
#include "runtime/Value.h"

#include <cassert>

using namespace Shiny;

//------------------------------------------------------------------------------
VmState::VmState(std::unique_ptr<Allocator> allocator)
    : allocator_(std::move(allocator)) {
  assert(allocator_ != nullptr);

  // Auto-register builtins.
  makeSymbol(SpecialForms::kQuote);
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

//------------------------------------------------------------------------------
Value VmState::makeSymbol(std::string_view name) {
  // TODO: Use a hashtable solution instead of linked list.
  // TODO: Move this to a SymbolTable class.
  // Search for an existing symbol with the same name.
  Value current = symbols_;

  while (!current.isEmptyList()) {
    auto symbol = car(current);
    assert(symbol.isSymbol());

    if (symbol.toStringView() == name) {
      // Matching symbol name found! Return it to the caller.
      return symbol;
    } else {
      current = cdr(current);
    }
  }

  // Create and add a new symbol to the symbol table.
  assert(allocator_ != nullptr);

  auto symbol = Value{create_string(allocator_.get(), name), ValueType::Symbol};
  symbols_ = cons(this, symbol, symbols_);

  return symbol;
}
