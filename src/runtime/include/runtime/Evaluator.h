#pragma once
#include "runtime/Value.h"

class Evaluator {
public:
  Evaluator();
  ~Evaluator();

  Shiny::Value Evaluate(Shiny::Value expression);
};
