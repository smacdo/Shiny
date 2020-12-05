#include "runtime/Procedures.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

/** Returns true if argument is a pair, false otherwise. */
Value isPair_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{ValueType::Pair == v.type()};
}

/**
 * Returns a newly allocated pair whose car is the first argument and cdr is the
 * second argument.
 */
Value cons_proc(ArgList& args, VmState& vm, EnvironmentFrame*) {
  auto a = popArgumentOrThrow(args);
  auto b = popArgumentOrThrow(args);
  return vm.makePair(a, b);
}

/**
 * Returns the contents of the car field of the argument which is expected to be
 * a pair.
 */
Value car_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto p = popArgumentOrThrow(args, ValueType::Pair);
  return car(p);
}

/**
 * Returns the contents of the cdr field of the argument which is expected to be
 * a pair.
 */
Value cdr_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto p = popArgumentOrThrow(args, ValueType::Pair);
  return cdr(p);
}

//------------------------------------------------------------------------------
void Shiny::registerPairProcs(VmState& vm, EnvironmentFrame* env) {
  using namespace ProcedureNames;

  static const std::array<PrimitiveProcDesc, 4> P{
      PrimitiveProcDesc{"pair?", &isPair_proc},
      PrimitiveProcDesc("cons", &cons_proc),
      PrimitiveProcDesc("car", &car_proc),
      PrimitiveProcDesc("cdr", &cdr_proc),
  };

  defineProcedures(P.data(), P.size(), vm, env);
}
