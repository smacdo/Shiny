#pragma once
#include "runtime/Value.h"

#include <memory>

namespace Shiny {
  class VmState;

  /** Read Shiny code and return result. */
  Value read(std::string_view input);

  /** Read Shiny code and return result. */
  Value read(std::string_view input, std::shared_ptr<VmState> vmState);

  /** Evaluate Shiny code and return result. */
  Value evaluate(std::string_view input, std::shared_ptr<VmState> vmState);

  /** Test if list `actual` is equal to list `actual. */
  bool listEquals(Value expected, Value actual);
} // namespace Shiny