// Copyright (c) Scott MacDonald. All rights reserved.
#pragma once

#include <string>

enum class ObjectType
{
    Fixnum
};

class Object
{
public:
    virtual ~Object();

    ObjectType Type() const;

    virtual std::string ToString() const = 0;

protected:
    Object(const Object& object);
    Object(ObjectType type);

private:
    ObjectType mType;
};

