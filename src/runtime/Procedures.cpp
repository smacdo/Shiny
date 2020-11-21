#include "runtime/Procedures.h"

#include "runtime/Environment.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

//------------------------------------------------------------------------------
void Shiny::registerBuiltinProcedures(VmState& vm, Environment& env) {
  using namespace ProcedureNames;

  env.defineVariable(vm.makeSymbol(kAdd), Value{&add_proc});
  env.defineVariable(vm.makeSymbol(kDebugPrintVars), Value{&printVars_proc});
}

//------------------------------------------------------------------------------
Value Shiny::add_proc(Value args, VmState&, Environment&) {
  // TODO: Handle cast to double.
  Value nextArg;
  fixnum_t sum = 0;

  while (tryPopArgument(args, &nextArg, &args, ValueType::Fixnum)) {
    sum += nextArg.toFixnum();
  }

  return Value{sum};
}

//------------------------------------------------------------------------------
Value Shiny::printVars_proc(Value, VmState&, Environment&) {
  // TODO: Implement me!
  return Value{};
}