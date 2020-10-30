// Copyright (c) Scott MacDonald. All rights reserved.
#pragma once
#include "runtime/Exception.h"

#include <array>
#include <cassert>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Shiny {
  class Allocator;
  class VmState;

  struct RawString;
  struct RawPair;

  /** Type of value stored in a Value instance. */
  enum class ValueType {
    EmptyList,
    Boolean,
    Fixnum,
    Symbol,
    Character,
    String,
    Pair
  };

  /** Value type string names. */
  static constexpr const std::array<const char*, 7> ValueTypeNames = {
      "EmptyList",
      "Boolean",
      "Fixnum",
      "Symbol",
      "Character",
      "String",
      "Pair"};

  /** Fixnum type. */
  using fixnum_t = int;

  /** A dynamically typed value. */
  class Value {
  public:
    /** Default constructor creates an empty list value. */
    constexpr Value() noexcept : type_(ValueType::EmptyList), fixnum_value(0) {}

    /** Bool constructor. */
    explicit constexpr Value(bool value) noexcept
        : type_(ValueType::Boolean),
          bool_value(value) {}

    /** Fixnum constructor. */
    explicit constexpr Value(fixnum_t fixnum) noexcept
        : type_(ValueType::Fixnum),
          fixnum_value(fixnum) {}

    /** Character constructor. */
    explicit constexpr Value(char c) noexcept
        : type_(ValueType::Character),
          char_value(c) {}

    /** String constructor. */
    explicit Value(
        RawString* rawString,
        ValueType type = ValueType::String) noexcept
        : type_(type),
          string_ptr(rawString) {
      assert(rawString != nullptr);
      assert(type == ValueType::String || type == ValueType::Symbol);
    }

    /** Pair constructor. */
    explicit Value(RawPair* rawPair) noexcept
        : type_(ValueType::Pair),
          pair_ptr(rawPair) {
      assert(rawPair != nullptr);
    }

  public:
    /** Get the type for this value. */
    constexpr ValueType type() const noexcept { return type_; }

    /** Print value as a string. */
    std::string toString() const;

  public:
    /** Test if value is of type empty list. */
    constexpr bool isEmptyList() const noexcept {
      return type_ == ValueType::EmptyList;
    }

    /** Test if value is of type boolean. */
    constexpr bool isBoolean() const noexcept {
      return type_ == ValueType::Boolean;
    }

    /** Test if value is of type fixnum. */
    constexpr bool isFixnum() const noexcept {
      return type_ == ValueType::Fixnum;
    }

    /** Test if value is of type symbol. */
    constexpr bool isSymbol() const noexcept {
      return type_ == ValueType::Symbol;
    }

    /** Test if value is of type character. */
    constexpr bool isCharacter() const noexcept {
      return type_ == ValueType::Character;
    }

    /** Test if value is of type string. */
    constexpr bool isString() const noexcept {
      return type_ == ValueType::String;
    }

    /** Test if value is of type pair. */
    constexpr bool isPair() const noexcept { return type_ == ValueType::Pair; }

  public:
    /** Convert to fixnum integer value. Undefined behavior if not a fixnum. */
    constexpr fixnum_t toFixnum() const noexcept { return fixnum_value; }

    /** Convert to a boolean value. Undefined behavior if not a boolean. */
    constexpr bool toBool() const noexcept { return bool_value; }

    /** Convert to a boolean value. Undefined behavior if not a boolean. */
    constexpr char toChar() const noexcept { return char_value; }

    /**
     * Convert to a string_view. Undefined behavior if not a string or a symbol.
     */
    std::string_view toStringView() const;

    /** Convert to raw pair pointer. Undefined behavior if not a pair. */
    RawPair* toRawPair() const noexcept { return pair_ptr; }

  public:
    /** Test if value is false. */
    constexpr bool isFalse() const noexcept {
      return isBoolean() && !bool_value;
    }

    /** Test if value is true (Scheme true is any value that is not false). */
    constexpr bool isTrue() const noexcept { return !isFalse(); }

  public:
    static std::ostream& print(std::ostream& os, const Value& v);

  public:
    /** Value equality operator. */
    bool operator==(const Value& rhs) const noexcept {
      if (type_ != rhs.type_) {
        return false;
      } else {
        switch (type_) {
        case ValueType::EmptyList:
          return true;
        case ValueType::Boolean:
          return bool_value == rhs.bool_value;
        case ValueType::Fixnum:
          return fixnum_value == rhs.fixnum_value;
        case ValueType::Character:
          return char_value == rhs.char_value;
        case ValueType::Symbol:
        case ValueType::String:
          return string_ptr == rhs.string_ptr;
        default:
          assert(false && "Value == operator missing support type");
          return false;
        }
      }
    }

    /** Inequality operator. */
    bool operator!=(const Value& rhs) const noexcept { return !(*this == rhs); }

  private:
    ValueType type_ = ValueType::EmptyList;
    union {
      fixnum_t fixnum_value;
      bool bool_value;
      char char_value;
      RawString* string_ptr;
      RawPair* pair_ptr;
    };
  };

  std::ostream& operator<<(std::ostream& os, const Value& v);

  /** Pair type. */
  struct RawPair {
    Value car;
    Value cdr;
  };

  /** Special character definitions. */
  namespace SpecialChars {
    const std::string kAlarmName = "alarm";
    const char kAlarmValue = 7;
    const std::string kBackspaceName = "backspace";
    const char kBackspaceValue = 8;
    const std::string kDeleteName = "delete";
    const char kDeleteValue = 127;
    const std::string kEscapeName = "escape";
    const char kEscapeValue = 27;
    const std::string kNewlineName = "newline";
    const char kNewlineValue = 10; // 0x0a linefeed (\n).
    const std::string kNullName = "null";
    const char kNullValue = 0;
    const std::string kReturnName = "return";
    const char kReturnValue = 13; // 0x0d carriage return (\r).
    const std::string kSpaceName = "space";
    const char kSpaceValue = 32;
    const std::string kTabName = "tab";
    const char kTabValue = 9;
  } // namespace SpecialChars

  /** Cast exception - expected type does not match actual type. */
  class WrongValueTypeException : public Exception {
  public:
    WrongValueTypeException(
        ValueType expectedType,
        ValueType actualType,
        EXCEPTION_CALLSITE_PARAMS);
  };

  // ------------------------------------------------------------------------ //
  // C style API for dealing with values.
  // TODO: Change from exceptions to something C compatible (error type?).
  // TODO: Finish C API, add tests.
  // TODO: Move to RuntimeApi.h

  /** Get string name for value type. */
  std::string_view to_string(ValueType valueType) noexcept;

  /** Create a new pair value. */
  Value cons(VmState* vm, Value car, Value cdr);

  /** Get the car of a pair (or throw an exception if value is not a pair). */
  Value car(Value pair);

  /** Set the car of a pair (or throw an exception if value is not a pair). */
  void set_car(Value pair, Value v);

  /** Get the cdr of a pair (or throw an exception if value is not a pair). */
  Value cdr(Value pair);

  /** Set the cdr of a pair (or throw an exception if value is not a pair). */
  void set_cdr(Value pair, Value v);

} // namespace Shiny