
#pragma once
#include "runtime/Value.h"

namespace Shiny {
  class Environment;
  struct ArgList;

  /** Register all builtin procedures. */
  void registerBuiltinProcedures(VmState& vm, Environment& env);

  /** Returns the sum of its arguments. */
  Value add_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns either the additive negation if one argument is provided, or the
   * difference of all of its arguments if more than one argument is provided.
   */
  Value sub_proc(ArgList& args, VmState& vm, Environment& env);

  /** Prints all bound variables. */
  Value printVars_proc(ArgList& args, VmState& vm, Environment& env);

  namespace ProcedureNames {
    constexpr const char* kDebugPrintVars = "debug-print-vars";
    constexpr const char* kAdd = "+";
    constexpr const char* kSubtract = "-";
    constexpr const char* kMultiply = "*";
    constexpr const char* kDivide = "/";
  } // namespace ProcedureNames

  namespace SpecialForms {
    constexpr const char* kDefine = "define";
    constexpr const char* kIf = "if";
    constexpr const char* kQuote = "quote";
    constexpr const char* kSet = "set!";
  } // namespace SpecialForms
} // namespace Shiny
