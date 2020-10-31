#pragma once
#include "runtime/Value.h"

class Evaluator {
public:
  Evaluator();
  ~Evaluator();

  Shiny::Value evaluate(Shiny::Value expression);
};
