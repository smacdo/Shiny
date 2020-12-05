// Copyright (c) Scott MacDonald. All rights reserved.
#pragma once
#include "runtime/Exception.h"

#include <array>
#include <cassert>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <string_view>

// TODO: Support marking certain types as "const" (like string, pair) so that
//       const methods can throw an exception when the underlying type is const.
//       Rationale for this is to prevent procedures from accidently modifying
//       the AST with recycle optimization.

namespace Shiny {
  class Allocator;
  class EnvironmentFrame;
  class Value;
  class VmState;

  struct ArgList;
  struct CompoundProcedure;
  struct RawString;
  struct RawPair;

  /** Type of value stored in a Value. */
  enum class ValueType {
    EmptyList,          // 0
    Boolean,            // 1
    Fixnum,             // 2
    Symbol,             // 3
    Character,          // 4
    String,             // 5
    Pair,               // 6
    CompoundProcedure,  // 7
    PrimitiveProcedure, // 8
  };

  /** Table of names for ValueType. */
  static constexpr const std::array<const char*, 9> ValueTypeNames = {
      "EmptyList",
      "Boolean",
      "Fixnum",
      "Symbol",
      "Character",
      "String",
      "Pair",
      "CompoundProcedure",
      "PrimitiveProcedure"};

  /** Fixnum native type. */
  using fixnum_t = int;

  /** Floating point native type. */
  using flonum_t = double;

  /** Native procedure type. */
  using procedure_t =
      Value (*)(ArgList& args, VmState& vmState, EnvironmentFrame* env);

  /** A dynamically typed value. */
  class Value {
  public:
    /** Initialize as EmptyList. */
    constexpr Value() noexcept : type_(ValueType::EmptyList), fixnum_value(0) {}

    /** Initialize with a boolean value. */
    explicit constexpr Value(bool value) noexcept
        : type_(ValueType::Boolean),
          bool_value(value) {}

    /** Initialize with a character value. */
    explicit constexpr Value(char c) noexcept
        : type_(ValueType::Character),
          char_value(c) {}

    /** Initialize with a fixnum value. */
    explicit constexpr Value(fixnum_t fixnum) noexcept
        : type_(ValueType::Fixnum),
          fixnum_value(fixnum) {}

    /**
     * Initialize with a string or as symbol value.
     *
     * \param rawString A garbage collected (or otherwise externally tracked)
     *                  string pointer.
     * \param type      Must be either ValueType::String or ValueType::Symbol.
     */
    explicit Value(
        RawString* rawString,
        ValueType type = ValueType::String) noexcept
        : type_(type),
          string_ptr(rawString) {
      assert(rawString != nullptr);
      assert(type == ValueType::String || type == ValueType::Symbol);
    }

    /**
     * Initialize with a pair.
     *
     * \param rawPair A garbage collected (or otherwise externally tracked) pair
     *                pointer.
     */
    explicit Value(RawPair* rawPair) noexcept
        : type_(ValueType::Pair),
          pair_ptr(rawPair) {
      assert(rawPair != nullptr);
    }

    /** Initialize with a compound procedure. */
    explicit constexpr Value(CompoundProcedure* compoundProcedure)
        : type_(ValueType::CompoundProcedure),
          compoundProcedure_ptr(compoundProcedure) {
      assert(compoundProcedure != nullptr);
    }

    /** Initialize with a pointer to a native function. */
    explicit constexpr Value(procedure_t function) noexcept
        : type_(ValueType::PrimitiveProcedure),
          procedure_ptr(function) {
      assert(procedure_ptr != nullptr);
    }

  public:
    /** Get this value's type. */
    constexpr ValueType type() const noexcept { return type_; }

    /** Get value as a printable string. */
    std::string toString() const;

  public:
    /** Test if this value is an empty list. */
    constexpr bool isEmptyList() const noexcept {
      return type_ == ValueType::EmptyList;
    }

