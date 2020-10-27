#pragma once
#include <cstddef>
#include <memory>

namespace Shiny {
  class Allocator;

  /** Stores a text length and character buffer together. */
  struct RawString {
    using char_type = char;

    size_t length;
    char data[1];
  };

  /** Delete functor for RawString. */
  struct RawStringDeleter {
    RawStringDeleter(Allocator* allocator);
    void operator()(RawString* rawString) const;

  private:
    Allocator* allocator_;
  };

  /** Allocate a new raw string. */
  RawString* create_string(Allocator* allocator, std::string_view text);

  /** Destroy a raw string. */
  void destroy_string(Allocator* allocator, RawString* rawString);

  /** Get string length. */
  size_t string_length(const RawString* rawString);

  /** Get string contents. May not be null terminated! */
  char* to_data(RawString* rawString);

  /** Get string contents. May not be null terminted! */
  const char* to_data(const RawString* rawString);

  /** Get string contents as a string_view. */
  std::string_view to_stringView(const RawString* rawString);

  /** Test if two strings are equal in value. */
  bool string_equal(const RawString* s1, const RawString* s2);
} // namespace Shiny