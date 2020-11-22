#pragma once

#include "runtime/Value.h"

#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace Shiny {
  class VmState;

  class EvaluatorFixture {
  public:
    EvaluatorFixture();

  protected:
    VmState& vmStateRef();
    Value evaluate(std::string_view code);
    void defineProc(const std::string& name, procedure_t proc);

    static int getProcCallCount(const std::string& name);
    static void incProcCallCount(const std::string& name);

  protected:
    static std::map<std::string, int> procCallCounter_;
    std::shared_ptr<VmState> vmState_;
  };
} // namespace Shiny
