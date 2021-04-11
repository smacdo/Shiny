#include "runtime/Procedures.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

using namespace Shiny;

/** Returns true if argument is the empty list, false otherwise. */
Value isEmptyList_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  auto v = popArgumentOrThrow(args);
  return Value{ValueType::EmptyList == v.type()};
}

/** Returns true if argument is a list, false otherwise. */
Value isList_proc(ArgList& args, VmState&, EnvironmentFrame*) {
  // TODO: Can this be optimized to be O(1) instead of O(N)?
  auto v = popArgumentOrThrow(args);

  while (v.type() == ValueType::Pair) {
    v = cdr(v);
  }

  return Value{v.type() == ValueType::EmptyList};
}

/** Returns a newly allocated list of its arguments. */
Value list_proc(ArgList& args, VmState& vm, EnvironmentFrame*) {
  auto head = Value::EmptyList;
  Value v;

  if (tryPopArgument(args, &v)) {
    head = cons(&vm, v, Value::EmptyList);
    auto tail = head;

    while (tryPopArgument(args, &v)) {
      auto p = cons(&vm, v, Value::EmptyList);
      set_cdr(tail, p);
      tail = p;
    }
  }

  return head;
}

//------------------------------------------------------------------------------
void Shiny::registerListProcs(VmState& vm, EnvironmentFrame* env) {
  using namespace ProcedureNames;

  // TODO: Move null? to scheme compatibility library.
  static const std::array<PrimitiveProcDesc, 4> P{
      PrimitiveProcDesc{"empty-list?", &isEmptyList_proc},
      PrimitiveProcDesc{"null?", &isEmptyList_proc},
      PrimitiveProcDesc{"list?", &isList_proc},
      PrimitiveProcDesc{"list", &list_proc},
  };

  defineProcedures(P.data(), P.size(), vm, env);
}
