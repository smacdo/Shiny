#include "runtime/Evaluator.h"

#include "runtime/EnvironmentFrame.h"
#include "runtime/Procedures.h"
#include "runtime/RuntimeApi.h"
#include "runtime/VmState.h"

#include <fmt/format.h>
#include <limits>

using namespace Shiny;

//------------------------------------------------------------------------------
Evaluator::Evaluator(std::shared_ptr<VmState> vmState) : vmState_(vmState) {
  defineSymbol_ = vmState_->makeSymbol(SpecialForms::kDefine);
  ifSymbol_ = vmState_->makeSymbol(SpecialForms::kIf);
  lambdaSymbol_ = vmState_->makeSymbol(SpecialForms::kLambda);
  quoteSymbol_ = vmState_->makeSymbol(SpecialForms::kQuote);
  setSymbol_ = vmState_->makeSymbol(SpecialForms::kSet);

  registerBuiltinProcedures(*vmState.get(), vmState->globalEnvironment());
}

//------------------------------------------------------------------------------
Evaluator::~Evaluator() = default;
#include <iostream>
//------------------------------------------------------------------------------
Value Evaluator::evaluate(Value expression, EnvironmentFrame* env) {
tailcall:
  if (isSelfEvaluating(expression)) {
    // Self evaluating expression.
    return expression;
  } else if (expression.isSymbol()) {
    // Variable evaluation.
    return env->lookup(expression);
  } else if (expression.isPair()) {
    // Procedure call.
    auto op = car(expression);
    auto args = cdr(expression);

    // Check if procedure is a special form, and evaluate those specially.
    // Otherwise evaluate the operator and treat it as a procedure call.
    //
    // TODO: Can we optimize this if..else if..else model to a LUT but keep
    // custom tail call behavior.
    // TODO: Can we evaluate argument count and type in one central place?
    if (op == defineSymbol_) {
      return defineProc(args, *vmState_.get(), env);
    } else if (op == ifSymbol_) {
      expression = ifProc(args, *vmState_.get(), env);
      goto tailcall;
    } else if (op == lambdaSymbol_) {
      auto parameters = car(args);
      auto body = cdr(args);
      return vmState_->makeCompoundProcedure(parameters, body, env);
    } else if (op == quoteSymbol_) {
      return quoteProc(args, *vmState_.get(), env);
    } else if (op == setSymbol_) {
      return setProc(args, *vmState_.get(), env);
    } else {
      // Looks like a standard procedure call. Evaluate the operator to find
      // out what procedure we should invoke.
      auto procedure = evaluate(op, env);

      // Evaluate formal arguments to get a "real" arguments list to pass to the
      // procedure.
      size_t argCount = 0;
      auto argValues = evaluateArgumentList(args, env, &argCount);

      assert(argValues.isPair() || argValues.isEmptyList());

      // Primitive procedures and compound procedures must be invoked
      // differently.
      if (procedure.isPrimitiveProcedure()) {
        // Invoke primitive procedure and return the result.
        return invokePrimitiveProcedure(procedure, argValues, argCount, env);
      } else if (procedure.isCompoundProcedure()) {
        // Invoke a compound procedure. First get the procedure's definition.
        auto compoundProc = procedure.toCompoundProcedure();
        assert(compoundProc != nullptr);

        // Extend the current environment to create a new lexical scope for this
        // procedure. Make sure the parameters names and matching argument
        // values are bound to this new enclosing environment frame.
        env = vmState_->extend(env, compoundProc->parameters, argValues);

        // Evaluate each expression in the compound procedure's body. Scheme
        // requires that the last statement in a lambda be evaluated in a tail
        // context. This means we will recursively call eval for each expression
        // except the last, load the last expression and then use goto to
        // perform a tail call.
        auto exprList = compoundProc->body;

        while (!isLastExpressionInBody(exprList)) {
          auto expr = car(exprList);
          exprList = cdr(exprList);

          evaluate(expr, env);
        }

        expression = car(exprList);
        goto tailcall;
      } else {
        // TODO: OperatorProcedureExpectedException
        throw Exception(
            fmt::format(
                "Expected operator to evaluate to a procedure type but was {}",
                to_string(procedure.type())),
            EXCEPTION_CALLSITE_ARGS);
      }
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
Value Evaluator::invokePrimitiveProcedure(
    Value procedure,
    Value arguments,
    size_t argCount,
    EnvironmentFrame* env) {
  assert(procedure.isPrimitiveProcedure());
  assert(arguments.isPair() || arguments.isEmptyList());
  // TODO: Verify argCount == count(arguments).

  // Construct argument list which allows us to track how many arguments have
  // been consumed by the invoked procedure.
  ArgList arglist;

  arglist.next = arguments;
  arglist.popCount = 0;

  // Invoke the procedure and keep the result.
  auto func = procedure.toPrimitiveProcedure();
  assert(func != nullptr);

  auto result = func(arglist, *vmState_.get(), env);

  // Double check that the invoked procedure actually used all the arguments
  // that it was passed. Once checked return the result of the invoked
  // procedure call.
  if (arglist.popCount != argCount &&
      arglist.popCount != std::numeric_limits<size_t>::max()) {
    // TODO: Unify this exception with ArgumentMissingException.
    throw ArgCountMismatch(arglist.popCount, argCount, EXCEPTION_CALLSITE_ARGS);
  }

  return result;
}

//------------------------------------------------------------------------------
Value Evaluator::evaluateArgumentList(
    Value args,
    EnvironmentFrame* env,
    size_t* argCount) {
  assert(argCount != nullptr);

  if (args.isEmptyList()) {
    return args;
  } else {
    *argCount += 1;

    // Generate a new argument list by evaluating each element of the passed
    // argument list.
    // TODO: Can we optimize this and not allocate a new cell for each arg?
    // TODO: Optimize by not using recursion here.
    return cons(
        vmState_.get(),
        evaluate(car(args), env),
        evaluateArgumentList(cdr(args), env, argCount));
  }
}

//------------------------------------------------------------------------------
Value Evaluator::ifProc(Value arguments, VmState&, EnvironmentFrame* env) {
  // TODO: Check argument validity.

  // Evaluate predicate and then check if it is true or false.
  auto predicate = evaluate(car(arguments), env);

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
Value Evaluator::defineProc(Value arguments, VmState&, EnvironmentFrame* env) {
  assert(env != nullptr);

  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  env->define(varName, varValue);
  return varValue;
}

//------------------------------------------------------------------------------
Value Evaluator::quoteProc(Value arguments, VmState&, EnvironmentFrame*) {
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
Value Evaluator::setProc(Value arguments, VmState&, EnvironmentFrame* env) {
  assert(env != nullptr);

  // TODO: Check argument validity.
  auto varName = car(arguments);
  auto varValue = car(cdr(arguments));

  env->set(varName, varValue);
  return varValue;
}

//------------------------------------------------------------------------------
bool Evaluator::isLastExpressionInBody(Value expressionList) {
  return cdr(expressionList) == Value::EmptyList;
}

//==============================================================================
ArgCountMismatch::ArgCountMismatch(
    size_t expectedCount,
    size_t actualCount,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Expected {} arguments but {} arguments were passed",
              expectedCount,
              actualCount),
          EXCEPTION_INIT_BASE_ARGS) {}
