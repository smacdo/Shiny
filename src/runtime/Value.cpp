#include "runtime/Value.h"

#include "runtime/Allocator.h"
#include "runtime/RawString.h"
#include "runtime/VmState.h"

#include <cassert>
#include <sstream>
#include <string>
#include <string_view>

#include <fmt/format.h>

using namespace Shiny;

namespace {
  // TODO: Move printing functions to a separate printing API.
  // TODO: Can we switch away from ostream? (perf, less C++ API)

  std::ostream& printPair(std::ostream& os, RawPair* pair) {
    assert(pair != nullptr);

    // Print the left side.
    Value::print(os, pair->car);

    // Right side printing depends on cdr type.
    if (pair->cdr.isPair()) {
      // Proper list - print next element.
      os << " ";
      printPair(os, pair->cdr.toRawPair());
    } else if (pair->cdr.isEmptyList()) {
      // Proper list - end; nothing to print.
    } else {
      // Improper list.
      os << " . ";
      Value::print(os, pair->cdr);
    }

    return os;
  }

  std::ostream& printString(std::ostream& os, std::string_view s) {
    // TODO: Improve performance by reading up to special character, and then
    // printing all non-special characters in one go.
    os << "\"";

    for (const auto c : s) {
      switch (c) {
      case '\\':
        os << "\\\\";
        break;
      case '"':
        os << "\\\"";
        break;
      case '\n':
        os << "\\n";
        break;
      default:
        os << c;
        break;
      }
    }

    os << "\"";
    return os;
  }

  std::ostream& printChar(std::ostream& os, char c) {
    // TODO: Replace with a table driven approach for special names.
    os << "#\\";

    switch (c) {
    case SpecialChars::kAlarmValue:
      os << SpecialChars::kAlarmName;
      break;
    case SpecialChars::kBackspaceValue:
      os << SpecialChars::kBackspaceName;
      break;
    case SpecialChars::kDeleteValue:
      os << SpecialChars::kDeleteName;
      break;
    case SpecialChars::kEscapeValue:
      os << SpecialChars::kEscapeName;
      break;
    case SpecialChars::kNewlineValue:
      os << SpecialChars::kNewlineName;
      break;
    case SpecialChars::kNullValue:
      os << SpecialChars::kNullName;
      break;
    case SpecialChars::kReturnValue:
      os << SpecialChars::kReturnName;
      break;
    case SpecialChars::kSpaceValue:
      os << SpecialChars::kSpaceName;
      break;
    case SpecialChars::kTabValue:
      os << SpecialChars::kTabName;
      break;
    default:
      os << c;
      break;
    }

    return os;
  }
} // namespace

//--------------------------------------------------------------------------------------------------
std::string_view Value::toStringView() const {
  assert(string_ptr != nullptr);
  return to_stringView(string_ptr);
}

//--------------------------------------------------------------------------------------------------
std::string Value::toString() const {
  std::ostringstream ss;
  print(ss, *this);
  return ss.str();
}

//--------------------------------------------------------------------------------------------------
std::ostream& Value::print(std::ostream& os, const Value& v) {
  switch (v.type()) {
  case ValueType::EmptyList:
    os << "()";
    break;
  case ValueType::Boolean:
    os << (v.toBool() ? "#t" : "#f");
    break;
  case ValueType::Fixnum:
    os << v.toFixnum();
    break;
  case ValueType::Character:
    printChar(os, v.toChar());
    break;
  case ValueType::String:
    printString(os, v.toStringView());
    break;
  case ValueType::Pair:
    os << "(";
    printPair(os, v.toRawPair());
    os << ")";
    break;
  default:
    throw Exception(
        "Missing printer support for this type", EXCEPTION_CALLSITE_ARGS);
    break;
  }

  return os;
}

//--------------------------------------------------------------------------------------------------
std::ostream& Shiny::operator<<(std::ostream& os, const Value& v) {
  return Value::print(os, v);
}

//--------------------------------------------------------------------------------------------------
std::string_view Shiny::to_string(ValueType valueType) noexcept {
  assert((size_t)valueType < ValueTypeNames.size());
  return ValueTypeNames[(size_t)valueType];
}

//------------------------------------------------------------------------------
WrongValueTypeException::WrongValueTypeException(
    ValueType expectedType,
    ValueType actualType,
    EXCEPTION_CALLSITE_PARAMS)
    : Exception(
          fmt::format(to_string(expectedType), to_string(actualType)),
          EXCEPTION_INIT_BASE_ARGS) {}

//--------------------------------------------------------------------------------------------------
Value Shiny::cons(VmState* vm, Value car, Value cdr) {
  assert(vm != nullptr);
  return vm->makePair(car, cdr);
}

//--------------------------------------------------------------------------------------------------
Value Shiny::car(Value pair) {
  if (!pair.isPair()) {
    throw WrongValueTypeException(
        ValueType::Pair, pair.type(), EXCEPTION_CALLSITE_ARGS);
  }

  auto rawPair = pair.toRawPair();
  assert(rawPair != nullptr);

  return rawPair->car;
}

//--------------------------------------------------------------------------------------------------
void Shiny::set_car(Value pair, Value v) {
  if (!pair.isPair()) {
    throw WrongValueTypeException(
        ValueType::Pair, pair.type(), EXCEPTION_CALLSITE_ARGS);
  }

  auto rawPair = pair.toRawPair();
  assert(rawPair != nullptr);

  rawPair->car = v;
}

//--------------------------------------------------------------------------------------------------
Value Shiny::cdr(Value pair) {
  if (!pair.isPair()) {
    throw WrongValueTypeException(
        ValueType::Pair, pair.type(), EXCEPTION_CALLSITE_ARGS);
  }

  auto rawPair = pair.toRawPair();
  assert(rawPair != nullptr);

  return rawPair->cdr;
}

//--------------------------------------------------------------------------------------------------
void Shiny::set_cdr(Value pair, Value v) {
  if (!pair.isPair()) {
    throw WrongValueTypeException(
        ValueType::Pair, pair.type(), EXCEPTION_CALLSITE_ARGS);
  }
  auto rawPair = pair.toRawPair();
  assert(rawPair != nullptr);

  rawPair->cdr = v;
}