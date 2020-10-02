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

class FixnumObject : public Object
{
public:
    FixnumObject(int value);
    FixnumObject(const FixnumObject& object);

    ~FixnumObject();

    virtual std::string ToString() const override;

    int Value() const;
    void SetValue(int newValue);

private:
    int mValue;
};
