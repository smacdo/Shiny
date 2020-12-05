#pragma once
#include "runtime/Value.h"

#include <memory>
#include <tuple>

namespace Shiny {
  class EnvironmentFrame;
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
    Value evaluate(Value expression, EnvironmentFrame* env);

    /** Check if a value is self-evaluating. */
    static bool isSelfEvaluating(Value value);

  private:
    /**
     * Invokes a primitive procedure with the given list of already evaluated
     * argument values and an enclosing environment.
     *
     * This method may throw exceptions if the arguments do not match the
     * primitive procedure's parameter definitions.
     *
     * \param procedure The procedure to invoke.
     * \param arguments A list of argument values to pass to the procedure.
     * \param argCount  The number of arguments passed to the procedure.
     * \param env       The environment frame enclosing the procedure call.
     * \returns The returned value of the procedure call if there was no error.
     */
    Value invokePrimitiveProcedure(
        Value procedure,
        Value arguments,
        size_t argCount,
        EnvironmentFrame* env);

    /**
     * Construct and return a new argument list containing the evaluated results
     * of the passed arguments. Also return the number of arguments present.
     */
    Value evaluateArgumentList(
        Value args,
        EnvironmentFrame* env,
        size_t* argCount);

    Value ifProc(Value arguments, VmState& vm, EnvironmentFrame* env);

    static Value
        defineProc(Value arguments, VmState& vm, EnvironmentFrame* env);
    static Value quoteProc(Value arguments, VmState& vm, EnvironmentFrame* env);
    static Value setProc(Value arguments, VmState& vm, EnvironmentFrame* env);

    static bool isLastExpressionInBody(Value expressionList);

  private:
    std::shared_ptr<VmState> vmState_;

    // TODO: Use a LUT for this.
    Shiny::Value defineSymbol_;
    Shiny::Value ifSymbol_;
    Shiny::Value lambdaSymbol_;
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
