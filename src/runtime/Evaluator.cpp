#include "runtime/Evaluator.h"

#include "runtime/Procedures.h"
#include "runtime/VmState.h"

#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
Evaluator::Evaluator(std::shared_ptr<VmState> vmState) : vmState_(vmState) {
  defineSymbol_ = vmState_->makeSymbol(SpecialForms::kDefine);
  ifSymbol_ = vmState_->makeSymbol(SpecialForms::kIf);
  quoteSymbol_ = vmState_->makeSymbol(SpecialForms::kQuote);
  setSymbol_ = vmState_->makeSymbol(SpecialForms::kSet);

  registerBuiltinProcedures(*vmState.get(), vmState->environment());
}

//------------------------------------------------------------------------------
Evaluator::~Evaluator() = default;

//------------------------------------------------------------------------------
Value Evaluator::evaluate(Value expression) {
tailcall:
  if (isSelfEvaluating(expression)) {
    // Self evaluating expression.
    return expression;
  } else if (expression.isSymbol()) {
    // Variable evaluation.
    return vmState_->environment().lookupVariable(expression);
  } else if (expression.isPair()) {
    // Procedure call.
    auto op = car(expression);
    auto args = cdr(expression);

    // Operator identifier must be a symbol.
    if (!op.isSymbol()) {
      // TODO: Explicit exception?
      throw Exception(
          "Procedure operator must be an identifier (symbol)",
          EXCEPTION_CALLSITE_ARGS);
    }

    // Check if procedure is a special form, and evaluate those specially.
    // Otherwise evaluate the operator and treat it as a procedure call.

    // TODO: Can we optimize this if..else if..else model to a LUT but keep
    // custom tail call behavior.
    // TODO: Can we evaluate argument count and type in one central place?
    if (op == defineSymbol_) {
      return defineProc(args, *vmState_.get());
    } else if (op == ifSymbol_) {
      expression = ifProc(args, *vmState_.get());
      goto tailcall;
    } else if (op == quoteSymbol_) {
      return quoteProc(args, *vmState_.get());
    } else if (op == setSymbol_) {
      return setProc(args, *vmState_.get());
    } else {
      // Looks like a standard procedure call. Evaluate the operator to find
      // out what procedure we should invoke.
      auto procedure = evaluate(op);

      // The operand must evaluate to a procedure type.
      if (!procedure.isPrimitiveProcedure()) {
        // TODO: OperatorProcedureExpectedException
        throw Exception(
            fmt::format(
                "Expected operator to evaluate to procedure but was {}",
                to_string(procedure.type())),
            EXCEPTION_CALLSITE_ARGS);
      }

      // Evaluate formal arguments to get a "real" arguments list to pass to the
      // procedure.
      auto argValues = evaluateArgumentList(args, vmState_->environment());
      assert(argValues.isPair() || argValues.isEmptyList());

      // Invoke the procedure and return the result.
      auto func = procedure.toPrimitiveProcedure();
      assert(func != nullptr);

      return func(argValues, *vmState_.get(), vmState_->environment());
    }
  } else {
    // Oops, don't know what to do with this...
    // TODO: Better error message?
    throw Exception(
        "Cannot evaluate unknown expression type", EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
bool Evaluator::isSelfEvaluating(Value value) {
  switch (value.type()) {
    case ValueType::Boolean:
    case ValueType::Fixnum:
    case ValueType::Character:
    case ValueType::String:
      return true;
    default:
      return false;
  }
}

//------------------------------------------------------------------------------
Value Evaluator::evaluateArgumentList(Value args, Environment& env) {
  if (args.isEmptyList()) {
    return args;
  } else {
    // Generate a new argument list by evaluating each element of the passed
    // argument list.
    // TODO: Can we optimize this and not allocate a new cell for each arg?
    // TODO: Optimize by not using recursion here.
    return cons(
        vmState_.get(),
        evaluate(car(args)),
        evaluateArgumentList(cdr(args), env));
  }
}

//------------------------------------------------------------------------------
Value Evaluator::ifProc(Value arguments, VmState&) {
  // TODO: Check argument validity.

  // Evaluate predicate and then check if it is true or false.
  auto predicate = evaluate(car(arguments));

  if (predicate.isTrue()) {
    // Return the consequent for evaluation.
    return cadr(arguments);
  } else {
    // Return the alternative for evaluation if present, otherwise return false.
    if (cddr(arguments).isEmptyList()) {
      return Value{false};
    } else {
      return caddr(arguments);
    }
  }
}

//------------------------------------------------------------------------------
Value Evaluator::defineProc(Value arguments, VmState& vmState) {
  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  vmState.environment().defineVariable(varName, varValue);
  return varValue;
}

//------------------------------------------------------------------------------
Value Evaluator::quoteProc(Value arguments, VmState&) {
  if (!arguments.isPair()) {
    // TODO: Throw specific evaluator exception + update test.
    throw Exception(
        "Quote special form expects at least one argument",
        EXCEPTION_CALLSITE_ARGS);
  }

  if (!cdr(arguments).isEmptyList()) {
    // TODO: Throw specific evaluator exception + update test.
    throw Exception(
        "Quote special form expects no more than one argument",
        EXCEPTION_CALLSITE_ARGS);
  }

  return car(arguments);
}

//------------------------------------------------------------------------------
Value Evaluator::setProc(Value arguments, VmState& vmState) {
  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  vmState.environment().setVariable(varName, varValue);
  return varValue;
}
