#include "runtime/Procedures.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

/** Returns true if argument is a symbol, false otherwise. */
Value isSymbol_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{ValueType::Symbol == v.type()};
}

/** Returns the name of the symbol as a string value. */
Value symbolToString_proc(ArgList& args, VmState& vm, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args, ValueType::Symbol);
  return Value{vm.makeString(v.toStringView())};
}

/** Returns a symbol with whose name is the string argument. */
Value stringToSymbol_proc(ArgList& args, VmState& vm, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args, ValueType::String);
  return Value{vm.makeSymbol(v.toStringView())};
}

//------------------------------------------------------------------------------
void Shiny::registerSymbolProcs(VmState& vm, EnvironmentFrame* env) {
  using namespace ProcedureNames;

  static const std::array<PrimitiveProcDesc, 3> P{
      PrimitiveProcDesc{"symbol?", &isSymbol_proc},
      PrimitiveProcDesc("symbol->string", &symbolToString_proc),
      PrimitiveProcDesc("string->symbol", &stringToSymbol_proc),
  };

  defineProcedures(P.data(), P.size(), vm, env);
}
