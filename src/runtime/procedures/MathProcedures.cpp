#include "runtime/Procedures.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

#include <charconv> // For string->number procedure.
#include <cmath>
#include <limits> // TODO: Remove when RuntimeAPI function TODO is done.

using namespace Shiny;

// TODO: Improve math functions by making them able to handle additional numeric
//       types like floating point and rational. Note that more work will need
//       to done to handle coercion and contagion.
namespace {
  /** True if the predicate is true for each argument. */
  template<typename Predicate>
  bool CompareNumberArgsTest(ArgList& args, Predicate&& func) {
    Value prev;

    if (tryPopArgument(args, &prev, ValueType::Fixnum)) {
      Value next;

      while (tryPopArgument(args, &next, ValueType::Fixnum)) {
        if (!func(prev, next)) {
          // TODO: Make this a RuntimeAPI function that marks all arguments as
          //       handled even if not popped.
          args.popCount = std::numeric_limits<size_t>::max();
          return false;
        }

        prev = next;
      }
    }

    return true;
  }

} // namespace

/** Test if argument is a number. */
Value isNumber_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

/** Test if argument is a complex number. */
Value isComplex_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

/** Test if argument is a real number. */
Value isReal_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

/** Test if argument is a rational number. */
Value isRational_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

/** Test if argument is an integer number. */
Value isInteger_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

/**
 * Test if argument is an exact number.
 * Error if argument is not a number.
 */
Value isExact_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::True;
}

/**
 * Test if argument is an inexact number.
 * Error if argument is not a number.
 */
Value isInexact_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::False;
}

/** Test if argument is an exact integer otherwise returns false. */
Value isExactInteger_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::True;
}

/**
 * Returns true if numeric argument is a rational number and not +inf, -inf,
 * or +nan. For complex numbers this procedure returns true if both the real
 * and imaginary parts are finite.
 */
Value isFinite_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::True;
}

/**
 * Returns true if numeric argument is a rational number and is +inf or -inf.
 * For complex numbers this procedure returns true if either the real,
 * imaginary or both parts are infinite.
 */
Value isInfinite_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::False;
}

/**
 * Returns true if the numeric argument is a rational number and is NaN. For
 * complex numbers this procedure returns true if either the real, imaginary
 * or both parts are NaN.
 */
Value isNan_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::False;
}

/** Test if argument is a positive number. */
Value isPositive_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{v.toFixnum() > 0};
  } else {
    return Value::False;
  }
}

/** Test if argument is a negative number. */
Value isNegative_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{v.toFixnum() < 0};
  } else {
    return Value::False;
  }
}

/** Test if argument is an even number. */
Value isEven_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{(v.toFixnum() % 2) == 0};
  } else {
    return Value::False;
  }
}

/** Test if argument is an odd number. */
Value isOdd_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{(v.toFixnum() % 2) != 0};
  } else {
    return Value::False;
  }
}

/** Return true if all arguments are numbers and equal. */
Value isNumberEqual_Proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() == b.toFixnum(); })};
}

/** Return true if all arguments are numbers and monotonically decreasing. */
Value isNumberLess_Proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() < b.toFixnum(); })};
}

/**
 * Return true if all arguments are numbers and monotonically non-increasing.
 */
Value isNumberLessEqual_Proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() <= b.toFixnum(); })};
}

/** Return true if all arguments are numbers and monotonically increasing. */
Value isNumberGreater_Proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() > b.toFixnum(); })};
}

/**
 * Return true if all arguments are numbers and monotonically non-decreasing.
 */
Value isNumberGreaterEqual_Proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() >= b.toFixnum(); })};
}

/** Returns the minimum argument. */
Value min_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args, ValueType::Fixnum);
  auto minValue = v;

  while (tryPopArgument(args, &v, ValueType::Fixnum)) {
    if (v.toFixnum() < minValue.toFixnum()) {
      minValue = v;
    }
  }

  return minValue;
}

/** Returns the maximum argument. */
Value max_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args, ValueType::Fixnum);
  auto maxValue = v;

  while (tryPopArgument(args, &v, ValueType::Fixnum)) {
    if (v.toFixnum() > maxValue.toFixnum()) {
      maxValue = v;
    }
  }

  return maxValue;
}

/** Returns the sum of its arguments. */
Value add_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  Value arg;
  fixnum_t sum = 0;

  while (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    sum += arg.toFixnum();
  }

  return Value{sum};
}

/**
 * Returns either the additive inverse if one argument is provided, or the
 * difference of all of its arguments if more than one argument is provided.
 */
Value sub_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  Value initial = popArgumentOrThrow(args, ValueType::Fixnum);
  Value arg;

  if (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    fixnum_t difference = initial.toFixnum() - arg.toFixnum();

    while (tryPopArgument(args, &arg, ValueType::Fixnum)) {
      difference -= arg.toFixnum();
    }

    return Value{difference};
  } else {
    return Value{-initial.toFixnum()};
  }
}

/** Returns the product of its arguments. */
Value mul_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  Value arg;
  fixnum_t sum = 1;

  while (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    sum *= arg.toFixnum();
  }

  return Value{sum};
}

/**
 * Returns either the multiplicative inverse if one argument is provided, or
 * the quotient of all of its arguments if more than one argument is provided.
 */
Value div_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  // TODO: Handle divide by zero - raise an error.
  Value initial = popArgumentOrThrow(args, ValueType::Fixnum);
  fixnum_t q = initial.toFixnum();

  Value arg;

  if (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    do {
      auto a = arg.toFixnum();
      if (a == 0) {
        // TODO: Handle divide by zero - raise an error.
        q = 0;
      } else {
        q /= a;
      }
    } while (tryPopArgument(args, &arg, ValueType::Fixnum));

    return Value{q};
  } else {
    if (q == 0) {
      // TODO: Handle divide by zero - raise an error.
      return Value{0};
    } else {
      return Value{1 / q};
    }
  }
}

/** Test if value is numerically equal to zero. */
Value isZero_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{v.toFixnum() == 0};
  } else {
    return Value::False;
  }
}

/** Returns the absolute value of its argument. */
Value abs_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args, ValueType::Fixnum);
  return Value{std::abs(v.toFixnum())};
}

/**
 * Divides the first argument by the second argument, and returns the
 * floored result.
 * See the floor procedure for more details on how floor is performed.
 */
Value floorQuotient_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  // TODO: Handle zero denominator.
  auto numer = popArgumentOrThrow(args, ValueType::Fixnum);
  auto denom = popArgumentOrThrow(args, ValueType::Fixnum);
  auto fpNumer = static_cast<flonum_t>(numer.toFixnum());
  auto fpDenom = static_cast<flonum_t>(denom.toFixnum());
  auto quot = std::floor(fpNumer / fpDenom);

  return Value{static_cast<fixnum_t>(quot)};
}

/**
 * Divides the first argument by the second argument, and returns the
 * floored remainder.
 * See the floor procedure for more details on how floor is performed.
 */
Value floorRemainder_proc(ArgList&, VmState&, EnvironmentFrame*) {
  // TODO: Implement correctly.
  // See truncate-floor definition in Scheme R7RS for explanation.
  return Value{0};
}

/**
 * Divides the first argument by the second argument, and returns the
 * truncated result.
 * See the truncation procedure for more details on how truncation is
 * performed.
 */
Value truncateQuotient_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  // TODO: Handle zero denominator.
  auto numer = popArgumentOrThrow(args, ValueType::Fixnum);
  auto denom = popArgumentOrThrow(args, ValueType::Fixnum);
  auto fpNumer = static_cast<flonum_t>(numer.toFixnum());
  auto fpDenom = static_cast<flonum_t>(denom.toFixnum());
  auto quot = std::trunc(fpNumer / fpDenom);

  return Value{static_cast<fixnum_t>(quot)};
}

/**
 * Divides the first argument by the second argument, and returns the
 * truncated remainder.
 * See the truncation procedure for more details on how truncation is
 * performed.
 */
Value truncateRemainder_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  // TODO: Handle zero denominator.
  auto numer = popArgumentOrThrow(args, ValueType::Fixnum);
  auto denom = popArgumentOrThrow(args, ValueType::Fixnum);
  auto fpNumer = static_cast<flonum_t>(numer.toFixnum());
  auto fpDenom = static_cast<flonum_t>(denom.toFixnum());
  auto rem = std::trunc(std::remainder(fpNumer, fpDenom));

  return Value{static_cast<fixnum_t>(rem)};
}

/** Returns the largest integer not greater than the numeric argument. */
Value floor_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

/** Returns the smallest integer not smaller than the numeric argument. */
Value ceiling_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

/**
 * Returns the integer closest to the numeric argument whose absolute value is
 * not larger than the absolute value of the given numeric argument.
 */
Value truncate_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

/**
 * Returns the closest integer to the numeric argument, rouding to even when
 * the numeric argument is halfway between two integers.
 */
Value round_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

#include <iostream> // TODO: REMOVE

/**
 * Takes a number as an argument and returns a string that is the external
 * representation of said number.
 */
