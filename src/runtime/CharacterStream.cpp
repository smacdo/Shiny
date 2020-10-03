#include "runtime/CharacterStream.h"

#include <cassert>
#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
bool CharacterStream::hasNext() const noexcept { return pos_ < buffer_.size(); }

//------------------------------------------------------------------------------
char CharacterStream::nextChar() {
  // Make sure the next character exists (not of bounds).
  if (pos_ < buffer_.size()) {
    // If this character and the next form a windows line ending (\r\n), then
    // skip this character and advance to the newline.
    if (buffer_[pos_] == '\r' && (pos_ + 1) < buffer_.size() &&
        buffer_[pos_ + 1] == '\n') {
      pos_++;
    }

    // If the character about to be returned is a newline character, move to the
    // next line. Otherwise just move to the next column.
    //
    // Additionally if this is the last character in the stream set both the
    // line and column indicator to -1 (invalid).
    if (pos_ + 1 < buffer_.size()) {
      if (buffer_[pos_] == '\n') {
        col_ = 1;
        lineNumber_++;
      } else {
        col_++;
      }
    } else {
      // TODO: This behavior is not consistent since it only happens in here.
      // Other functions like skip* don't do this. Make the behavior uniform.
      col_ = -1;
      lineNumber_ = -1;
    }

    // Return the current character and advance the internal position to the
    // next unread character.
    return buffer_[pos_++];
  } else {
    throw CharacterStreamEndOfStreamException(
        buffer_.size(), EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
char CharacterStream::peekChar(size_t offset) const {
  char c;

  // Make sure the requested character offset is valid.
  if (tryPeekChar(offset, &c)) {
    return c;
  } else {
    throw CharacterStreamEndOfStreamException(
        buffer_.size(), EXCEPTION_CALLSITE_ARGS);
  }
}

//------------------------------------------------------------------------------
bool CharacterStream::tryPeekChar(size_t offset, char* result) const noexcept {
  // Make sure the requested character offset is valid.
  if (pos_ + offset >= buffer_.size()) {
    return false;
  }

  // Only copy character is caller gave us a valid pointer.
  if (result != nullptr) {
    *result = buffer_[pos_ + offset];
  }

  return true;
}

//------------------------------------------------------------------------------
bool CharacterStream::peekIsMatch(size_t offset, char expected) const noexcept {
  char c;

  if (tryPeekChar(offset, &c)) {
    return expected == c;
  }

  return false;
}

//------------------------------------------------------------------------------
bool CharacterStream::peekIsDigit(size_t offset) const noexcept {
  char c;

  if (tryPeekChar(offset, &c)) {
    if (c >= '0' && c <= '9') {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
bool CharacterStream::peekIsWhitespace(size_t offset) const noexcept {
  char c;

  if (tryPeekChar(offset, &c)) {
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
size_t CharacterStream::skipWhitespace() noexcept {
  // Advance current position until a non-whitespace character is found.
  size_t whitespaceCount = 0;

  while (pos_ < buffer_.size() && peekIsWhitespace(0)) {
    whitespaceCount++;
    pos_++;
  }

  return whitespaceCount;
}

//------------------------------------------------------------------------------
size_t CharacterStream::skipToNextLine() noexcept {
  // Advance one character past the next newline (\n), or until the end of the
  // stream is found.
  size_t charSkipCount = 0;

  while (pos_ < buffer_.size() && !peekIsMatch(0, '\n')) {
    charSkipCount++;
    pos_++;
  }

  // If a newline was found (not end of stream) then move one character more to
  // skip the newline (since we want the first character following the newline).
  if (pos_ < buffer_.size()) {
    assert(buffer_[pos_] == '\n');
    charSkipCount++;
    pos_++;
  }

  return charSkipCount;
}

//------------------------------------------------------------------------------
CharacterStreamEndOfStreamException::CharacterStreamEndOfStreamException(
    size_t bufferSize,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(
              "Cannot read beyond end of character stream (size {})",
              bufferSize),
          EXCEPTION_INIT_BASE_ARGS) {}
