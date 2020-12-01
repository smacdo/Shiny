#include "runtime/Procedures.h"

#include "runtime/Environment.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

#include <cmath>
#include <limits> // TODO: Remove when RuntimeAPI function TODO is done.

using namespace Shiny;

// TODO: Improve math functions by making them able to handle additional numeric
//       types like floating point and rational. Note that more work will need
//       to done to handle coercion and contagion.
namespace {
  /** Metadata for a native procedure. */
  struct NativeProcedureDesc {
    NativeProcedureDesc(const char* name, procedure_t func)
        : name(name),
          func(func) {}

    /** Unique identifier name for a native procedure. */
    std::string name;

    /** Pointer to native procedure. */
    procedure_t func;
  };

  /** Define multiple procedures at the same time. */
  void DefineProcedures(
      const NativeProcedureDesc procs[],
      size_t count,
      VmState& vm,
      Environment& env) {
    // TODO: Warn/error if duplicate definition?
    for (size_t i = 0; i < count; ++i) {
      const auto& p = procs[i];
      env.defineVariable(vm.makeSymbol(p.name), Value{p.func});
    }
  }

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

  // Table of native math procedures.
  static const std::array<NativeProcedureDesc, 38> MathProcedures{
      NativeProcedureDesc{ProcedureNames::kIsNumber, &isNumber_proc},
      NativeProcedureDesc{ProcedureNames::kIsComplex, &isComplex_proc},
      NativeProcedureDesc{ProcedureNames::kIsReal, &isReal_proc},
      NativeProcedureDesc{ProcedureNames::kIsRational, &isRational_proc},
      NativeProcedureDesc{ProcedureNames::kIsInteger, &isInteger_proc},

      NativeProcedureDesc{ProcedureNames::kIsExact, &isExact_proc},
      NativeProcedureDesc{ProcedureNames::kIsInexact, &isInexact_proc},
      NativeProcedureDesc{
          ProcedureNames::kIsExactInteger,
          &isExactInteger_proc},
      NativeProcedureDesc{ProcedureNames::kIsFinite, &isFinite_proc},
      NativeProcedureDesc{ProcedureNames::kIsInfinite, &isInfinite_proc},
      NativeProcedureDesc{ProcedureNames::kIsNan, &isNan_proc},

      NativeProcedureDesc{ProcedureNames::kIsZero, &isZero_proc},
      NativeProcedureDesc{ProcedureNames::kIsPositive, &isPositive_proc},
      NativeProcedureDesc{ProcedureNames::kIsNegative, &isNegative_proc},
      NativeProcedureDesc{ProcedureNames::kIsEven, &isEven_proc},
      NativeProcedureDesc{ProcedureNames::kIsOdd, &isOdd_proc},

      NativeProcedureDesc{ProcedureNames::kIsNumberEqual, &isNumberEqual_Proc},
      NativeProcedureDesc{ProcedureNames::kIsNumberLess, &isNumberLess_Proc},
      NativeProcedureDesc{
          ProcedureNames::kIsNumberLessEqual,
          &isNumberLessEqual_Proc},
      NativeProcedureDesc{
          ProcedureNames::kIsNumberGreater,
          &isNumberGreater_Proc},
      NativeProcedureDesc{
          ProcedureNames::kIsNumberGreaterEqual,
          &isNumberGreaterEqual_Proc},

      NativeProcedureDesc{ProcedureNames::kMin, &min_proc},
      NativeProcedureDesc{ProcedureNames::kMax, &max_proc},

      NativeProcedureDesc{ProcedureNames::kAdd, &add_proc},
      NativeProcedureDesc{ProcedureNames::kSubtract, &sub_proc},
      NativeProcedureDesc{ProcedureNames::kMultiply, &mul_proc},
      NativeProcedureDesc{ProcedureNames::kDivide, &div_proc},

      NativeProcedureDesc{ProcedureNames::kAbs, &abs_proc},
      NativeProcedureDesc{ProcedureNames::kFloorQuotient, &floorQuotient_proc},
      NativeProcedureDesc{
          ProcedureNames::kFloorRemainder,
          &floorRemainder_proc},
      NativeProcedureDesc{
          ProcedureNames::kTruncateQuotient,
          &truncateQuotient_proc},
      NativeProcedureDesc{
          ProcedureNames::kTruncateRemainder,
          &truncateRemainder_proc},
      NativeProcedureDesc{ProcedureNames::kQuotient, &truncateQuotient_proc},
      NativeProcedureDesc{ProcedureNames::kRemainder, &truncateRemainder_proc},
      NativeProcedureDesc{ProcedureNames::kFloor, &floor_proc},
      NativeProcedureDesc{ProcedureNames::kCeiling, &ceiling_proc},
      NativeProcedureDesc{ProcedureNames::kTruncate, &truncate_proc},
      NativeProcedureDesc{ProcedureNames::kRound, &round_proc},
  };
} // namespace

