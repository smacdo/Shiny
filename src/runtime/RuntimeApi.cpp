#include "runtime/RuntimeApi.h"

#include "runtime/Value.h"

#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
bool Shiny::tryPopArgument(
    Value args,
    Value* resultOut,
    Value* argsTailOut,
    std::optional<ValueType> expectedType) {
  assert(resultOut != nullptr && "Must be set for exception message to work");
  auto status = popArgument(args, resultOut, argsTailOut, expectedType);

  switch (status) {
    case PopArgResult::Ok:
      return true;
    case PopArgResult::EmptyArgList:
      return false;
    case PopArgResult::WrongArgListType:
      throw WrongValueTypeException(
          ValueType::Pair, args.type(), EXCEPTION_CALLSITE_ARGS);
    case PopArgResult::WrongArgValueType:
      throw WrongArgTypeException(
          expectedType.value(), resultOut->type(), EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
Value Shiny::popArgumentOrThrow(
    Value args,
    Value* argsTailOut,
    std::optional<ValueType> expectedType) {

  Value value;
  auto result = popArgument(args, &value, argsTailOut, expectedType);

  switch (result) {
    case PopArgResult::Ok:
      return value;
    case PopArgResult::EmptyArgList:
      throw ArgumentListEmptyException(EXCEPTION_CALLSITE_ARGS);
      break;
    case PopArgResult::WrongArgListType:
      throw WrongValueTypeException(
          ValueType::Pair, args.type(), EXCEPTION_CALLSITE_ARGS);
      break;
    case PopArgResult::WrongArgValueType:
      throw WrongArgTypeException(
          expectedType.value(), value.type(), EXCEPTION_CALLSITE_ARGS);
      break;
  }
}

//------------------------------------------------------------------------------
PopArgResult Shiny::popArgument(
    Value args,
    Value* resultOut,
    Value* argsTailOut,
    std::optional<ValueType> expectedType) {
  if (args.isEmptyList()) {
    // Can't pop arguments when the argument list is an empty list!
    return PopArgResult::EmptyArgList;
  } else if (args.isPair()) {
    // Fetch the next argument. Args and argsTailOut might be the same variable
    // so it is important to fetch the next argument first before modifying
    // argsTailOut.
    Value value, tail;
    get_pair(args, &value, &tail);

    // Write value of next argument if requested.
    if (resultOut != nullptr) {
      *resultOut = value;
    }

    // Write arg list tail if requested.
    if (argsTailOut != nullptr) {
      *argsTailOut = tail;
    }

    // Type check the argument if an expected type was provided.
    if (expectedType.has_value() && expectedType.value() != value.type()) {
      return PopArgResult::WrongArgValueType;
    } else {
      return PopArgResult::Ok;
    }
  } else {
    // Argument list must be a pair so this is a wrong type.
    return PopArgResult::WrongArgListType;
  }
}

//------------------------------------------------------------------------------
WrongArgTypeException::WrongArgTypeException(
    ValueType expectedType,
    ValueType actualType,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Expected argument to be of type {} but was {}",
              to_string(expectedType),
              to_string(actualType)),
          EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
ArgumentListEmptyException::ArgumentListEmptyException(
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          "Expected argument but no more arguments were found",
          EXCEPTION_INIT_BASE_ARGS) {}
