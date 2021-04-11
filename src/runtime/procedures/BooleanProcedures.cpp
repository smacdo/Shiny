#include "runtime/Procedures.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

/** Returns true if argument is boolean, false otherwise. */
Value isBoolean_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{ValueType::Boolean == v.type()};
}

//------------------------------------------------------------------------------
void Shiny::registerBooleanProcs(VmState& vm, EnvironmentFrame* env) {
  using namespace ProcedureNames;

  static const std::array<PrimitiveProcDesc, 1> P{
      PrimitiveProcDesc{"boolean?", &isBoolean_proc},
  };

  defineProcedures(P.data(), P.size(), vm, env);
}
