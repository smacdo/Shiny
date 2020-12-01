
#pragma once
#include "runtime/Value.h"

namespace Shiny {
  class Environment;
  struct ArgList;

  /** Register all builtin procedures. */
  void registerBuiltinProcedures(VmState& vm, Environment& env);

  /** Test if argument is a number. */
  Value isNumber_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is a complex number. */
  Value isComplex_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is a real number. */
  Value isReal_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is a rational number. */
  Value isRational_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is an integer number. */
  Value isInteger_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns true if numeric argument is a rational number and not +inf, -inf,
   * or +nan. For complex numbers this procedure returns true if both the real
   * and imaginary parts are finite.
   */
  Value isFinite_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns true if numeric argument is a rational number and is +inf or -inf.
   * For complex numbers this procedure returns true if either the real,
   * imaginary or both parts are infinite.
   */
  Value isInfinite_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns true if the numeric argument is a rational number and is NaN. For
   * complex numbers this procedure returns true if either the real, imaginary
   * or both parts are NaN.
   */
  Value isNan_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Test if argument is an exact number.
   * Error if argument is not a number.
   */
  Value isExact_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Test if argument is an inexact number.
   * Error if argument is not a number.
   */
  Value isInexact_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is an exact integer otherwise returns false. */
  Value isExactInteger_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is a positive number. */
  Value isPositive_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is a negative number. */
  Value isNegative_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is an even number. */
  Value isEven_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if argument is an odd number. */
  Value isOdd_proc(ArgList& args, VmState& vm, Environment& env);

  /** Return true if all arguments are numbers and equal. */
  Value isNumberEqual_Proc(ArgList& args, VmState& vm, Environment& env);

  /** Return true if all arguments are numbers and monotonically increasing. */
  Value isNumberGreater_Proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Return true if all arguments are numbers and monotonically non-decreasing.
   */
  Value isNumberGreaterEqual_Proc(ArgList& args, VmState& vm, Environment& env);

  /** Return true if all arguments are numbers and monotonically decreasing. */
  Value isNumberLess_Proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Return true if all arguments are numbers and monotonically non-increasing.
   */
  Value isNumberLessEqual_Proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the minimum argument. */
  Value min_proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the maximum argument. */
  Value max_proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the sum of its arguments. */
  Value add_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns either the additive inverse if one argument is provided, or the
   * difference of all of its arguments if more than one argument is provided.
   */
  Value sub_proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the product of its arguments. */
  Value mul_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns either the multiplicative inverse if one argument is provided, or
   * the quotient of all of its arguments if more than one argument is provided.
   */
  Value div_proc(ArgList& args, VmState& vm, Environment& env);

  /** Test if value is numerically equal to zero. */
  Value isZero_proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the absolute value of its argument. */
  Value abs_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Divides the first argument by the second argument, and returns the
   * floored result.
   * See the floor procedure for more details on how floor is performed.
   */
  Value floorQuotient_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Divides the first argument by the second argument, and returns the
   * floored remainder.
   * See the floor procedure for more details on how floor is performed.
   */
  Value floorRemainder_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Divides the first argument by the second argument, and returns the
   * truncated result.
   * See the truncation procedure for more details on how truncation is
   * performed.
   */
  Value truncateQuotient_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Divides the first argument by the second argument, and returns the
   * truncated remainder.
   * See the truncation procedure for more details on how truncation is
   * performed.
   */
  Value truncateRemainder_proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the largest integer not greater than the numeric argument. */
  Value floor_proc(ArgList& args, VmState& vm, Environment& env);

  /** Returns the smallest integer not smaller than the numeric argument. */
  Value ceiling_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns the integer closest to the numeric argument whose absolute value is
   * not larger than the absolute value of the given numeric argument.
   */
  Value truncate_proc(ArgList& args, VmState& vm, Environment& env);

  /**
   * Returns the closest integer to the numeric argument, rouding to even when
   * the numeric argument is halfway between two integers.
   */
  Value round_proc(ArgList& args, VmState& vm, Environment& env);

  /** Prints all bound variables. */
  Value printVars_proc(ArgList& args, VmState& vm, Environment& env);

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
    constexpr const char* kQuote = "quote";
    constexpr const char* kSet = "set!";
  } // namespace SpecialForms
} // namespace Shiny
