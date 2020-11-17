#include "runtime/SExpression.h"
#include <catch2/catch.hpp>

using namespace Shiny;

TEST_CASE("Default initialize atom S-Expression", "[SExpression]") {
  SExprAtom a{};
  REQUIRE(a.value().isEmptyList());
}

TEST_CASE("Create a atom S-Expression with a value", "[SExpression]") {
  SExprAtom a{Value{23}};
  REQUIRE(Value{23} == a.value());

  const SExprAtom b{Value{-25}};
  REQUIRE(Value{-25} == b.value());
}

TEST_CASE("Set an atom S-Expression with a new value", "[SExpression]") {
  SExprAtom a{Value{123}};
  REQUIRE(Value{123} == a.value());

  a.setValue(Value{'~'});
  REQUIRE(Value{'~'} == a.value());
}

TEST_CASE("Create a list S-Expression node", "[SExpression") {
  auto child = std::make_unique<SExprAtom>(Value{32});
  auto sibling = std::make_unique<SExprAtom>(Value{-7});
  auto node = std::make_unique<SExprList>(std::move(child), std::move(sibling));

  auto c = dynamic_cast<SExprAtom*>(node->firstChild());
  auto s = dynamic_cast<SExprAtom*>(node->nextSibling());

  REQUIRE(Value{32} == c->value());
  REQUIRE(Value{-7} == s->value());
}

TEST_CASE("Test if list S-Expression has a child", "[SExpression") {
  SECTION("when constructed with a child") {
    auto child = std::make_unique<SExprAtom>(Value{32});
    auto sibling = std::make_unique<SExprAtom>(Value{-7});
    auto node =
        std::make_unique<SExprList>(std::move(child), std::move(sibling));
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE(node->hasChild());
    REQUIRE(cNode->hasChild());
  }

  SECTION("when constructed without a child") {
    auto child = nullptr;
    auto sibling = std::make_unique<SExprAtom>(Value{-7});
    auto node =
        std::make_unique<SExprList>(std::move(child), std::move(sibling));
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE_FALSE(node->hasChild());
    REQUIRE_FALSE(cNode->hasChild());
  }
}

TEST_CASE("Test if list S-Expression has a sibling", "[SExpression") {
  SECTION("when constructed with a sibling") {
    auto child = std::make_unique<SExprAtom>(Value{32});
    auto sibling = std::make_unique<SExprAtom>(Value{-7});
    auto node =
        std::make_unique<SExprList>(std::move(child), std::move(sibling));
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE(node->hasNextSibling());
    REQUIRE(cNode->hasNextSibling());
  }

  SECTION("when constructed without a sibling") {
    auto child = std::make_unique<SExprAtom>(Value{32});
    auto sibling = nullptr;
    auto node =
        std::make_unique<SExprList>(std::move(child), std::move(sibling));
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE_FALSE(node->hasNextSibling());
    REQUIRE_FALSE(cNode->hasNextSibling());
  }
}

TEST_CASE("Set first child on list S-Expression", "[SExpression") {
  SECTION("when initially null") {
    auto node = std::make_unique<SExprList>();
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE(nullptr == node->firstChild());
    REQUIRE(nullptr == cNode->firstChild());

    node->setFirstChild(std::make_unique<SExprAtom>(Value{'c'}));
    auto c = dynamic_cast<SExprAtom*>(node->firstChild());

    REQUIRE(Value{'c'} == c->value());
  }

  SECTION("replaces previously set first child") {
    auto node = std::make_unique<SExprList>();
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE(nullptr == node->firstChild());
    REQUIRE(nullptr == cNode->firstChild());

    node->setFirstChild(std::make_unique<SExprAtom>(Value{'c'}));
    node->setFirstChild(std::make_unique<SExprAtom>(Value{-100}));
    auto c = dynamic_cast<SExprAtom*>(node->firstChild());

    REQUIRE(Value{-100} == c->value());
  }
}

TEST_CASE("Set next sibling on list S-Expression", "[SExpression") {
  SECTION("when initially null") {
    auto node = std::make_unique<SExprList>();
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE(nullptr == node->nextSibling());
    REQUIRE(nullptr == cNode->nextSibling());

    node->setNextSibling(std::make_unique<SExprAtom>(Value{'c'}));
    auto c = dynamic_cast<SExprAtom*>(node->nextSibling());

    REQUIRE(Value{'c'} == c->value());
  }

  SECTION("replaces previously set next sibling") {
    auto node = std::make_unique<SExprList>();
    auto cNode = static_cast<const SExprList*>(node.get());

    REQUIRE(nullptr == node->nextSibling());
    REQUIRE(nullptr == cNode->nextSibling());

    node->setNextSibling(std::make_unique<SExprAtom>(Value{'c'}));
    node->setNextSibling(std::make_unique<SExprAtom>(Value{-100}));
    auto c = dynamic_cast<SExprAtom*>(node->nextSibling());

    REQUIRE(Value{-100} == c->value());
  }
}

