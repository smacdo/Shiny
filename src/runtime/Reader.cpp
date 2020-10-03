#include "runtime/Reader.h"
#include "runtime/CharacterStream.h"
#include "runtime/Value.h"

#include <fmt/format.h>
#include <string>

using namespace Shiny;

namespace {
  const char kLineCommentStartChar = ';';
}

//--------------------------------------------------------------------------------------------------
Value Reader::read(std::string_view buffer) {
  // TODO: Support floating point values.

  CharacterStream input{buffer};
  skipWhitespace(input);

  // Is the next character the start of a number?
  if (input.peekIsDigit(0) ||
      (input.peekIsMatch(0, '-') && input.peekIsDigit(1))) {
    // Is this a negative number? Eat the leading negative sign if thats the
    // case.
    bool isNegative = input.peekIsMatch(0, '-');

    if (isNegative) {
      input.nextChar();
    }

    // Read all the digits and convert to an integer.
    // TODO: Handle integer overflow when negative.
    long long rawNumber = 0;

    while (input.peekIsDigit(0)) {
      char nextDigit = input.nextChar();
      rawNumber = (rawNumber * 10) + (nextDigit - '0');
    }

    // Apply negative sign (if it was part of the number).
    rawNumber *= (isNegative ? -1 : 1);

    // Numbers must be followed by a separator.
    if (!peekIsDelimOrEnd(input, 0)) {
      throw ReaderExpectedDelimException(
          input.position(), EXCEPTION_CALLSITE_ARGS);
    }

    // TODO: Warn if value too large to store.
    return Value{static_cast<fixnum_t>(rawNumber)};
  } else {
    // oops i didn't recogonize this!
    // TODO: this breaks badly for an empty/whitespace string. Fix!!
    throw ReaderUnexpectedCharException(
        input.peekChar(0), input.position(), EXCEPTION_CALLSITE_ARGS);
  }
}

//--------------------------------------------------------------------------------------------------
void Reader::skipWhitespace(CharacterStream& input) {
  size_t charSkipped = 0;

  do {
    // Skip leading whitespace on line.
    charSkipped = input.skipWhitespace();

    // Check if next character is a comment, and if so then skip the rest of the
    // line.
    if (input.peekIsMatch(0, kLineCommentStartChar)) {
      charSkipped += input.skipToNextLine();
    }
  } while (charSkipped > 0);
}

//--------------------------------------------------------------------------------------------------
bool Reader::peekIsDelim(const CharacterStream& input, size_t offset) {
  if (input.peekIsWhitespace(offset)) {
    char c;

    if (input.tryPeekChar(offset, &c)) {
      if (c == '(' || c == ')' || c == '"' || c == ';' || c == '[' ||
          c == ']') {
        return true;
      }
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------------------
bool Reader::peekIsDelimOrEnd(const CharacterStream& input, size_t offset) {
  if (input.tryPeekChar(offset, nullptr)) {
    return peekIsDelim(input, offset);
  }

  return true;
}

//------------------------------------------------------------------------------
ReaderUnexpectedCharException::ReaderUnexpectedCharException(
    char c,
    size_t index,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Unrecogonized character '{}' at index {} when reading",
              c,
              index),
          EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
ReaderExpectedDelimException::ReaderExpectedDelimException(
    size_t index,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Expected delimiter character at index {} when reading",
              index),
          EXCEPTION_INIT_BASE_ARGS) {}