//------------------------------------------------------------------------------
void Shiny::registerBuiltinProcedures(VmState& vm, Environment& env) {
  DefineProcedures(MathProcedures.data(), MathProcedures.size(), vm, env);
  // defun(kDebugPrintVars, &printVars_proc);
}

//------------------------------------------------------------------------------
Value Shiny::isNumber_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

//------------------------------------------------------------------------------
Value Shiny::isComplex_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

//------------------------------------------------------------------------------
Value Shiny::isReal_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

//------------------------------------------------------------------------------
Value Shiny::isRational_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

//------------------------------------------------------------------------------
Value Shiny::isInteger_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);
  return Value{v.isFixnum()};
}

//------------------------------------------------------------------------------
Value Shiny::isExact_proc(ArgList& args, VmState&, Environment&) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::True;
}

//------------------------------------------------------------------------------
Value Shiny::isInexact_proc(ArgList& args, VmState&, Environment&) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::False;
}

//------------------------------------------------------------------------------
Value Shiny::isExactInteger_proc(ArgList& args, VmState&, Environment&) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::True;
}

//------------------------------------------------------------------------------
Value Shiny::isFinite_proc(ArgList& args, VmState&, Environment&) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::True;
}

//------------------------------------------------------------------------------
Value Shiny::isInfinite_proc(ArgList& args, VmState&, Environment&) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::False;
}

//------------------------------------------------------------------------------
Value Shiny::isNan_proc(ArgList& args, VmState&, Environment&) {
  popArgumentOrThrow(args, ValueType::Fixnum);
  return Value::False;
}

//------------------------------------------------------------------------------
Value Shiny::isPositive_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{v.toFixnum() > 0};
  } else {
    return Value::False;
  }
}

//------------------------------------------------------------------------------
Value Shiny::isNegative_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{v.toFixnum() < 0};
  } else {
    return Value::False;
  }
}

//------------------------------------------------------------------------------
Value Shiny::isEven_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{(v.toFixnum() % 2) == 0};
  } else {
    return Value::False;
  }
}

//------------------------------------------------------------------------------
Value Shiny::isNumberEqual_Proc(ArgList& args, VmState&, Environment&) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() == b.toFixnum(); })};
}

//------------------------------------------------------------------------------
Value Shiny::isNumberLess_Proc(ArgList& args, VmState&, Environment&) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() < b.toFixnum(); })};
}

//------------------------------------------------------------------------------
Value Shiny::isNumberLessEqual_Proc(ArgList& args, VmState&, Environment&) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() <= b.toFixnum(); })};
}

//------------------------------------------------------------------------------
Value Shiny::isNumberGreater_Proc(ArgList& args, VmState&, Environment&) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() > b.toFixnum(); })};
}

//------------------------------------------------------------------------------
Value Shiny::isNumberGreaterEqual_Proc(ArgList& args, VmState&, Environment&) {
  return Value{CompareNumberArgsTest(
      args, [](Value a, Value b) { return a.toFixnum() >= b.toFixnum(); })};
}

//------------------------------------------------------------------------------
Value Shiny::isOdd_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{(v.toFixnum() % 2) != 0};
  } else {
    return Value::False;
  }
}

//------------------------------------------------------------------------------
Value Shiny::min_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args, ValueType::Fixnum);
  auto minValue = v;

  while (tryPopArgument(args, &v, ValueType::Fixnum)) {
    if (v.toFixnum() < minValue.toFixnum()) {
      minValue = v;
    }
  }

  return minValue;
}

