#pragma once

namespace Shiny {
  /**
   * Pops the next argument from the argument list or throws an exception.
   *
   * \param args        Argument list to pop.
   * \param nextArgsOut Optional, receives the remainder of the popped
   *                    argument list.
   * \returns The popped argument value.
   */
  Value popArgument(Value args, Value* nextArgsOut);

  /**
   * Pops the next argument from the argument list and checks that it matches
   * the requested type otherwise throws an exception.
   *
   * \param args         Argument list to pop.
   * \param expectedType The expected value type for the popped argument.
   * \param nextArgsOut  Optional, receives the remainder of the popped
   *                     argument list.
   * \returns The popped argument value.
   */
  Value popArgument(Value args, ValueType expectedType, Value* nextArgsOut);

  /**
   * Tries to get the next argument from the argument list and optionally
   * calculates the remaining unread arguments.
   *
   * \param args        Argument list to pop.
   * \param resultOut   Optional, receives the value of popped argument.
   * \param nextArgsOut Optional, receives the remainder of the popped
   *                    argument list.
   * \returns True if there was an argument to pop, false otherwise.
   */
  bool tryPopArgument(Value args, Value* resultOut, Value* nextArgsOut);

  /**
   * Tries to get the next argument from the argument list and optionally
   * calculates the remaining unread arguments.
   *
   * If the popped argument value does not match the given value type an
   * exception will be thrown.
   *
   * \param args         Argument list to pop.
   * \param expectedType The expected value type for the popped argument.
   * \param resultOut    Optional, receives the value of popped argument.
   * \param nextArgsOut  Optional, receives the remainder of the popped
   *                     argument list.
   * \returns True if there was an argument to pop, false otherwise.
   */
  bool tryPopArgument(
      Value args,
      ValueType expectedType,
      Value* resultOut,
      Value* nextArgsOut);

  namespace SpecialForms {
    constexpr const char* kDefine = "define";
    constexpr const char* kIf = "if";
    constexpr const char* kQuote = "quote";
    constexpr const char* kSet = "set!";
  } // namespace SpecialForms
} // namespace Shiny
