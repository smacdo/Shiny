#include "runtime/Value.h"
#include <sstream>
#include <string>

using namespace Shiny;

//--------------------------------------------------------------------------------------------------
std::string Value::toString() const {
  std::ostringstream ss;

  switch (type_) {
  case ValueType::Null:
    ss << "()";
    break;
  case ValueType::Boolean:
    ss << (bool_value ? "#t" : "#f");
    break;
  case ValueType::Fixnum:
    ss << fixnum_value;
    break;
  case ValueType::Character:
    ss << "#\\";

    // TODO: Replace with a table driven approach for special names.
    switch (char_value) {
    case SpecialChars::kAlarmValue:
      ss << SpecialChars::kAlarmName;
      break;
    case SpecialChars::kBackspaceValue:
      ss << SpecialChars::kBackspaceName;
      break;
    case SpecialChars::kDeleteValue:
      ss << SpecialChars::kDeleteName;
      break;
    case SpecialChars::kEscapeValue:
      ss << SpecialChars::kEscapeName;
      break;
    case SpecialChars::kNewlineValue:
      ss << SpecialChars::kNewlineName;
      break;
    case SpecialChars::kNullValue:
      ss << SpecialChars::kNullName;
      break;
    case SpecialChars::kReturnValue:
      ss << SpecialChars::kReturnName;
      break;
    case SpecialChars::kSpaceValue:
      ss << SpecialChars::kSpaceName;
      break;
    case SpecialChars::kTabValue:
      ss << SpecialChars::kTabName;
      break;
    default:
      ss << char_value;
      break;
    }
    break;
  default:
    throw Exception(
        "Missing implementation for this type", EXCEPTION_CALLSITE_ARGS);
    break;
  }

  return ss.str();
}

//--------------------------------------------------------------------------------------------------
std::ostream& Shiny::operator<<(std::ostream& os, const Value& v) {
  // TODO: handle internally w/out converting to a string.
  os << v.toString();
  return os;
}
