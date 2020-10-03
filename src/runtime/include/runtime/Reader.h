#pragma once
#include "runtime/Exception.h"
#include "runtime/Value.h"
#include <string_view>

namespace Shiny {
  class CharacterStream;

  /** Reads s-expressions an
   * d converts to abstract syntax tree. */
  class Reader {
  public:
    /** Convert s-expression input to abstract syntax tree. */
    Shiny::Value read(std::string_view input);

  private:
    /** Advance past whitespace including comments. */
    static void skipWhitespace(CharacterStream& input);

    /** Peek if character is a delimitter. */
    static bool peekIsDelim(const CharacterStream& input, size_t offset);

    /** Peek if character is a delimitter or end of stream. */
    static bool peekIsDelimOrEnd(const CharacterStream& input, size_t offset);

  private:
  };

  // TODO: Add a base Reader exception type that can capture more token
  // information (start, stop) to enable highlight errors.
  class ReaderUnexpectedCharException : public Exception {
  public:
    ReaderUnexpectedCharException(
        char c,
        size_t index,
        EXCEPTION_CALLSITE_PARAMS);
  };

  class ReaderExpectedDelimException : public Exception {
  public:
    ReaderExpectedDelimException(size_t index, EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny
