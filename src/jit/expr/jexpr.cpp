#include "jexpr.h"
#include "../jhelper.h"

using namespace JIT;
using namespace Expressions;
using namespace Assembler;

//TODO: callbackExpression can cause a leak as the ref counter will never deref
//TODO: entryRef will do the same

Expression::Expression(ExpressionType type, std::vector<SafeExpression> const& args) {
  _type = type;
  _args = args;
  _callbackLocation = nullptr;
  _callbackExpression = nullptr;
}

void Expression::updateCallback(void* callback, SafeExpression callbackExpression) {
  _callbackLocation = callback;
  _callbackExpression = callbackExpression;
}

ExpressionType Expression::getExpressionType() const {
  return _type;
}

void* Expression::getCallback() const {
  return _callbackLocation;
}

std::vector<SafeExpression> Expression::getArguments() const {
  return _args;
}

int Expression::getNumArgs() const {
  return _args.size();
}

void Expression::setArguments(std::vector<SafeExpression> const& args) {
  _args = args;
}

void Expression::setMarker(std::string const& marker) {
  _marker = marker;
}

std::string Expression::getMarker() const {
  return _marker;
}

void Expression::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
  printf("JIT shouldnt be called on base type\n");
}

ExpressionCheckResult Expression::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
  return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};
}

CompiledStatement::CompiledStatement(SafeExpression expr) {
  _expr = expr;
  _cachedCallback = nullptr;
}

CompiledStatement::~CompiledStatement() {
  if (_cachedCallback) {
    Helper::freeFunctionPointer(_cachedCallback, _fnSize);
  }
}

void CompiledStatement::prepare(size_t numArgs) {
  ByteBuffer buffer;

  Helper::insertPrologue(buffer);

  //Push all the args so they sit left to right from ebp
  Helper::functionEntryPushArgs(numArgs, buffer);
  _expr->write(buffer, _unresolvedCallList);
  Helper::popResult(buffer);
  Helper::functionExitDiscardArgs(numArgs, buffer);
  Helper::insertEpilogue(buffer);
  
  _cachedCallback = Helper::prepareFunctionPointer(buffer);
  _fnSize = buffer.current();
}

void CompiledStatement::setExpression(SafeExpression const& expr) {
  _expr = expr;
}

SafeExpression CompiledStatement::getExpression() const {
  return _expr;
}

JFPTR CompiledStatement::getCompiled(size_t numArgs) {
  
  if (!_cachedCallback) {
    prepare(numArgs);
  }

  return _cachedCallback;
}