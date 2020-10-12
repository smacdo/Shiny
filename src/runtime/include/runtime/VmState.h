#pragma once
#include "runtime/Value.h"

namespace Shiny {

  /** Shiny virtual machine state. */
  class VmState {
    struct Globals {
      Value bTrue{true};
      Value bFalse{false};
    };

  public:
    /** Get reference to VM allocated globals. */
    Globals& globals() noexcept { return globals_; }

    /** Get reference to VM allocated globals. */
    const Globals& globals() const noexcept { return globals_; }

  private:
    Globals globals_;
  };
} // namespace Shiny