#pragma once
#include "runtime/Value.h"

#include <cassert>
#include <memory>
#include <vector>

namespace Shiny {
  class SExpression;
  class SExprAtom;
  class SExprList;

  /** S-Expression visitor base class. */
  class SExpressionVisitor {
  public:
    virtual ~SExpressionVisitor() = default;

    virtual void visit(SExpression& sexpr) {
      throw std::runtime_error("Missing overload for S-Expression visitor");
    }

    virtual void visit(SExprAtom& sexpr) = 0;
    virtual void visit(SExprList& sexpr) = 0;
  };

  /** Const S-Expression visitor base class. */
  class ConstSExpressionVisitor {
  public:
    virtual ~ConstSExpressionVisitor() = default;

    virtual void visit(const SExpression& sexpr) {
      throw std::runtime_error("Missing overload for S-Expression visitor");
    }

    virtual void visit(const SExprAtom& sexpr) = 0;
    virtual void visit(const SExprList& sexpr) = 0;
  };

  /**
   * Represents the base S-Expression type used for expressing nested leaf type
   * data. The base S-Expression holds metadata used by the language's runtime
   * to store data like file locations.
   */
  class SExpression {
  public:
    /** Destructor. */
    virtual ~SExpression() = default;

    /** Calls the appropriate overloaded method on the visitor object. */
    virtual void accept(SExpressionVisitor& visitor) = 0;

    /** Calls the appropriate overloaded method on the visitor object. */
    virtual void accept(ConstSExpressionVisitor& visitor) const = 0;
  };

  /**
   * Holds a pointer to the next sibling in the linked list as well as a pointer
   * to the first child node of this node.
   *
   * This class is not a general purpose linked list class as it lacks a lot of
   * expected utility functions. In general SExprList should be used to
   * construct linked lists in one go without any modifications once children
   * and siblings are attached (eg there is no way to remove a child short of
   * unsetting the first child node).
   */
  class SExprList : public SExpression {
  public:
    /**
     * Initialize new cons cell S-Expression with optional child, sibling
     * pointers.
     *
     * \param firstChild   Optional pointer to the first child for this node.
     * \param nexxtSibling Optional pointer to the next sibling of this node.
     */
    SExprList(
        std::unique_ptr<SExpression> firstChild = nullptr,
        std::unique_ptr<SExpression> nextSibling = nullptr) noexcept
        : firstChild_(std::move(firstChild)),
          nextSibling_(std::move(nextSibling)) {}

    /** Destructor that also releases its child and next sibling nodes. */
    ~SExprList() override = default;

    /** Check if this S-Expression has a non-null first child. */
    bool hasChild() const noexcept { return firstChild_ != nullptr; }

    /** Get the first child as an unowned pointer. */
    SExpression* firstChild() noexcept { return firstChild_.get(); }

    /** Get the first child as an unowned pointer. */
    const SExpression* firstChild() const noexcept { return firstChild_.get(); }

    /** Set pointer to this node's first child. */
    void setFirstChild(std::unique_ptr<SExpression> firstChild) noexcept {
      firstChild_ = std::move(firstChild);
    }

    /** Check if this S-Expression has a non-null sibling. */
    bool hasNextSibling() const noexcept { return nextSibling_ != nullptr; }

    /** Get the next sibling as an unowned pointer. */
    SExpression* nextSibling() noexcept { return nextSibling_.get(); }

    /** Get the next sibling as an unowned pointer. */
    const SExpression* nextSibling() const noexcept {
      return nextSibling_.get();
    }

    /** Set pointer to this node's next sibling. */
    void setNextSibling(std::unique_ptr<SExpression> nextSibling) noexcept {
      nextSibling_ = std::move(nextSibling);
    }

    /** Calls the appropriate overloaded method on the visitor object. */
    void accept(SExpressionVisitor& visitor) override { visitor.visit(*this); }

    /** Calls the appropriate overloaded method on the visitor object. */
    void accept(ConstSExpressionVisitor& visitor) const override {
      visitor.visit(*this);
    }

  private:
    std::unique_ptr<SExpression> firstChild_;
    std::unique_ptr<SExpression> nextSibling_;
  };

  /** Holds a value. */
  class SExprAtom : public SExpression {
  public:
    /** Initialize atom with value as empty list. */
    SExprAtom() noexcept = default;

    /** Initialize atom with value. */
    SExprAtom(Value value) : value_(value) {}

    /** Get atom value. */
    Value value() const { return value_; }

    /** Set atom value. */
    void setValue(Value v) { value_ = v; }

    /** Calls the appropriate overloaded method on the visitor object. */
    void accept(SExpressionVisitor& visitor) override { visitor.visit(*this); }

    /** Calls the appropriate overloaded method on the visitor object. */
    void accept(ConstSExpressionVisitor& visitor) const override {
      visitor.visit(*this);
    }

  private:
    Value value_;
  };
} // namespace Shiny
