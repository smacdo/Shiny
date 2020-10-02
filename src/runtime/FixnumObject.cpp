#include <string>
#include <sstream>
#include "runtime/Object.h"

FixnumObject::FixnumObject(int value)
    : Object(ObjectType::Fixnum),
      mValue(value)
{
}

FixnumObject::~FixnumObject()
{
}

int FixnumObject::Value() const
{
    return mValue;
}

void FixnumObject::SetValue(int newValue)
{
    mValue = newValue;
}

std::string FixnumObject::ToString() const
{
    std::ostringstream ss;
    ss << mValue;

    return ss.str();
}
