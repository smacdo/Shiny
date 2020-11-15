#include "runtime/Evaluator.h"

#include "runtime/Builtins.h"
#include "runtime/VmState.h"

using namespace Shiny;

//--------------------------------------------------------------------------------------------------
Evaluator::Evaluator(std::shared_ptr<VmState> vmState) : vmState_(vmState) {
  defineSymbol_ = vmState_->makeSymbol(SpecialForms::kDefine);
  quoteSymbol_ = vmState_->makeSymbol(SpecialForms::kQuote);
  setSymbol_ = vmState_->makeSymbol(SpecialForms::kSet);
}

//--------------------------------------------------------------------------------------------------
Evaluator::~Evaluator() = default;

//--------------------------------------------------------------------------------------------------
Value Evaluator::evaluate(Value expression) {
  if (isSelfEvaluating(expression)) {
    return expression;
  } else if (expression.isPair()) {
    return evaluateList(expression);
  } else if (expression.isSymbol()) {
    return vmState_->environment().lookupVariable(expression);
  } else {
    throw Exception(
        "Cannot evaluate unknown expression type", EXCEPTION_CALLSITE_ARGS);
  }
}

//--------------------------------------------------------------------------------------------------
Value Evaluator::evaluateList(Shiny::Value expression) {
  assert(expression.isPair());
  auto procedure = car(expression);

  if (procedure.isSymbol()) {
    return evaluateProcedure(procedure, cdr(expression));
  } else {
    throw Exception(
        "First element of list must be symbol to be evaluatable",
        EXCEPTION_CALLSITE_ARGS);
  }

  // TODO: Variable can appear here.
}

//--------------------------------------------------------------------------------------------------
Value Evaluator::evaluateProcedure(
    Shiny::Value procedure,
    Shiny::Value arguments) {
  assert(procedure.isSymbol());

  // TODO: Optimize this by moving to a lookup table model.
  if (procedure == defineSymbol_) {
    return defineProc(arguments, *vmState_.get());
  } else if (procedure == quoteSymbol_) {
    return quoteProc(arguments, *vmState_.get());
  } else if (procedure == setSymbol_) {
    return setProc(arguments, *vmState_.get());
  } else {
    throw Exception("Unknown procedure", EXCEPTION_CALLSITE_ARGS);
  }
}

//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
Shiny::Value Evaluator::defineProc(Shiny::Value arguments, VmState& vmState) {
  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  vmState.environment().defineVariable(varName, varValue);
  return varValue;
}

//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
Shiny::Value Evaluator::setProc(Shiny::Value arguments, VmState& vmState) {
  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  vmState.environment().setVariable(varName, varValue);
  return varValue;
}
