#include "runtime/VmState.h"

#include "runtime/Allocator.h"
#include "runtime/EnvironmentFrame.h"
#include "runtime/RawString.h"
#include "runtime/Value.h"

#include <cassert>

using namespace Shiny;

//------------------------------------------------------------------------------
VmState::VmState(std::unique_ptr<Allocator> allocator)
    : allocator_(std::move(allocator)),
      globalEnvironment_(std::make_unique<EnvironmentFrame>()) {
  assert(allocator_ != nullptr);
}

//------------------------------------------------------------------------------
VmState::~VmState() = default;

//------------------------------------------------------------------------------
EnvironmentFrame* VmState::globalEnvironment() noexcept {
  return globalEnvironment_.get();
}

//------------------------------------------------------------------------------
const EnvironmentFrame* VmState::globalEnvironment() const noexcept {
  return globalEnvironment_.get();
}

//------------------------------------------------------------------------------
EnvironmentFrame* VmState::extend(EnvironmentFrame* parent) {
  return extend(parent, Value::EmptyList, Value::EmptyList);
}

//------------------------------------------------------------------------------
EnvironmentFrame*
    VmState::extend(EnvironmentFrame* parent, Value names, Value values) {
  assert(parent != nullptr);
  auto env = allocator_->construct<EnvironmentFrame>(parent);

  // Populate the new environment frame with the passed bindings by iterating
  // through the list of binding names and values.
  auto nameItr = names;
  auto valueItr = values;

  assert(nameItr.type() == valueItr.type());
  assert(nameItr.isPair() || nameItr.isEmptyList());

  while (!nameItr.isEmptyList()) {
    // Get the next name and value.
    auto name = car(nameItr);
    assert(name.isSymbol());

    auto value = car(valueItr);

    // Register the binding.
    // TODO: Check at runtime that this is not a duplicate definition instead of
    //       relying on this assert.
    auto didDefine = env->define(name, value);
    assert(didDefine);

    // Move to the next name and value binding entry.
    nameItr = cdr(nameItr);
    valueItr = cdr(valueItr);

    assert(nameItr.type() == valueItr.type());
    assert(nameItr.isPair() || nameItr.isEmptyList());
  }

  return env;
}

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
Value VmState::makeList(std::initializer_list<Value> values) {
  assert(allocator_ != nullptr);

  if (values.size() == 0) {
    return Value::EmptyList;
  } else {
    Value head;
    Value next;

    for (auto v : values) {
      auto n = makePair(v, Value::EmptyList);

      if (head == Value::EmptyList) {
        head = n;
        next = n;
      } else {
        next.toRawPair()->cdr = n;
        next = n;
      }
    }

    return head;
  }
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

//------------------------------------------------------------------------------
Value VmState::makeCompoundProcedure(
    Value parameters,
    Value body,
    EnvironmentFrame* enclosing) {
  assert(enclosing != nullptr);

  // Count the number of parameters in the compound procedure. Also double check
  // that all the parameters are symbols.
  // TODO: Check that no parameter name is repeated.
  Value nextParam = parameters;
  size_t paramCount = 0;

  while (nextParam.isPair()) {
    auto param = car(nextParam);
    nextParam = cdr(nextParam);

    if (param.isSymbol()) {
      paramCount++;
    } else {
      throw WrongValueTypeException(
          ValueType::Symbol, param.type(), EXCEPTION_CALLSITE_ARGS);
    }
  }

  assert(nextParam.isEmptyList());

  // Construct and return a new compound procedure entry.
  auto proc = allocator_->construct<CompoundProcedure>();

  proc->parameters = parameters;
  proc->parameterCount = paramCount;
  proc->body = body;
  proc->enclosingFrame = enclosing;

  return Value{proc};
}
