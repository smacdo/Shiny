#pragma once
#include "runtime/CharacterStream.h"
#include "runtime/Exception.h"
#include "runtime/Value.h"
#include <string_view>

namespace Shiny {
  /** Reads s-expressions an
   * d converts to abstract syntax tree. */
  class Reader {
  public:
    /** Convert s-expression input to abstract syntax tree. */
    Shiny::Value read(std::string_view input);

  private:
    Value readFixnum(CharacterStream& input);
    Value readBoolean(CharacterStream& input);
    Value readCharacter(CharacterStream& input);

    /** Advance past whitespace including comments. */
    static void skipWhitespace(CharacterStream& input);

    /** Peek if character is a delimitter. */
    static bool peekIsDelim(const CharacterStream& input, size_t offset);

    /** Peek if character is a delimitter or end of stream. */
    static bool peekIsDelimOrEnd(const CharacterStream& input, size_t offset);

    /** Peek and advance if next characters match. */
    static bool
    consumeIfMatches(CharacterStream& input, std::string_view needle);

  private:
  };

  /**
   * Holds information about input text region that may have raised the error.
   */
  class ReaderException : public Exception {
  public:
    ReaderException(
        std::string message,
        const CharacterStreamPosition& startAt,
        const CharacterStreamPosition& endAt,
        EXCEPTION_CALLSITE_PARAMS)
        : Exception(std::move(message), EXCEPTION_INIT_BASE_ARGS),
          startAt_(startAt),
          endAt_(endAt) {}

    const CharacterStreamPosition& start() const noexcept { return startAt_; }
    const CharacterStreamPosition& end() const noexcept { return endAt_; }

  private:
    CharacterStreamPosition startAt_;
    CharacterStreamPosition endAt_;
  };

  class ReaderUnexpectedCharException : public ReaderException {
  public:
    ReaderUnexpectedCharException(
        CharacterStreamPosition position,
        EXCEPTION_CALLSITE_PARAMS);
  };

  class ReaderExpectedDelimException : public ReaderException {
  public:
    ReaderExpectedDelimException(
        CharacterStreamPosition position,
        EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny
