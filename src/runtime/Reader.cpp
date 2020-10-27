#include "runtime/Reader.h"

#include "runtime/CharacterStream.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"

#include <fmt/format.h>
#include <iostream> // temp

#include <array>
#include <cassert>
#include <string>
#include <tuple>

using namespace Shiny;

namespace {
  const char kLineCommentStartChar = ';';
  const size_t kDefaultTextBufferSize = 256;
} // namespace

//------------------------------------------------------------------------------
Reader::Reader(std::shared_ptr<VmState> vmState) : vmState_(vmState) {
  assert(vmState_ != nullptr);
  textBuffer_.reserve(kDefaultTextBufferSize);
}

//------------------------------------------------------------------------------
Value Reader::read(std::string_view buffer) {
  // TODO: Support floating point values.

  CharacterStream input{buffer};
  skipWhitespace(input);

  // First character (or first few) determine what sort of lexeme we are going
  // to read.
  if (input.peekIsMatch(0, '#')) {
    // First character is #. Read another character to determine if this is a
    // boolean (#t or #f), or a character (#\).
    if (input.peekIsMatch(1, 't') || input.peekIsMatch(1, 'f')) {
      return readBoolean(input);
    } else {
      return readCharacter(input);
    }
  } else if (input.peekIsMatch(0, '"')) {
    // This is a string as the first character is ". Add all following
    // characters to the string until we finding a terminating quote.
    return readString(input);
  } else if (input.peekIsMatch(0, '(')) { // TODO: readIfMatch('(')
    return readPair(input);
  } else if (
      input.peekIsDigit(0) ||
      (input.peekIsMatch(0, '-') && input.peekIsDigit(1))) {
    // This is a number because the first character is either a digit or starts
    // with a negative sign.
    return readFixnum(input);
  } else {
    // oops i didn't recogonize this!
    // TODO: this breaks badly for an empty/whitespace string. Fix!!
    throw ReaderUnexpectedCharException(
        input.position(), EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
Value Reader::readPair(CharacterStream& input) {
  auto lexemeStart = input.position();

  input.nextChar(); // TODO: readIfMatch('(')
  skipWhitespace(input);

  // TODO: read all pairs not just empty pair.
  if (input.peekIsMatch(0, ')')) {
    return vmState_->globals().emptyList;
  } else {
    throw ReaderException(
        "Did not read expected closing parentheses",
        lexemeStart,
        input.position(),
        EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
Value Reader::readBoolean(CharacterStream& input) {
  auto lexemeStart = input.position();

  // consume the leading '#' character.
  input.nextChar();

  // Read the next character to determine if it is a boolean or a character.
  Value result;

  if (!input.hasNext()) {
    throw ReaderException(
        "Unexpected end of stream when reading boolean",
        lexemeStart,
        input.position(),
        EXCEPTION_CALLSITE_ARGS);
  }

  switch (input.nextChar()) {
  case 't':
    result = vmState_->globals().bTrue;
    break;
  case 'f':
    result = vmState_->globals().bFalse;
    break;
  default:
    throw ReaderException(
        "Unexpected character following # when parsing for boolean or char",
        lexemeStart,
        input.position(),
        EXCEPTION_CALLSITE_ARGS);
  }

  // Character / boolean values lexemes must terminate with a separator
  // character.
  if (!peekIsDelimOrEnd(input, 0)) {
    throw ReaderExpectedDelimException(
        input.position(), EXCEPTION_CALLSITE_ARGS);
  }

  return result;
}

//------------------------------------------------------------------------------
Value Reader::readCharacter(CharacterStream& input) {
  auto lexemeStart = input.position();

  // Consume the leading '#' character.
  input.nextChar();

  // Next character must be \ otherwise this is not a valid character.
  if (!input.peekIsMatch(0, '\\')) {
    input.nextChar(); // TODO: peek + next => consumeIfMatches
    throw ReaderException(
        "Expected \\ to follow #",
        lexemeStart,
        input.position(),
        EXCEPTION_CALLSITE_ARGS);
  }

  input.nextChar();

  // Check for special characters next.
  // TODO: Make this a table driven solution.
  char c = 0;

  if (consumeIfMatches(input, SpecialChars::kAlarmName)) {
    c = SpecialChars::kAlarmValue;
  } else if (consumeIfMatches(input, SpecialChars::kBackspaceName)) {
    c = SpecialChars::kBackspaceValue;
  } else if (consumeIfMatches(input, SpecialChars::kDeleteName)) {
    c = SpecialChars::kDeleteValue;
  } else if (consumeIfMatches(input, SpecialChars::kEscapeName)) {
    c = SpecialChars::kEscapeValue;
  } else if (consumeIfMatches(input, SpecialChars::kNewlineName)) {
    c = SpecialChars::kNewlineValue;
  } else if (consumeIfMatches(input, SpecialChars::kNullName)) {
    c = SpecialChars::kNullValue;
  } else if (consumeIfMatches(input, SpecialChars::kReturnName)) {
    c = SpecialChars::kReturnValue;
  } else if (consumeIfMatches(input, SpecialChars::kSpaceName)) {
    c = SpecialChars::kSpaceValue;
  } else if (consumeIfMatches(input, SpecialChars::kTabName)) {
    c = SpecialChars::kTabValue;
  } else {
    // Default is just a regular single character value.
    // Make sure there is a letter to read.
    if (!input.hasNext()) {
      throw ReaderException(
          "Expected character letter but got end of file",
          lexemeStart,
          input.position(),
          EXCEPTION_CALLSITE_ARGS);
    }

    c = input.nextChar();

    // Check that this letter is printable.
    if (!::isalnum(c) && !::ispunct(c)) {
      throw ReaderException(
          "Character must be letter, digit or punctation",
          lexemeStart,
          input.position(),
          EXCEPTION_CALLSITE_ARGS);
    }
  }

  // Characters must be followed by a delimiter.
  if (!peekIsDelimOrEnd(input, 0)) {
    throw ReaderExpectedDelimException(
        input.position(), EXCEPTION_CALLSITE_ARGS);
  }

  return Value{c};
}

//------------------------------------------------------------------------------
Value Reader::readString(CharacterStream& input) {
  textBuffer_.clear();

  // Consume the leading quote.
  auto lexemeStart = input.position();
  input.nextChar();

  // Read characters into the temporary text buffer until a terminating double
  // quote is found.
  while (input.hasNext() && !input.peekIsMatch(0, '"')) {
    // If this is an escaped quote move past it otherwise keep scanning for the
    // terminating double quote.
    // TODO: Handle escaped characters.
    // TODO: Handle edge case of \ followed by EOF.
    if (input.peekIsMatch(0, '\\')) {
      input.nextChar(); // Discard \ .

      // Make sure there is a valid character following the start of the escape
      // sequence.
      if (!input.hasNext()) {
        throw ReaderException(
            "Unexpected end of stream when parsing escape sequence",
            lexemeStart,
            input.position(),
            EXCEPTION_CALLSITE_ARGS);
      }

      // Handle the escaped character or throw an exception if it isn't
      // supported.
      char n = input.nextChar();

      switch (n) {
      case '\\':
        textBuffer_.push_back('\\');
        break;
      case '"':
        textBuffer_.push_back('"');
        break;
      case 'n':
        textBuffer_.push_back('\n');
        break;
      default:
        throw ReaderException(
            "Unknown escape sequence",
            lexemeStart,
            input.position(),
            EXCEPTION_CALLSITE_ARGS);
      }
    } else {
      // Normal, non-escaped character.
      textBuffer_.push_back(input.nextChar());
    }
  }

  // Verify the string is terminated by a double quote.
  if (!input.peekIsMatch(0, '"')) {
    if (input.hasNext()) {
      input.nextChar();
    }

    throw ReaderException(
        "Terminating double quote missing at end of string",
        lexemeStart,
        input.position(),
        EXCEPTION_CALLSITE_ARGS);
  }

  return Value{vmState_->makeString(textBuffer_)};
}

//------------------------------------------------------------------------------
Value Reader::readFixnum(CharacterStream& input) {
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
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool Reader::peekIsDelimOrEnd(const CharacterStream& input, size_t offset) {
  if (input.tryPeekChar(offset, nullptr)) {
    return peekIsDelim(input, offset);
  }

  return true;
}

//------------------------------------------------------------------------------
bool Reader::consumeIfMatches(CharacterStream& input, std::string_view needle) {
  // If any character doesn't match then its false.
  for (size_t i = 0; i < needle.size(); ++i) {
    if (!input.peekIsMatch(i, needle[i])) {
      return false;
    }
  }

  // It matches! Advance the character stream to move past the needle.
  for (size_t i = 0; i < needle.size(); ++i) {
    input.nextChar();
  }

  return true;
}

//------------------------------------------------------------------------------
ReaderUnexpectedCharException::ReaderUnexpectedCharException(
    CharacterStreamPosition position,
    EXCEPTION_CALLSITE_PARAMS)
    : ReaderException(
          fmt::format(
              "Unrecogonized character at index {} when reading",
              position.offset),
          position,
          position,
          EXCEPTION_INIT_BASE_ARGS) {}

//------------------------------------------------------------------------------
ReaderExpectedDelimException::ReaderExpectedDelimException(
    CharacterStreamPosition position,
    EXCEPTION_CALLSITE_PARAMS)
    : ReaderException(
          fmt::format(
              "Expected delimiter character at index {} when reading",
              position.offset),
          position,
          position,
          EXCEPTION_INIT_BASE_ARGS) {}
