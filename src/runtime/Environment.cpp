#include "runtime/Environment.h"
#include "runtime/Exception.h"

#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
Environment::Environment(Environment* enclosing) : enclosing_(enclosing) {}

//------------------------------------------------------------------------------
bool Environment::defineVariable(Value name, Value value) {
  // Only symbols are allowed for variable names.
  if (!name.isSymbol()) {
    throw VariableNameSymbolRequiredException(name, EXCEPTION_CALLSITE_ARGS);
  }

  // Set an existing variable to the new value if a variable of the same name
  // exists otherwise create a new entry.
  size_t existingIndex = 0;

  if (tryFindVariableSlot(name, &existingIndex)) {
    variables_[existingIndex].value = value;
    return false;
  } else {
    variables_.push_back({name, value});
    return true;
  }
}

//------------------------------------------------------------------------------
Value Environment::lookupVariable(Value name) const {
  Value result;

  if (!tryLookupVariable(name, &result)) {
    throw UnboundVariableException(name, EXCEPTION_CALLSITE_ARGS);
  }

  return result;
}

//------------------------------------------------------------------------------
bool Environment::tryLookupVariable(Value name, Value* resultOut) const {
  // Only symbols are allowed for variable names.
  if (!name.isSymbol()) {
    throw VariableNameSymbolRequiredException(name, EXCEPTION_CALLSITE_ARGS);
  }

  // Search current environment for variable name. If it does not exist
  // recursively search the enclosing enviroment.
  size_t existingIndex = 0;

  if (tryFindVariableSlot(name, &existingIndex)) {
    if (resultOut != nullptr) {
      *resultOut = variables_[existingIndex].value;
    }

    return true;
  } else if (enclosing_ != nullptr) {
    return enclosing_->tryLookupVariable(name, resultOut);
  } else {
    return false;
  }
}

//------------------------------------------------------------------------------
void Environment::setVariable(Value name, Value value) {
  if (!trySetVariable(name, value)) {
    throw UnboundVariableException(name, EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
bool Environment::trySetVariable(Value name, Value value) {
  // Only symbols are allowed for variable names.
  if (!name.isSymbol()) {
    throw VariableNameSymbolRequiredException(name, EXCEPTION_CALLSITE_ARGS);
  }

  // Search current environment for variable name, or if it does not exist
  // recursively search its enclosing environment. If no variable could be found
  // for this name then return false to to indicate no variable was changed.
  size_t existingIndex = 0;

  if (tryFindVariableSlot(name, &existingIndex)) {
    variables_[existingIndex].value = value;
    return true;
  } else if (enclosing_ != nullptr) {
    // TODO: Optimize by using iteration instead of recursion.
    return enclosing_->trySetVariable(name, value);
  } else {
    return false;
  }
}

//------------------------------------------------------------------------------
bool Environment::tryFindVariableSlot(Value name, size_t* indexOut)
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