//------------------------------------------------------------------------------
Value Shiny::max_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args, ValueType::Fixnum);
  auto maxValue = v;

  while (tryPopArgument(args, &v, ValueType::Fixnum)) {
    if (v.toFixnum() > maxValue.toFixnum()) {
      maxValue = v;
    }
  }

  return maxValue;
}

//------------------------------------------------------------------------------
Value Shiny::add_proc(ArgList& args, VmState&, Environment&) {
  Value arg;
  fixnum_t sum = 0;

  while (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    sum += arg.toFixnum();
  }

  return Value{sum};
}

//------------------------------------------------------------------------------
Value Shiny::sub_proc(ArgList& args, VmState&, Environment&) {
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

//------------------------------------------------------------------------------
Value Shiny::mul_proc(ArgList& args, VmState&, Environment&) {
  Value arg;
  fixnum_t sum = 1;

  while (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    sum *= arg.toFixnum();
  }

  return Value{sum};
}

//------------------------------------------------------------------------------
Value Shiny::div_proc(ArgList& args, VmState&, Environment&) {
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

//------------------------------------------------------------------------------
Value Shiny::isZero_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args);

  if (v.isFixnum()) {
    return Value{v.toFixnum() == 0};
  } else {
    return Value::False;
  }
}

//------------------------------------------------------------------------------
Value Shiny::abs_proc(ArgList& args, VmState&, Environment&) {
  auto v = popArgumentOrThrow(args, ValueType::Fixnum);
  return Value{std::abs(v.toFixnum())};
}

//------------------------------------------------------------------------------
Value Shiny::floorQuotient_proc(ArgList& args, VmState&, Environment&) {
  // TODO: Handle zero denominator.
  auto numer = popArgumentOrThrow(args, ValueType::Fixnum);
  auto denom = popArgumentOrThrow(args, ValueType::Fixnum);
  auto fpNumer = static_cast<flonum_t>(numer.toFixnum());
  auto fpDenom = static_cast<flonum_t>(denom.toFixnum());
  auto quot = std::floor(fpNumer / fpDenom);

  return Value{static_cast<fixnum_t>(quot)};
}

//------------------------------------------------------------------------------
Value Shiny::floorRemainder_proc(ArgList&, VmState&, Environment&) {
  // TODO: Implement correctly.
  // See truncate-floor definition in Scheme R7RS for explanation.
  return Value{0};
}

//------------------------------------------------------------------------------
Value Shiny::truncateQuotient_proc(ArgList& args, VmState&, Environment&) {
  // TODO: Handle zero denominator.
  auto numer = popArgumentOrThrow(args, ValueType::Fixnum);
  auto denom = popArgumentOrThrow(args, ValueType::Fixnum);
  auto fpNumer = static_cast<flonum_t>(numer.toFixnum());
  auto fpDenom = static_cast<flonum_t>(denom.toFixnum());
  auto quot = std::trunc(fpNumer / fpDenom);

  return Value{static_cast<fixnum_t>(quot)};
}

//------------------------------------------------------------------------------
Value Shiny::truncateRemainder_proc(ArgList& args, VmState&, Environment&) {
  // TODO: Handle zero denominator.
  auto numer = popArgumentOrThrow(args, ValueType::Fixnum);
  auto denom = popArgumentOrThrow(args, ValueType::Fixnum);
  auto fpNumer = static_cast<flonum_t>(numer.toFixnum());
  auto fpDenom = static_cast<flonum_t>(denom.toFixnum());
  auto rem = std::trunc(std::remainder(fpNumer, fpDenom));

  return Value{static_cast<fixnum_t>(rem)};
}

//------------------------------------------------------------------------------
Value Shiny::floor_proc(ArgList& args, VmState&, Environment&) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

//------------------------------------------------------------------------------
Value Shiny::ceiling_proc(ArgList& args, VmState&, Environment&) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

//------------------------------------------------------------------------------
Value Shiny::truncate_proc(ArgList& args, VmState&, Environment&) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

//------------------------------------------------------------------------------
Value Shiny::round_proc(ArgList& args, VmState&, Environment&) {
  return popArgumentOrThrow(args, ValueType::Fixnum);
}

//------------------------------------------------------------------------------
Value Shiny::printVars_proc(ArgList&, VmState&, Environment&) {
  // TODO: Implement me!
  return Value{};
}
