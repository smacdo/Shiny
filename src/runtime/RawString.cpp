#include "runtime/RawString.h"
#include "runtime/Allocator.h"

#include <array>
#include <cassert>
#include <cstring>

using namespace Shiny;

//------------------------------------------------------------------------------
RawStringDeleter::RawStringDeleter(Allocator* allocator)
    : allocator_(allocator) {}

//------------------------------------------------------------------------------
void RawStringDeleter::operator()(RawString* rawString) const {
  if (allocator_ != nullptr) {
    destroy_string(allocator_, rawString);
  }
}

//------------------------------------------------------------------------------
RawString* Shiny::create_string(Allocator* allocator, std::string_view text) {
  assert(allocator != nullptr);

  // Optimization - for strings that are either empty (size 0) or a single
  // chacter use the same statically allocated raw string instance.
  static RawString kEmptyString = {0, {'\0'}};
  static std::array<RawString, 256> kSingleCharStrings;

  if (text.length() == 0) {
    return &kEmptyString;
  } else if (text.length() == 1) {
    size_t index = static_cast<size_t>(text[0]);
    assert(index < 256);

    kSingleCharStrings[index].length = 1;
    kSingleCharStrings[index].data[0] = text[0];

    return &kSingleCharStrings[index];
  }

  // The string length is stored adjacent to the text characters, and to
  // accomodate this we use a variable length buffer array. C++ does not
  // support zero sized arrays so we resort to always storing at least one
  // character (even if its an empty string).
  //
  //  *****
  // [SSSS0...N]
  //   |  ||  \- final character in text (not always null terminator!)
  //   |  |\---- additional characters after the first character.
  //   |  \----- first character of text.
  //   \-------- length of text.
  auto rawLength = std::max(static_cast<size_t>(1), text.length());
  auto extraCharBytes = (rawLength - 1) * sizeof(RawString::char_type);
  auto rawBuffer = allocator->allocate(sizeof(RawString) + extraCharBytes);

  auto rawString = reinterpret_cast<RawString*>(rawBuffer);

  memmove(rawString->data, text.data(), text.length() * sizeof(char));
  rawString->length = text.length();

  return rawString;
}

//------------------------------------------------------------------------------
void Shiny::destroy_string(Allocator* allocator, RawString* rawString) {
  if (rawString != nullptr && rawString->length > 1) {
    assert(allocator != nullptr);
    allocator->destroy(rawString);
  }
}

//------------------------------------------------------------------------------
size_t Shiny::string_length(const RawString* rawString) {
  assert(rawString != nullptr);
  return rawString->length;
}

//------------------------------------------------------------------------------
char* Shiny::to_data(RawString* rawString) {
  assert(rawString != nullptr);
  return rawString->data;
}

//------------------------------------------------------------------------------
const char* Shiny::to_data(const RawString* rawString) {
  assert(rawString != nullptr);
  return rawString->data;
}

//------------------------------------------------------------------------------
std::string_view Shiny::to_stringView(const RawString* rawString) {
  return {to_data(rawString), string_length(rawString)};
}

//------------------------------------------------------------------------------
bool Shiny::string_equal(const RawString* s1, const RawString* s2) {
  assert(s1 != nullptr);
  assert(s2 != nullptr);

  if (s1->length != s2->length) {
    return false;
  } else if (s1 == s2) {
    return true;
  } else {
    return 0 == strncmp(to_data(s1), to_data(s2), string_length(s1));
  }
}