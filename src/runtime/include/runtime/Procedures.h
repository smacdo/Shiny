
#pragma once
#include "runtime/Value.h"

#include <string>

namespace Shiny {
  class EnvironmentFrame;
  struct ArgList;

  /** Metadata for a native procedure. */
  struct PrimitiveProcDesc {
    PrimitiveProcDesc(const char* name, procedure_t func)
        : name(name),
          func(func) {}

    /** Unique identifier name for a native procedure. */
    std::string name;

    /** Pointer to native procedure. */
    procedure_t func;
  };

  /** Define multiple procedures at the same time. */
  void defineProcedures( // TODO: Is this a RuntimeAPI function?
      const PrimitiveProcDesc procs[],
      size_t count,
      VmState& vm,
      EnvironmentFrame* env);

  /** Register all builtin procedures. */
  void registerBuiltinProcedures(VmState& vm, EnvironmentFrame* env);

  void registerMathProcs(VmState& vm, EnvironmentFrame* env);
  void registerPairProcs(VmState& vm, EnvironmentFrame* env);

  namespace ProcedureNames {
    constexpr const char* kIsNumber = "number?";
    constexpr const char* kIsComplex = "complex?";
    constexpr const char* kIsReal = "real?";
    constexpr const char* kIsRational = "rational?";
    constexpr const char* kIsInteger = "integer?";
    constexpr const char* kIsExact = "exact?";
    constexpr const char* kIsInexact = "inexact?";
    constexpr const char* kIsExactInteger = "exact-integer?";
    constexpr const char* kIsFinite = "finite?";
    constexpr const char* kIsInfinite = "infinite?";
    constexpr const char* kIsNan = "nan?";
    constexpr const char* kIsZero = "zero?";
    constexpr const char* kIsPositive = "positive?";
    constexpr const char* kIsNegative = "negative?";
    constexpr const char* kIsEven = "even?";
    constexpr const char* kIsOdd = "odd?";

    constexpr const char* kIsNumberEqual = "=";
    constexpr const char* kIsNumberLess = "<";
    constexpr const char* kIsNumberLessEqual = "<=";
    constexpr const char* kIsNumberGreater = ">";
    constexpr const char* kIsNumberGreaterEqual = ">=";

    constexpr const char* kMin = "min";
    constexpr const char* kMax = "max";

    constexpr const char* kAdd = "+";
    constexpr const char* kSubtract = "-";
    constexpr const char* kMultiply = "*";
    constexpr const char* kDivide = "/";

    constexpr const char* kAbs = "abs";
    constexpr const char* kFloorQuotient = "floor-quotient";
    constexpr const char* kFloorRemainder = "floor-remainder";
    constexpr const char* kTruncateQuotient = "truncate-quotient";
    constexpr const char* kTruncateRemainder = "truncate-remainder";
    constexpr const char* kQuotient = "quotient";
    constexpr const char* kRemainder = "remainder";

    constexpr const char* kFloor = "floor";
    constexpr const char* kCeiling = "ceiling";
    constexpr const char* kTruncate = "truncate";
    constexpr const char* kRound = "round";

    constexpr const char* kDebugPrintVars = "debug-print-vars";
  } // namespace ProcedureNames

  namespace SpecialForms {
    constexpr const char* kDefine = "define";
    constexpr const char* kIf = "if";
    constexpr const char* kLambda = "lambda";
    constexpr const char* kQuote = "quote";
    constexpr const char* kSet = "set!";
  } // namespace SpecialForms
} // namespace Shiny
