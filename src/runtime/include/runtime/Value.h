// Copyright (c) Scott MacDonald. All rights reserved.
#pragma once

#include <iosfwd>
#include <stdexcept>
#include <string>

namespace Shiny {
  /** Value type contained in Value. */
  enum class ValueType { None, Fixnum };

  /** Fixnum type. */
  using fixnum_t = int;

  /** A dynamically typed value. */
  class Value {
  public:
    /** Fixnum constructor. */
    explicit Value(fixnum_t fixnum)
        : type_(ValueType::Fixnum),
          fixnum_value(fixnum) {}

    /** Get the type for this value. */
    ValueType type() const noexcept { return type_; }

    /** Print value as a string. */
    std::string toString() const;

  public:
    bool isFixnum() const noexcept { return type_ == ValueType::Fixnum; }

  public:
    fixnum_t toFixnum() const {
      if (type_ == ValueType::Fixnum) {
        return fixnum_value;
      } else {
        // TODO: Better exception.
        throw std::runtime_error("not a fixnum");
      }
    }

  public:
    bool operator==(const Value& rhs) const {
      if (type_ != rhs.type_) {
        return false;
      } else {
        switch (type_) {
        case ValueType::Fixnum:
          return fixnum_value == rhs.fixnum_value;
        default:
          // TODO: better exception.
          throw std::runtime_error("need to implement == for type");
        }
      }
    }

  private:
    ValueType type_ = ValueType::None;
    union {
      fixnum_t fixnum_value;
    };
  };

  std::ostream& operator<<(std::ostream& os, const Value& v);

} // namespace Shiny