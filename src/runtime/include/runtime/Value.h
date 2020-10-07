// Copyright (c) Scott MacDonald. All rights reserved.
#pragma once
#include "runtime/Exception.h"

#include <cassert>
#include <iosfwd>
#include <stdexcept>
#include <string>

namespace Shiny {
  /** Type of value stored in a Value instance. */
  enum class ValueType { Null, Boolean, Fixnum };

  /** Fixnum type. */
  using fixnum_t = int;

  /** A dynamically typed value. */
  class Value {
  public:
    /** Default constructor creates a null value. */
    constexpr Value() noexcept : type_(ValueType::Null), fixnum_value(0) {}

    /** Bool constructor. */
    explicit constexpr Value(bool value) noexcept
        : type_(ValueType::Boolean),
          bool_value(value) {}

    /** Fixnum constructor. */
    explicit constexpr Value(fixnum_t fixnum) noexcept
        : type_(ValueType::Fixnum),
          fixnum_value(fixnum) {}

    /** Get the type for this value. */
    constexpr ValueType type() const noexcept { return type_; }

    /** Print value as a string. */
    std::string toString() const;

  public:
    /** Test if value is of type null. */
    constexpr bool isNull() const noexcept { return type_ == ValueType::Null; }

    /** Test if value is of type boolean. */
    constexpr bool isBoolean() const noexcept {
      return type_ == ValueType::Boolean;
    }

    /** Test if value is of type fixnum. */
    constexpr bool isFixnum() const noexcept {
      return type_ == ValueType::Fixnum;
    }

  public:
    /** Convert to fixnum integer value. Undefined behavior if not a fixnum. */
    constexpr fixnum_t toFixnum() const noexcept { return fixnum_value; }

    /** Convert to a boolean value. Undefined behavior if not a boolean. */
    constexpr bool toBool() const noexcept { return bool_value; }

  public:
    /** Value equality operator. */
    constexpr bool operator==(const Value& rhs) const noexcept {
      if (type_ != rhs.type_) {
        return false;
      } else {
        switch (type_) {
        case ValueType::Null:
          return true;
        case ValueType::Boolean:
          return bool_value == rhs.bool_value;
        case ValueType::Fixnum:
          return fixnum_value == rhs.fixnum_value;
        default:
          assert(false && "Value == operator missing support type");
          return false;
        }
      }
    }

    /** Inequality operator. */
    constexpr bool operator!=(const Value& rhs) const noexcept {
      return !(*this == rhs);
    }

  private:
    ValueType type_ = ValueType::Null;
    union {
      fixnum_t fixnum_value;
      bool bool_value;
    };
  };

  std::ostream& operator<<(std::ostream& os, const Value& v);

} // namespace Shiny