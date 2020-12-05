#include "runtime/EnvironmentFrame.h"
#include "runtime/Exception.h"

#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
EnvironmentFrame::EnvironmentFrame(EnvironmentFrame* parent) noexcept
    : parent_(parent) {}

//------------------------------------------------------------------------------
bool EnvironmentFrame::define(Value name, Value value) {
  // Only symbols are allowed for variable names.
  if (!name.isSymbol()) {
    throw VariableNameSymbolRequiredException(name, EXCEPTION_CALLSITE_ARGS);
  }

  // Set an existing variable to the new value if a variable of the same name
  // exists otherwise create a new entry.
  size_t existingIndex = 0;

  if (tryFindSlot(name, &existingIndex)) {
    variables_[existingIndex].value = value;
    return false;
  } else {
    variables_.push_back({name, value});
    return true;
  }
}

//------------------------------------------------------------------------------
Value EnvironmentFrame::lookup(Value name, SearchMode mode) const {
  Value result;

  if (!tryLookup(name, mode, &result)) {
    throw UnboundVariableException(name, EXCEPTION_CALLSITE_ARGS);
  }

  return result;
}

//------------------------------------------------------------------------------
bool EnvironmentFrame::tryLookup(Value name, Value* resultOut) const {
  return tryLookup(name, SearchMode::Recurse, resultOut);
}

//------------------------------------------------------------------------------
bool EnvironmentFrame::tryLookup(Value name, SearchMode mode, Value* resultOut)
    const {
  // Only symbols are allowed for variable names.
  if (!name.isSymbol()) {
    throw VariableNameSymbolRequiredException(name, EXCEPTION_CALLSITE_ARGS);
  }

  // Search current EnvironmentFrame for variable name. If it does not exist
  // recursively search the enclosing enviroment.
  size_t existingIndex = 0;

  if (tryFindSlot(name, &existingIndex)) {
    if (resultOut != nullptr) {
      *resultOut = variables_[existingIndex].value;
    }

    return true;
  } else if (parent_ != nullptr && mode == SearchMode::Recurse) {
    return parent_->tryLookup(name, resultOut);
  } else {
    return false;
  }
}

//------------------------------------------------------------------------------
void EnvironmentFrame::set(Value name, Value value) {
  if (!trySet(name, value)) {
    throw UnboundVariableException(name, EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
bool EnvironmentFrame::trySet(Value name, Value value) {
  // Only symbols are allowed for variable names.
  if (!name.isSymbol()) {
    throw VariableNameSymbolRequiredException(name, EXCEPTION_CALLSITE_ARGS);
  }

  // Search current EnvironmentFrame for variable name, or if it does not exist
  // recursively search its enclosing EnvironmentFrame. If no variable could be
  // found for this name then return false to to indicate no variable was
  // changed.
  size_t existingIndex = 0;

  if (tryFindSlot(name, &existingIndex)) {
    variables_[existingIndex].value = value;
    return true;
  } else if (parent_ != nullptr) {
    // TODO: Optimize by using iteration instead of recursion.
    return parent_->trySet(name, value);
  } else {
    return false;
  }
}

//------------------------------------------------------------------------------
bool EnvironmentFrame::tryFindSlot(Value name, size_t* indexOut)
    const noexcept {
  for (size_t i = 0; i < variables_.size(); ++i) {
    if (variables_[i].name == name) {
      if (indexOut != nullptr) {
        *indexOut = i;
      }

      return true;
    }
  }

  return false;
}

//==============================================================================
VariableNameSymbolRequiredException::VariableNameSymbolRequiredException(
    Value name,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Expected variable name to be type symbol but was {}",
              to_string(name.type())),
          EXCEPTION_INIT_BASE_ARGS) {}

//==============================================================================
UnboundVariableException::UnboundVariableException(
    Value name,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format("Unbound variable {}", name.toString()),
          EXCEPTION_INIT_BASE_ARGS) {}
