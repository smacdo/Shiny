#pragma once

#include <string>

class Object;

class Reader {
public:
  Reader();
  ~Reader();

  Object *Read(const std::string &input);
};
