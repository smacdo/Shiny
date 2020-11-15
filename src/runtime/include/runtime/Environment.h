#pragma once
#include "runtime/Exception.h"
#include "runtime/Value.h"

#include <vector>

namespace Shiny {
  /** Stores and retrieves variables. */
  class Environment {
    struct Variable {
      Value name;
      Value value;
    };

  public:
    /** Initialize a new environment with an optional parent. */
    Environment(Environment* enclosing = nullptr);

    /** Define variable in this environment. */
    bool defineVariable(Value name, Value value);

    /** Look up a variable or throw an exception if not found. */
    Value lookupVariable(Value name) const;

    /** Lookup a value in this environment. */
    bool tryLookupVariable(Value name, Value* resultOut) const;

    /** Set a variable value or throw an exception if not defined. */
    void setVariable(Value name, Value value);

    /** Try to set a variable value if it is defined or return false. */
    bool trySetVariable(Value name, Value value);

  private:
    bool tryFindVariableSlot(Value name, size_t* indexOut) const noexcept;

  private:
    /** This environment's parent (enclosing) environment. */
    Environment* enclosing_;
    std::vector<Variable> variables_; // TODO: Make this much better.
  };

  class VariableNameSymbolRequiredException : public Exception {
  public:
    VariableNameSymbolRequiredException(Value name, EXCEPTION_CALLSITE_PARAMS);
  };

  class UnboundVariableException : public Exception {
  public:
    UnboundVariableException(Value name, EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny
