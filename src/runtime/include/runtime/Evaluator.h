#pragma once
#include "runtime/Value.h"
#include <memory>

namespace Shiny {
  class VmState;

  /** Evalutes Scheme code in the context an executing VM. */
  class Evaluator {
  public:
    Evaluator(std::shared_ptr<VmState> vmState);
    ~Evaluator();

    /** Evaluate expression and return result. */
    Shiny::Value evaluate(Shiny::Value expression);

    /** Check if a value is self-evaluating. */
    static bool isSelfEvaluating(Shiny::Value value);

  private:
    Shiny::Value evaluateList(Shiny::Value expression);
    Shiny::Value
    evaluateProcedure(Shiny::Value procedure, Shiny::Value arguments);

    static Shiny::Value defineProc(Shiny::Value arguments, VmState& vm);
    static Shiny::Value quoteProc(Shiny::Value arguments, VmState& vm);
    static Shiny::Value setProc(Shiny::Value arguments, VmState& vm);

  private:
    std::shared_ptr<VmState> vmState_;

    // TODO: Use a LUT for this.
    Shiny::Value defineSymbol_;
    Shiny::Value quoteSymbol_;
    Shiny::Value setSymbol_;
  };

} // namespace Shiny
