#include "runtime/Value.h"

#include "runtime/Allocator.h"
#include "runtime/RawString.h"

#include <sstream>
#include <string>
#include <string_view>

using namespace Shiny;

namespace {
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
