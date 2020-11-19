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
      throw Exception(
          "Procedure operator must be an identifier (symbol)",
          EXCEPTION_CALLSITE_ARGS);
    }

    // TODO: EVALUATE.
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
      throw Exception(
          fmt::format("Unknown procedure operator '{}'", op.toStringView()),
          EXCEPTION_CALLSITE_ARGS);
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

#include <iostream>

//------------------------------------------------------------------------------
Shiny::Value Evaluator::ifProc(Shiny::Value arguments, VmState&) {
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
Shiny::Value Evaluator::defineProc(Shiny::Value arguments, VmState& vmState) {
  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  vmState.environment().defineVariable(varName, varValue);
  return varValue;
}

//------------------------------------------------------------------------------
Shiny::Value Evaluator::quoteProc(Shiny::Value arguments, VmState&) {
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
Shiny::Value Evaluator::setProc(Shiny::Value arguments, VmState& vmState) {
  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  vmState.environment().setVariable(varName, varValue);
  return varValue;
}
