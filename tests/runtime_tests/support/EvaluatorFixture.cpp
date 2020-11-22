#include "EvaluatorFixture.h"

#include "TestHelpers.h"

#include "runtime/Value.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

#include <catch2/catch.hpp>

using namespace Shiny;

std::map<std::string, int> EvaluatorFixture::procCallCounter_;

//------------------------------------------------------------------------------
EvaluatorFixture::EvaluatorFixture()
    : vmState_(std::make_shared<VmState>(std::make_unique<MallocAllocator>())) {
  procCallCounter_.clear();
}

//------------------------------------------------------------------------------
VmState& EvaluatorFixture::vmStateRef() { return *vmState_.get(); }

//------------------------------------------------------------------------------
Value EvaluatorFixture::evaluate(std::string_view code) {
  return Shiny::evaluate(code, vmState_);
}

//------------------------------------------------------------------------------
void EvaluatorFixture::defineProc(const std::string& name, procedure_t proc) {
  REQUIRE(nullptr != proc);

  // define call counter.
  REQUIRE(procCallCounter_.find(name) == procCallCounter_.end());
  procCallCounter_[name] = 0;

  // register.
  vmState_->environment().defineVariable(
      vmState_->makeSymbol(name), Value{proc});
}

//------------------------------------------------------------------------------
int EvaluatorFixture::getProcCallCount(const std::string& name) {
  return procCallCounter_.at(name);
}

//------------------------------------------------------------------------------
void EvaluatorFixture::incProcCallCount(const std::string& name) {
  procCallCounter_.at(name) += 1;
}
