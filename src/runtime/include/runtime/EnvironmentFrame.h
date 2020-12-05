#pragma once
#include "runtime/Exception.h"
#include "runtime/Value.h"

#include <vector>

namespace Shiny {
  /**
   * Holds an unordered set of bindings with the restriction that no two
   * bindings bind the same name. An environment frame also holds a pointer to
   * its parent frame, so long as it is not the root environment frame.
   */
  class EnvironmentFrame {
  public:
    struct Binding {
      Value name;
      Value value;
    };

    enum class SearchMode { Recurse, NoRecurse };

  public:
    /** Initialize a new environment with an optional parent. */
    EnvironmentFrame(EnvironmentFrame* parent = nullptr) noexcept;

    /** Get the parent environment frame. */
    EnvironmentFrame* parent() noexcept { return parent_; }

    /** Get the parent environment frame. */
    const EnvironmentFrame* parent() const noexcept { return parent_; }

    /** Check if environment frame has a parent. */
    bool hasParent() const noexcept { return parent_ != nullptr; }

    /**
     * Define a new binding.
     *
     * \param name  A symbol representing the binding name.
     * \param value The bound value.
     * \returns     True if a new binding was created, or false if an existing
     *              binding with the same name was set to this new value.
     */
    bool define(Value name, Value value);

    /**
     * Recursively looks up the value of a binding in this frame and its parent.
     * Throws an error if the binding could not be found.
     *
     * \param name A symbol representing the binding name.
     * \param mode Option to enable or disable recursive search.
     * \returns    The bound value.
     */
    Value lookup(Value name, SearchMode mode = SearchMode::Recurse) const;

    /**
     * Recursively looks up the value of a binding in this frame and its parent.
     *
     * \param name      A symbol representing the binding name.
     * \param resultOut Optional. Receives the bound value if lookup succesful.
     * \returns         True if the binding was found, false otherwise.
     */
    bool tryLookup(Value name, Value* resultOut) const;

    /**
     * Recursively looks up the value of a binding in this frame and its parent.
     *
     * \param name      A symbol representing the binding name.
     * \param mode      Option to enable or disable recursive search.
     * \param resultOut Optional. Receives the bound value if lookup succesful.
     * \returns         True if the binding was found, false otherwise.
     */
    bool tryLookup(Value name, SearchMode mode, Value* resultOut) const;

    /**
     * Sets the value of a binding after recursively searching for it in this
     * frame and its parent.
     * Throws an error if the binding could not be located.
     *
     * \param name  A symbol representing the binding name.
     * \param value The bound value.
     */
    void set(Value name, Value value);

    /**
     * Sets the value of a binding after recursively searching for it in this
     * frame and its parent.
     * Throws an error if the binding could not be located.
     *
     * \param name  A symbol representing the binding name.
     * \param value The bound value.
     * \returns     True if the binding was found and updated, false otherwise.
     */
    bool trySet(Value name, Value value);

  private:
    /**
     * Looks for a binding slot matching the name.
     *
     * \param name      A symbol representing the binding name.
     * \param resultOut Optional. Receives the slot index if lookup successful.
     * \returns         True if a matching slot was found, false otherwise.
     */
    bool tryFindSlot(Value name, size_t* indexOut) const noexcept;

  private:
    /** This environment's parent (enclosing) environment. */
    EnvironmentFrame* parent_;
    std::vector<Binding> variables_; // TODO: Make this much better.
  };

  class VariableNameSymbolRequiredException : public Exception {
  public:
    VariableNameSymbolRequiredException(Value name, EXCEPTION_CALLSITE_PARAMS);
  };

  class UnboundVariableException : public Exception {
  public:
    UnboundVariableException(Value name, EXCEPTION_CALLSITE_PARAMS);
  };
} // namespace Shiny