Value numberToString_proc(ArgList& args, VmState& vm, EnvironmentFrame*) {
  // Char buffer large enough to hold a maximum/minimum sized fixnum_t to string
  // form.
  //
  // Why +2?
  //
  // The first +1 is because maxdigits returns the minimum number of digits
  // required without loss, so for a 'char' it returns 2 (0..99) instead of what
  // we need which is 3 (0.255).
  //
  // The second +1 is because negative numbers need an additional character for
  // the minus sign.
  std::array<char, std::numeric_limits<fixnum_t>::digits10 + 2> buffer;

  // Get arguments.
  auto num = popArgumentOrThrow(args, ValueType::Fixnum);

  // Convert and write to a temporary stack allocated string buffer.
  // TODO: Support other bases than 10.
  std::errc success{};
  auto result = std::to_chars(
      buffer.data(), buffer.data() + buffer.size(), num.toFixnum(), 10);

  if (result.ec != success) {
    // This should not happen and if it does then there's a bug in how we
    // determine the size of 'buffer'.
    throw Exception(
        "fixnum too large for numberToString internal char buffer",
        EXCEPTION_CALLSITE_ARGS);
  }

  // Create a new string with the convertd number and return it to the caller.
  auto charsWritten = static_cast<size_t>(result.ptr - buffer.data());
  assert(charsWritten <= buffer.size()); // One past end means <= not <.

  return Value{vm.makeString(std::string_view(buffer.data(), charsWritten))};
}

/**
 * Returns a number of the maximally precise representation expressed by the
 * given string.
 *
 * If the argument cannot be represented as a numeric value than this procedure
 * will return #f.
 */
Value stringToNumber_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  // TODO: Reject numbers like "4p" or "-3z".
  // TODO: Support other bases than 10.
  // TODO: Re-use the reader to ensure consistency in numeric parsing?
  auto s = popArgumentOrThrow(args, ValueType::String);
  auto sv = s.toStringView();

  fixnum_t fixnum;
  auto result = std::from_chars(sv.data(), sv.data() + sv.size(), fixnum, 10);

  if (result.ec == std::errc::invalid_argument) {
    return Value::False;
  }

  return Value{fixnum};
}

//------------------------------------------------------------------------------
void Shiny::registerMathProcs(VmState& vm, EnvironmentFrame* env) {
  using namespace ProcedureNames;

  static const std::array<PrimitiveProcDesc, 40> P{
      PrimitiveProcDesc{kIsNumber, &isNumber_proc},
      PrimitiveProcDesc{kIsComplex, &isComplex_proc},
      PrimitiveProcDesc{kIsReal, &isReal_proc},
      PrimitiveProcDesc{kIsRational, &isRational_proc},
      PrimitiveProcDesc{kIsInteger, &isInteger_proc},

      PrimitiveProcDesc{kIsExact, &isExact_proc},
      PrimitiveProcDesc{kIsInexact, &isInexact_proc},
      PrimitiveProcDesc{kIsExactInteger, &isExactInteger_proc},
      PrimitiveProcDesc{kIsFinite, &isFinite_proc},
      PrimitiveProcDesc{kIsInfinite, &isInfinite_proc},
      PrimitiveProcDesc{kIsNan, &isNan_proc},

      PrimitiveProcDesc{kIsZero, &isZero_proc},
      PrimitiveProcDesc{kIsPositive, &isPositive_proc},
      PrimitiveProcDesc{kIsNegative, &isNegative_proc},
      PrimitiveProcDesc{kIsEven, &isEven_proc},
      PrimitiveProcDesc{kIsOdd, &isOdd_proc},

      PrimitiveProcDesc{kIsNumberEqual, &isNumberEqual_Proc},
      PrimitiveProcDesc{kIsNumberLess, &isNumberLess_Proc},
      PrimitiveProcDesc{kIsNumberLessEqual, &isNumberLessEqual_Proc},
      PrimitiveProcDesc{kIsNumberGreater, &isNumberGreater_Proc},
      PrimitiveProcDesc{kIsNumberGreaterEqual, &isNumberGreaterEqual_Proc},

      PrimitiveProcDesc{kMin, &min_proc},
      PrimitiveProcDesc{kMax, &max_proc},

      PrimitiveProcDesc{kAdd, &add_proc},
      PrimitiveProcDesc{kSubtract, &sub_proc},
      PrimitiveProcDesc{kMultiply, &mul_proc},
      PrimitiveProcDesc{kDivide, &div_proc},

      PrimitiveProcDesc{kAbs, &abs_proc},
      PrimitiveProcDesc{kFloorQuotient, &floorQuotient_proc},
      PrimitiveProcDesc{kFloorRemainder, &floorRemainder_proc},
      PrimitiveProcDesc{kTruncateQuotient, &truncateQuotient_proc},
      PrimitiveProcDesc{kTruncateRemainder, &truncateRemainder_proc},
      PrimitiveProcDesc{kQuotient, &truncateQuotient_proc},
      PrimitiveProcDesc{kRemainder, &truncateRemainder_proc},
      PrimitiveProcDesc{kFloor, &floor_proc},
      PrimitiveProcDesc{kCeiling, &ceiling_proc},
      PrimitiveProcDesc{kTruncate, &truncate_proc},
      PrimitiveProcDesc{kRound, &round_proc},

      PrimitiveProcDesc{"number->string", &numberToString_proc},
      PrimitiveProcDesc{"string->number", &stringToNumber_proc},
  };

  defineProcedures(P.data(), P.size(), vm, env);
}
