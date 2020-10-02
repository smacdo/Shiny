#include "runtime/Value.h"
#include <sstream>
#include <string>

using namespace Shiny;

//--------------------------------------------------------------------------------------------------
std::string Value::toString() const {
  std::ostringstream ss;

  switch (type_) {
  case ValueType::None:
    ss << "nil"; // TODO: Not sure if correct.
    break;
  case ValueType::Fixnum:
    ss << fixnum_value;
    break;
  default:
    // TODO: Better exceptions.
    throw std::runtime_error("unsupported type for tostring");
    break;
  }

  return ss.str();
}

//--------------------------------------------------------------------------------------------------
std::ostream &Shiny::operator<<(std::ostream &os, const Value &v) {
  // TODO: handle internally w/out converting to a string.
  os << v.toString();
  return os;
}
