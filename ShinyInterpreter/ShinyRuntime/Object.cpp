#include "stdafx.h"
#include "Object.h"

Object::Object(ObjectType type)
    : mType(type)
{
}

Object::Object(const Object& object)
    : mType(object.mType)
{
}

Object::~Object()
{
}

ObjectType Object::Type() const
{
    return mType;
}