    /** Test if this value is a boolean. */
    constexpr bool isBoolean() const noexcept {
      return type_ == ValueType::Boolean;
    }

    /** Test if this value is a fixnum. */
    constexpr bool isFixnum() const noexcept {
      return type_ == ValueType::Fixnum;
    }

    /** Test if this value is a symbol. */
    constexpr bool isSymbol() const noexcept {
      return type_ == ValueType::Symbol;
    }

    /** Test if this value is a character. */
    constexpr bool isCharacter() const noexcept {
      return type_ == ValueType::Character;
    }

    /** Test if this value is a string. */
    constexpr bool isString() const noexcept {
      return type_ == ValueType::String;
    }

    /** Test if this value is a pair. */
    constexpr bool isPair() const noexcept { return type_ == ValueType::Pair; }

    /** Test if this value is a compound procedure. */
    constexpr bool isCompoundProcedure() const noexcept {
      return type_ == ValueType::CompoundProcedure;
    }

    /** Test if this value is a primitive procedure. */
    constexpr bool isPrimitiveProcedure() const noexcept {
      return type_ == ValueType::PrimitiveProcedure;
    }

    /** Test if this value is considered false. */
    constexpr bool isFalse() const noexcept {
      return isBoolean() && !bool_value;
    }

    /** Test if this value is a considered true. */
    constexpr bool isTrue() const noexcept { return !isFalse(); }

  public:
    /**
     * Returns this value as an integer.
     * This method has undefined behavior if it is not a fixnum type.
     */
    constexpr fixnum_t toFixnum() const noexcept { return fixnum_value; }

    /**
     * Returns this value as a bool.
     * This method has undefined behavior if it is not a bool type.
     */
    constexpr bool toBool() const noexcept { return bool_value; }

    /**
     * Returns this value as a character.
     * This method has undefined behavior if it is not a character type.
     */
    constexpr char toChar() const noexcept { return char_value; }

    /**
     * Returns this value as a string_view.
     * This method has undefined behavior if it is not convertible to a string
     * type (e.g., strings, symbols).
     */
    std::string_view toStringView() const;

    /**
     * Returns this value as a raw pair.
     * This method has undefined behavior if it is not a pair type.
     */
    RawPair* toRawPair() noexcept { return pair_ptr; }

    /**
     * Returns this value as a raw pair.
     * This method has undefined behavior if it is not a pair type.
     */
    const RawPair* toRawPair() const noexcept { return pair_ptr; }

    /**
     * Returns this value as pointer to a compound procedure.
     * This method has undefined behavior if it is not a compound procedure
     * type.
     */
    CompoundProcedure* toCompoundProcedure() noexcept {
      return compoundProcedure_ptr;
    }

    /**
     * Returns this value as pointer to a compound procedure.
     * This method has undefined behavior if it is not a compound procedure
     * type.
     */
    const CompoundProcedure* toCompoundProcedure() const noexcept {
      return compoundProcedure_ptr;
    }

    /**
     * Returns this value as a primitive procedure pointer.
     * This method has undefined behavior if it is not a primitive procedure
     * type.
     */
    procedure_t toPrimitiveProcedure() const noexcept { return procedure_ptr; }

