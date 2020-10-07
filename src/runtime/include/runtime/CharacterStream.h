#pragma once
#include "runtime/Exception.h"
#include <string_view>

namespace Shiny {
  struct CharacterStreamPosition {
    size_t offset;
    int col;
    int lineNumber;
  };

  /** Represents an array of characters as a stream. */
  class CharacterStream {
  public:
    /** Constructor. */
    CharacterStream(std::string_view buffer) : buffer_(buffer) {}

    /** Get the column number. */
    int col() const noexcept { return col_; }

    /** Get the line number. */
    int lineNumber() const noexcept { return lineNumber_; }

    /** Get character index. */
    CharacterStreamPosition position() const noexcept {
      return {pos_, col_, lineNumber_};
    }

    /** Get if there are more characters left in the stream. */
    bool hasNext() const noexcept;

  public:
    /** Peek at current or following characters without advancing stream. */
    char peekChar(size_t offset) const;

    /**
     * Try to peek at current or following characters without advancing the
     * stream or throwing an exception.
     */
    bool tryPeekChar(size_t offset, char* result) const noexcept;

    /** Peek to see if character matches the given character. */
    bool peekIsMatch(size_t offset, char expected) const noexcept;

    /** Peek to see if character is a digit (0-9). */
    bool peekIsDigit(size_t offset) const noexcept;

    /** Peek to see if character is whitespace. */
    bool peekIsWhitespace(size_t offset) const noexcept;

  public:
    /** Gets the next character. */
    char nextChar();

    /** Advance to next character that is not whitespace. */
    size_t skipWhitespace() noexcept;

    /** Advance to first character on the next line. */
    size_t skipToNextLine() noexcept;

  private:
    std::string_view buffer_;
    size_t pos_ = 0;
    int col_ = 1;
    int lineNumber_ = 1;
  };

  class CharacterStreamEndOfStreamException : public Exception {
  public:
    CharacterStreamEndOfStreamException(
        size_t bufferSize,
        EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny