#include "runtime/RuntimeApi.h"

#include "runtime/Value.h"

#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
ArgList::ArgList() : next(Value::EmptyList), popCount(0) {}

//------------------------------------------------------------------------------
ArgList::ArgList(Value args) : next(args), popCount(0) {
  if (!next.isPair() && !next.isEmptyList()) {
    throw WrongValueTypeException(
        ValueType::Pair, next.type(), EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
bool Shiny::tryPopArgument(
    ArgList& args,
    Value* argOut,
    std::optional<ValueType> expectedType) {
  Value temp;
  auto argOutPtr = (argOut == nullptr ? &temp : argOut);
  auto status = popArgument(args, argOutPtr, expectedType);

  switch (status) {
    case PopArgResult::Ok:
      return true;
    case PopArgResult::EmptyArgList:
      return false;
    case PopArgResult::WrongArgValueType:
      assert(args.popCount > 0);
      throw WrongArgTypeException(
          args.popCount - 1,
          expectedType.value(),
          argOutPtr->type(),
          EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
Value Shiny::popArgumentOrThrow(
    ArgList& args,
    std::optional<ValueType> expectedType) {
  Value value;
  auto result = popArgument(args, &value, expectedType);

  switch (result) {
    case PopArgResult::Ok:
      return value;
    case PopArgResult::EmptyArgList:
      throw ArgumentMissingException(args.popCount, EXCEPTION_CALLSITE_ARGS);
      break;
    case PopArgResult::WrongArgValueType:
      assert(args.popCount > 0);
      throw WrongArgTypeException(
          args.popCount - 1,
          expectedType.value(),
          value.type(),
          EXCEPTION_CALLSITE_ARGS);
      break;
  }
}

//------------------------------------------------------------------------------
PopArgResult Shiny::popArgument(
    ArgList& args,
    Value* argOut,
    std::optional<ValueType> expectedType) {
  switch (args.next.type()) {
    case ValueType::EmptyList:
      // Can't pop arguments when the argument list is an empty list!
      return PopArgResult::EmptyArgList;
    case ValueType::Pair: {
      // Fetch the next argument. Args and argsTailOut might be the same
      // variable so it is important to fetch the next argument first before
      // modifying argsTailOut.
      Value value, tail;
      get_pair(args.next, &value, &tail);

      // Write value of next argument if requested.
      if (argOut != nullptr) {
        *argOut = value;
      }

      // Advance argument list.
      args.next = tail;
      args.popCount++;

      // Type check the argument if an expected type was provided.
      if (expectedType.has_value() && expectedType.value() != value.type()) {
        return PopArgResult::WrongArgValueType;
      } else {
        return PopArgResult::Ok;
      }
    }
    default:
      // Argument list must be a pair so this is a wrong type.
      assert(false && "should never happen");
      throw Exception(
          "ArgList should always verify type to prevent this case",
          EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
WrongArgTypeException::WrongArgTypeException(
    size_t argumentIndex,
    ValueType expectedType,
    ValueType actualType,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Expected argument #{} to be of type {} but was {}",
              argumentIndex,
              to_string(expectedType),
              to_string(actualType)),
          EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
ArgumentMissingException::ArgumentMissingException(
    size_t argumentIndex,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Expected argument #{} but no more arguments were left",
              argumentIndex),
          EXCEPTION_INIT_BASE_ARGS) {}
