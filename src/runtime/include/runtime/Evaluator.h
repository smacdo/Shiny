#pragma once
// Forward declarations.
class Object;

class Evaluator {
public:
  Evaluator();
  ~Evaluator();

  Object *Evaluate(Object *pExpression);
};
