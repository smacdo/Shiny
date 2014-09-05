#include "stdafx.h"
#include "Reader.h"

#include "FixnumObject.h"

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
