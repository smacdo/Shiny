#pragma once
#include "runtime/Value.h"

#include <memory>
#include <string_view>

namespace Shiny {
  class Allocator;

  /** Shiny virtual machine state. */
  class VmState {
    struct Globals {
      Value bTrue{true};
      Value bFalse{false};
      Value emptyList;
    };

  public:
    /** Constructor. */
    VmState(std::unique_ptr<Allocator> allocator);

    /** Destructor. */
    ~VmState();

    /** Get reference to VM allocated globals. */
    Globals& globals() noexcept { return globals_; }

    /** Get reference to VM allocated globals. */
    const Globals& globals() const noexcept { return globals_; }

    /** Create a new string. */
    Value makeString(std::string_view value);

    /** Make a new pair. */
    Value makePair(Value car, Value cdr);

  private:
    std::unique_ptr<Allocator> allocator_;
    Globals globals_;
  };

  // TODO: Create a C API.

} // namespace Shiny