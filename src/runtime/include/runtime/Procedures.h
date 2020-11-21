
#pragma once
#include "runtime/Value.h"

namespace Shiny {
  class Environment;

  /** Register all builtin procedures. */
  void registerBuiltinProcedures(VmState& vm, Environment& env);

  /** Returns the sum of their arguments. */
  Value add_proc(Value args, VmState& vm, Environment& env);

  /** Prints all bound variables. */
  Value printVars_proc(Value args, VmState& vm, Environment& env);

  namespace ProcedureNames {
    constexpr const char* kDebugPrintVars = "debug-print-vars";
    constexpr const char* kAdd = "+";
  } // namespace ProcedureNames

  namespace SpecialForms {
    constexpr const char* kDefine = "define";
    constexpr const char* kIf = "if";
    constexpr const char* kQuote = "quote";
    constexpr const char* kSet = "set!";
  } // namespace SpecialForms
} // namespace Shiny
