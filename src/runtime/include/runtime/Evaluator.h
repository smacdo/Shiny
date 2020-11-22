#pragma once
#include "runtime/Value.h"

#include <memory>
#include <tuple>

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
    Value evaluate(Value expression);

    /** Check if a value is self-evaluating. */
    static bool isSelfEvaluating(Value value);

  private:
    /**
     * Construct and return a new argument list containing the evaluated results
     * of the passed arguments. Also return the number of arguments present.
     */
    Value evaluateArgumentList(Value args, Environment& env, size_t* argCount);

    Value ifProc(Value arguments, VmState& vm);

    static Value defineProc(Value arguments, VmState& vm);
    static Value quoteProc(Value arguments, VmState& vm);
    static Value setProc(Value arguments, VmState& vm);

  private:
    std::shared_ptr<VmState> vmState_;

    // TODO: Use a LUT for this.
    Shiny::Value defineSymbol_;
    Shiny::Value ifSymbol_;
    Shiny::Value quoteSymbol_;
    Shiny::Value setSymbol_;
  };

  /** More arguments were passed than consumed by the invoked procedure. */
  class ArgCountMismatch : public Exception {
  public:
    ArgCountMismatch(
        size_t expectedCount,
        size_t actualCount,
        EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny
