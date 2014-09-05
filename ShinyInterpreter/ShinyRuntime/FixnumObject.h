#pragma once
#include "Object.h"
#include <string>

class FixnumObject :
    public Object
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

