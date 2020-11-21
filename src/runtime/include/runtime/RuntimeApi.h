#pragma once

#include "Value.h"

#include <optional>

namespace Shiny {
  /** Result of trying to pop the next argument from an argument list. */
  enum class PopArgResult {
    Ok,               ///< Argument was succesfully popped.
    EmptyArgList,     ///< Argument list was empty.
    WrongArgListType, ///< Argument list is wrong type (requires pair).
    WrongArgValueType ///< Popped argument type did not match expected.
  };

  /** Argument value type does not match expected type. */
  class WrongArgTypeException : public Exception {
  public:
    WrongArgTypeException(
        ValueType expectedType,
        ValueType actualType,
        EXCEPTION_CALLSITE_PARAMS);
  };

  /** Expected argument but argument list was empty. */
  // TODO: Consider renaming this to ArgumentMissingException.
  class ArgumentListEmptyException : public Exception {
  public:
    ArgumentListEmptyException(EXCEPTION_CALLSITE_PARAMS);
  };

  /**
   * Pops the next argument from the argument list and checks that it matches
   * the requested type otherwise throws an exception. This function will return
   * true if there are more arguments in argsTail or false otherwise.
   *
   * `resultOut` and `argsTailOut` are still set to the correct value even when
   * a failed type check causes this function to throw.S
   *
   * \param args         Argument list to pop.
   * \param resultOut    Required. Receives the value of popped argument.
   * \param argsTailOut  Optional, receives the remainder of the popped
   *                     argument list.
   * \param expectedType The expected value type for the popped argument.
   * \returns The popped argument value.
   */
  bool tryPopArgument(
      Value args,
      Value* resultOut,
      Value* argsTailOut = nullptr,
      std::optional<ValueType> expectedType = std::nullopt);

  /**
   * Pops the next argument from the argument list and checks that it matches
   * the requested type otherwise throws an exception.
   *
   * \param args         Argument list to pop.
   * \param argsTailOut  Optional, receives the remainder of the popped
   *                     argument list.
   * \param expectedType The expected value type for the popped argument.
   * \returns The popped argument value.
   */
  Value popArgumentOrThrow( // TODO: Not sure how useful this method is...
      Value args,
      Value* argsTailOut = nullptr,
      std::optional<ValueType> expectedType = std::nullopt);

  /**
   * Tries to get the next argument from the argument list and optionally
   * fetches the remaining unread arguments.
   *
   * Type checking will be performed if the `expectedType` parameter is set to
   * any value other than nullopt. If the next argument does not the expected
   * type this function will still set `resultOut` and `argsTailOut` but it will
   * return `WrongArgValueType` error instead of `Ok`.
   *
   * \param args         Argument list to pop.
   * \param resultOut    Optional, receives the value of popped argument.
   * \param argsTailOut  Optional, receives the remainder of the popped
   *                     argument list.
   * \param expectedType The expected value type for the popped argument.
   * \returns True if there was an argument to pop, false otherwise.
   */
  PopArgResult popArgument(
      Value args,
      Value* resultOut,
      Value* argsTailOut,
      std::optional<ValueType> expectedType = std::nullopt);
} // namespace Shiny