TEST_CASE("Visit an atom S-Expression", "[SExpression]") {
  SExprAtom a{Value{88888}};

  SECTION("with regular visitor") {
    class SCustomVisitor : public SExpressionVisitor {
    public:
      SCustomVisitor(Value* resultOut) : resultOut_(resultOut) {
        REQUIRE(nullptr != resultOut);
      }

      void visit(SExprAtom& sexpr) override { *resultOut_ = sexpr.value(); }
      void visit(SExprList& sexpr) override { FAIL(); }

    private:
      Value* resultOut_;
    };

    SExprAtom a{Value{321}};
    SExprAtom b{Value{false}};
    Value result;

    SCustomVisitor v{&result};

    a.accept(v);
    REQUIRE(Value{321} == result);

    b.accept(v);
    REQUIRE(Value{false} == result);
  }

  SECTION("with const visitor") {
    class SCustomVisitor : public ConstSExpressionVisitor {
    public:
      SCustomVisitor(Value* resultOut) : resultOut_(resultOut) {
        REQUIRE(nullptr != resultOut);
      }

      void visit(const SExprAtom& sexpr) override {
        *resultOut_ = sexpr.value();
      }
      void visit(const SExprList& sexpr) override { FAIL(); }

    private:
      Value* resultOut_;
    };

    const SExprAtom a{Value{5}};
    SExprAtom b{Value{'X'}};
    Value result;

    SCustomVisitor v{&result};

    a.accept(v);
    REQUIRE(Value{5} == result);

    b.accept(v);
    REQUIRE(Value{'X'} == result);
  }
}

TEST_CASE("Visit a list S-Expression", "[SExpression]") {
  SECTION("with regular visitor") {
    class SCustomVisitor : public SExpressionVisitor {
    public:
      SCustomVisitor(Value* childOut, Value* siblingOut)
          : childOut_(childOut),
            siblingOut_(siblingOut) {
        REQUIRE(nullptr != childOut);
        REQUIRE(nullptr != siblingOut);
      }

      void visit(SExprAtom& sexpr) override { FAIL(); }
      void visit(SExprList& sexpr) override {
        *childOut_ = dynamic_cast<SExprAtom*>(sexpr.firstChild())->value();
        *siblingOut_ = dynamic_cast<SExprAtom*>(sexpr.nextSibling())->value();
      }

    private:
      Value* childOut_;
      Value* siblingOut_;
    };

    auto child = std::make_unique<SExprAtom>(Value{'C'});
    auto sibling = std::make_unique<SExprAtom>(Value{'s'});
    auto node =
        std::make_unique<SExprList>(std::move(child), std::move(sibling));

    Value childValue, siblingValue;
    SCustomVisitor v{&childValue, &siblingValue};

    node->accept(v);

    REQUIRE(Value{'C'} == childValue);
    REQUIRE(Value{'s'} == siblingValue);
  }

  SECTION("with const visitor") {
    class SCustomVisitor : public ConstSExpressionVisitor {
    public:
      SCustomVisitor(Value* childOut, Value* siblingOut)
          : childOut_(childOut),
            siblingOut_(siblingOut) {
        REQUIRE(nullptr != childOut);
        REQUIRE(nullptr != siblingOut);
      }

      void visit(const SExprAtom& sexpr) override { FAIL(); }

      void visit(const SExprList& sexpr) override {
        *childOut_ =
            dynamic_cast<const SExprAtom*>(sexpr.firstChild())->value();
        *siblingOut_ =
            dynamic_cast<const SExprAtom*>(sexpr.nextSibling())->value();
      }

    private:
      Value* childOut_;
      Value* siblingOut_;
    };

    auto child = std::make_unique<SExprAtom>(Value{'C'});
    auto sibling = std::make_unique<SExprAtom>(Value{'s'});
    auto node =
        std::make_unique<const SExprList>(std::move(child), std::move(sibling));

    Value childValue, siblingValue;
    SCustomVisitor v{&childValue, &siblingValue};

    node->accept(v);

    REQUIRE(Value{'C'} == childValue);
    REQUIRE(Value{'s'} == siblingValue);
  }
}
