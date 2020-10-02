#include "runtime/Object.h"
#include <sstream>
#include <string>

//--------------------------------------------------------------------------------------------------
FixnumObject::FixnumObject(int value) : Object(ObjectType::Fixnum), mValue(value) {}

//--------------------------------------------------------------------------------------------------
FixnumObject::~FixnumObject() {}

//--------------------------------------------------------------------------------------------------
int FixnumObject::Value() const { return mValue; }

//--------------------------------------------------------------------------------------------------
void FixnumObject::SetValue(int newValue) { mValue = newValue; }

//--------------------------------------------------------------------------------------------------
std::string FixnumObject::ToString() const {
  std::ostringstream ss;
  ss << mValue;

  return ss.str();
}
