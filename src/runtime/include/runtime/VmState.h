#pragma once
#include "runtime/Environment.h"
#include "runtime/Value.h"

#include <memory>
#include <string_view>

namespace Shiny {
  class Allocator;
  class Environment;

  /** Shiny virtual machine state. */
  class VmState {
    struct Constants {
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
    Constants& constants() noexcept { return constants_; }

    /** Get reference to VM allocated globals. */
    const Constants& constants() const noexcept { return constants_; }

    /** Get the root environment. */
    Environment& environment() noexcept { return env_; };

    /** Get the root environment. */
    const Environment& environment() const noexcept { return env_; };

    /** Create a new string. */
    Value makeString(std::string_view value);

    /** Make a new pair. */
    Value makePair(Value car, Value cdr);

    /** Make a symbol that matches the given string value. */
    Value makeSymbol(std::string_view name);

  private:
    std::unique_ptr<Allocator> allocator_;
    Environment env_;
    Constants constants_;
    Value symbols_;
  };

  // TODO: Create a C API.

} // namespace Shiny