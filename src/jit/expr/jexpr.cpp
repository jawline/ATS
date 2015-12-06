#include "jexpr.h"
#include "../jhelper.h"

using namespace JIT;
using namespace Expressions;

//TODO: callbackExpression can cause a leak as the ref counter will never deref
//TODO: entryRef will do the same

Expression::Expression(ExpressionType type, size_t storedIndex) {
  _type = type;
  _storedIndex = storedIndex;
}

Expression::Expression(ExpressionType type, std::vector<SafeExpression> const& args) {
  _type = type;
  _args = args;
}

Expression::Expression(ExpressionType type, void* callback, std::vector<SafeExpression> const& args) {
  _type = type;
  _callbackLocation = callback;
  _callbackExpression = nullptr;
  _args = args;
}

void Expression::updateCallback(void* callback, SafeExpression callbackExpression) {
  _callbackLocation = callback;
  _callbackExpression = callbackExpression;
}

void* Expression::getCallback() const {
  return _callbackLocation;
}

int Expression::getNumArgs() const {
  return _args.size();
}

void Expression::setEntry(SafeExpression stmt) {
  _entryRef = stmt;
  for (unsigned int i = 0; i < _args.size(); i++) {
    _args[i]->setEntry(stmt);
  }
}

void Expression::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
  printf("JIT shouldnt be called on base type\n");
}

ExpressionCheckResult Expression::checkResultType(std::vector<Type> const& storedTypes, unsigned int level) {
  return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Boolean)};
}
