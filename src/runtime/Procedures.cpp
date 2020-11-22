#include "runtime/Procedures.h"

#include "runtime/Environment.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

//------------------------------------------------------------------------------
void Shiny::registerBuiltinProcedures(VmState& vm, Environment& env) {
  using namespace ProcedureNames;

  env.defineVariable(vm.makeSymbol(kAdd), Value{&add_proc});
  env.defineVariable(vm.makeSymbol(kSubtract), Value{&sub_proc});

  env.defineVariable(vm.makeSymbol(kDebugPrintVars), Value{&printVars_proc});
}

//------------------------------------------------------------------------------
Value Shiny::add_proc(ArgList& args, VmState&, Environment&) {
  // TODO: Handle other numeric types like float, rational, etc. (+ tests!)
  Value arg;
  fixnum_t sum = 0;

  while (tryPopArgument(args, &arg, ValueType::Fixnum)) {
    sum += arg.toFixnum();
  }

  return Value{sum};
}

//------------------------------------------------------------------------------
Value Shiny::sub_proc(ArgList& args, VmState&, Environment&) {
  // TODO: Handle other numeric types like float, rational, etc. (+ tests!)
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
Value Shiny::printVars_proc(ArgList&, VmState&, Environment&) {
  // TODO: Implement me!
  return Value{};
}