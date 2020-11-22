#pragma once

#include "Value.h"

#include <optional>

namespace Shiny {
  /** Result of trying to pop the next argument from an argument list. */
  enum class PopArgResult {
    Ok,               ///< Argument was succesfully popped.
    EmptyArgList,     ///< Argument list was empty.
    WrongArgValueType ///< Popped argument type did not match expected.
  };

  /** List of evaluated arguments passed to called procedure. */
  struct ArgList {
    ArgList();
    ArgList(Value args);

    Value next;      ///< Next unpopped argument in list.
    size_t popCount; ///< Number of arguments popped - modified by RuntimeAPI.
  };

  /** Argument value type does not match expected type. */
  class WrongArgTypeException : public Exception {
  public:
    WrongArgTypeException(
        size_t argumentIndex,
        ValueType expectedType,
        ValueType actualType,
        EXCEPTION_CALLSITE_PARAMS);
  };

  /** Expected argument but argument list was empty. */
  class ArgumentMissingException : public Exception {
  public:
    ArgumentMissingException(size_t argumentIndex, EXCEPTION_CALLSITE_PARAMS);
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
   * \param argOut       Receives the value of the popped argument.
   * \param expectedType The expected value type for the popped argument.
   * \returns The popped argument value.
   */
  bool tryPopArgument(
      ArgList& args,
      Value* argOut,
      std::optional<ValueType> expectedType = std::nullopt);

  /**
   * Pops the next argument from the argument list and checks that it matches
   * the requested type otherwise throws an exception.
   *
   * \param args         Argument list to pop.
   * \param expectedType The expected value type for the popped argument.
   * \returns The popped argument value.
   */
  Value popArgumentOrThrow( // TODO: Not sure how useful this method is...
      ArgList& args,
      std::optional<ValueType> expectedType = std::nullopt);

  /**
   * Tries to get the next argument from the argument list and optionally
   * fetches the remaining unread arguments.
   *
   * Type checking will be performed if the `expectedType` parameter is set to
   * any value other than nullopt. If the next argument does not the expected
   * type this function will still move the arglist to the next argument.
   *
   * \param args         Argument list to pop.
   * \param argOut       Optional. Receives the value of the popped argument.
   * \param expectedType The expected value type for the popped argument.
   * \returns Status of the pop action.
   */
  PopArgResult popArgument(
      ArgList& args,
      Value* argOut,
      std::optional<ValueType> expectedType = std::nullopt);
} // namespace Shiny
