#pragma once
#include "runtime/Value.h"
#include <string>

class Object;

class Reader {
public:
  Reader();
  ~Reader();

  // TODO: string_view?
  Shiny::Value Read(const std::string &input);
};
