#include "runtime/Reader.h"
#include "runtime/Object.h"

#include <string>

Reader::Reader()
{
}

Reader::~Reader()
{
}

Object * Reader::Read(const std::string& input)
{
    return new FixnumObject(42);
}
