#pragma once
#include "runtime/Value.h"

#include <initializer_list>
#include <memory>
#include <string_view>

namespace Shiny {
  class Allocator;
  class EnvironmentFrame;

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

    /** Get the global (root) environment. */
    EnvironmentFrame* globalEnvironment() noexcept;

    /** Get the gloabl (root) environment. */
    const EnvironmentFrame* globalEnvironment() const noexcept;

    /** Create a new environment by extending another environment. */
    EnvironmentFrame* extend(EnvironmentFrame* parent);

    /**
     * Create a new environment by extending another environment, and populate
     * it with the given set of bindings.
     *
     * The names and values are parallel lists and thus must be the same size.
     *
     * \param parent The parent environment frame to extend.
     * \param names  A list of symbols defining the binding names.
     * \param values A list of binding values,.
     */
    EnvironmentFrame*
        extend(EnvironmentFrame* parent, Value names, Value values);

    /** Create a new string. */
    Value makeString(std::string_view value);

    /** Make a symbol that matches the given string value. */
    Value makeSymbol(std::string_view name);

    /** Make a new pair. */
    Value makePair(Value car, Value cdr);

    /**
     * Makes a cons list of items in traditional lisp style.
     *
     * Example:
     *  makeList(1, 2, 3) => (1 . (2 . (3 . ()))).
     */
    Value makeList(std::initializer_list<Value> values);

    /**
     * Make a new compound procedure.
     *
     * \param parameters List of parameters accepted by this function.
     * \param body       List of statements and expressions to execute.
     * \param enclosing  Environment frame enclosing the compound procedure at
     *                   the time of creation.
     * \returns New CompoundProcedure value representing.
     */
    Value makeCompoundProcedure(
        Value parameters,
        Value body,
        EnvironmentFrame* enclosing);

  private:
    std::unique_ptr<Allocator> allocator_;
    std::unique_ptr<EnvironmentFrame> globalEnvironment_;
    Constants constants_;
    Value symbols_;
  };

  // TODO: Create a C API.

} // namespace Shiny