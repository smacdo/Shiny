#include "runtime/Procedures.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

//------------------------------------------------------------------------------
void Shiny::registerBuiltinProcedures(VmState& vm, EnvironmentFrame* env) {
  registerMathProcs(vm, env);
  registerPairProcs(vm, env);
}

//------------------------------------------------------------------------------
void Shiny::defineProcedures(
    const PrimitiveProcDesc procs[],
    size_t count,
    VmState& vm,
    EnvironmentFrame* env) {
  // TODO: Warn/error if duplicate definition?
  for (size_t i = 0; i < count; ++i) {
    const auto& p = procs[i];
    env->define(vm.makeSymbol(p.name), Value{p.func});
  }
}
