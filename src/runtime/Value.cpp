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
