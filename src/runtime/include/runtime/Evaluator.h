#pragma once
#include "runtime/Value.h"
#include <memory>

namespace Shiny {
  class VmState;
  /**
   * Evaluates Scheme code in the context of an executing VM.
   *
   * The current evaluator implementation is a simple AST (abstract syntax
   * tree) evaluator, which is sub-optimal for a real world implementation. It
   * will be replaced with a faster bytecode interpreter in the future but for
   * the moment I am priotizing bootstrapping a functional scheme interpreter
   * over a fast one.
   */
  class Evaluator {
  public:
    Evaluator(std::shared_ptr<VmState> vmState);
    ~Evaluator();

    /** Evaluate expression and return result. */
    Shiny::Value evaluate(Shiny::Value expression);

    /** Check if a value is self-evaluating. */
    static bool isSelfEvaluating(Shiny::Value value);

  private:
    Shiny::Value ifProc(Shiny::Value arguments, VmState& vm);

    static Shiny::Value defineProc(Shiny::Value arguments, VmState& vm);
    static Shiny::Value quoteProc(Shiny::Value arguments, VmState& vm);
    static Shiny::Value setProc(Shiny::Value arguments, VmState& vm);

  private:
    std::shared_ptr<VmState> vmState_;

    // TODO: Use a LUT for this.
    Shiny::Value defineSymbol_;
    Shiny::Value ifSymbol_;
    Shiny::Value quoteSymbol_;
    Shiny::Value setSymbol_;
  };

} // namespace Shiny