  public:
    /**
     * Equality operator.
     * Follows the equality rules set forth in the Scheme R7RS standard for eq?,
     * and are reproduced here as follows:
     *
     * 1. Values are never equal if their types are not equal.
     * 2. Numbers, characters and bools are equal if their values are equal.
     * 3. EmptyList is equal to any other EmptyList.
     * 4. Pairs, Strings, Symbols and PrimitiveProcedures are equal only if the
     *    underlying pointer used to initialize the Value are identical.
     */
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
          case ValueType::CompoundProcedure:
            return compoundProcedure_ptr == rhs.compoundProcedure_ptr;
          case ValueType::PrimitiveProcedure:
            return procedure_ptr == rhs.procedure_ptr;
          case ValueType::Pair:
            return pair_ptr == rhs.pair_ptr;
          default:
            assert(false && "Value == operator missing support type");
            return false;
        }
      }
    }

    /** Inequality operator. */
    bool operator!=(const Value& rhs) const noexcept { return !(*this == rhs); }

  public:
    /** Read only field to get the default EmptyList value. */
    static const Value EmptyList;

    /** Read only field to get a True value. */
    static const Value True;

    /** Read only field to get a False value. */
    static const Value False;

    /** Print the Value to an output stream. */
    static std::ostream& print(std::ostream& os, const Value& v);

  private:
    ValueType type_ = ValueType::EmptyList;
    union {
      bool bool_value;
      CompoundProcedure* compoundProcedure_ptr;
      char char_value;
      fixnum_t fixnum_value;
      RawString* string_ptr;
      RawPair* pair_ptr;
      procedure_t procedure_ptr;
    };
  };

  std::ostream& operator<<(std::ostream& os, const Value& v);

  /** Pair type. */
  struct RawPair {
    Value car;
    Value cdr;
  };

  /** Information on compound procedure. */
  struct CompoundProcedure {
    /** List of parameters this function accepts. */
    Value parameters;

    /** Count of parameters. */
    size_t parameterCount;

    /** List of statements and expressions to execute. */
    Value body;

    /** Procedure's lexically scoped environment frame. */
    EnvironmentFrame* enclosingFrame;
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

  /** Get car and cdr of pair in one function call. */
  void get_pair(Value pair, Value* carOut, Value* cdrOut);

  inline Value caar(Value obj) { return car(car(obj)); }
  inline Value cadr(Value obj) { return car(cdr(obj)); }
  inline Value cdar(Value obj) { return cdr(car(obj)); }
  inline Value cddr(Value obj) { return cdr(cdr(obj)); }
  inline Value caaar(Value obj) { return car(car(car(obj))); }
  inline Value caadr(Value obj) { return car(car(cdr(obj))); }
  inline Value cadar(Value obj) { return car(cdr(car(obj))); }
  inline Value caddr(Value obj) { return car(cdr(cdr(obj))); }
  inline Value cdaar(Value obj) { return cdr(car(car(obj))); }
  inline Value cdadr(Value obj) { return cdr(car(cdr(obj))); }
  inline Value cddar(Value obj) { return cdr(cdr(car(obj))); }
  inline Value cdddr(Value obj) { return cdr(cdr(cdr(obj))); }
  inline Value caaaar(Value obj) { return car(car(car(car(obj)))); }
  inline Value caaadr(Value obj) { return car(car(car(cdr(obj)))); }
  inline Value caadar(Value obj) { return car(car(cdr(car(obj)))); }
  inline Value caaddr(Value obj) { return car(car(cdr(cdr(obj)))); }
  inline Value cadaar(Value obj) { return car(cdr(car(car(obj)))); }
  inline Value cadadr(Value obj) { return car(cdr(car(cdr(obj)))); }
  inline Value caddar(Value obj) { return car(cdr(cdr(car(obj)))); }
  inline Value cadddr(Value obj) { return car(cdr(cdr(cdr(obj)))); }
  inline Value cdaaar(Value obj) { return cdr(car(car(car(obj)))); }
  inline Value cdaadr(Value obj) { return cdr(car(car(cdr(obj)))); }
  inline Value cdadar(Value obj) { return cdr(car(cdr(car(obj)))); }
  inline Value cdaddr(Value obj) { return cdr(car(cdr(cdr(obj)))); }
  inline Value cddaar(Value obj) { return cdr(cdr(car(car(obj)))); }
  inline Value cddadr(Value obj) { return cdr(cdr(car(cdr(obj)))); }
  inline Value cdddar(Value obj) { return cdr(cdr(cdr(car(obj)))); }
  inline Value cddddr(Value obj) { return cdr(cdr(cdr(cdr(obj)))); }
} // namespace Shiny