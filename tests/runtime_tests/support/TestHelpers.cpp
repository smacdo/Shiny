#include "TestHelpers.h"

#include "runtime/Evaluator.h"
#include "runtime/Reader.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <iostream>

using namespace Shiny;

//------------------------------------------------------------------------------
Value Shiny::read(std::string_view input) {
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  return read(input, vmState);
}

//------------------------------------------------------------------------------
Value Shiny::read(std::string_view input, std::shared_ptr<VmState> vmState) {
  Reader r{vmState};
  return r.read(input);
}

//------------------------------------------------------------------------------
Value Shiny::evaluate(
    std::string_view input,
    std::shared_ptr<VmState> vmState) {
  Evaluator eval(vmState);
  return eval.evaluate(read(input, vmState));
}

//------------------------------------------------------------------------------
bool Shiny::listEquals(Value expected, Value actual) {
  if (!expected.isPair() || !actual.isPair()) {
    return false;
  }

  auto a = expected;
  auto b = actual;

  int index = 0;

  do {
    auto va = car(a);
    auto vb = car(b);

    a = cdr(a);
    b = cdr(b);

    if (va != vb || (a.isEmptyList() != b.isEmptyList())) {
      std::cerr << "List mismatch at index " << index << ": expected "
                << a.toString() << ", but was " << b.toString() << std::endl;
      return false;
    }

    index++;
  } while (!a.isEmptyList() && !b.isEmptyList());

  return true;
}
