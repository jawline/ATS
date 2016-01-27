#include "jexpr.h"
#include "../jhelper.h"
#include <stdlib.h>

using namespace JIT;
using namespace Expressions;
using namespace Assembler;

//TODO: callbackExpression can cause a leak as the ref counter will never deref

Expression::Expression(ExpressionType type, std::vector<SafeExpression> const& args) {
  _type = type;
  _args = args;
  _callbackEntry = nullptr;
}

void Expression::setCallbackEntry(SafeCompiledExpression const& stmt) {
  _callbackEntry = stmt;
}

ExpressionType Expression::getExpressionType() const {
  return _type;
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

void Expression::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls) {
  printf("ERROR: JIT shouldnt be called on base type\n");
  exit(150);
}

ExpressionCheckResult Expression::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
  return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};
}

SafeExpression Expression::getCallbackExpression() const {
  return _callbackEntry->getExpression();
}

SafeCompiledExpression Expression::getCallbackCompiledExpression() const {
  return _callbackEntry;
}

CompiledExpression::CompiledExpression(SafeExpression expr, size_t numArgs) {
  _expr = expr;
  _cachedCallback = nullptr;
  _numArgs = numArgs;
}

CompiledExpression::~CompiledExpression() {
  freeStoredPointer();
}

void CompiledExpression::prepare(size_t numArgs, std::vector<SafeCompiledExpression> const& currentCalls) {
  ByteBuffer buffer;

  Helper::insertPrologue(buffer);

  //Push all the args so they sit left to right from ebp
  Helper::functionEntryPushArgs(numArgs, buffer);
  _expr->write(buffer, _unresolvedCallList, currentCalls);
  Helper::popResult(buffer);
  Helper::functionExitDiscardArgs(numArgs, buffer);
  Helper::insertEpilogue(buffer);
  
  _cachedCallback = Helper::prepareFunctionPointer(buffer);
  _fnSize = buffer.current();
}

void CompiledExpression::freeStoredPointer() {
  if (_cachedCallback) {
    Helper::freeFunctionPointer(_cachedCallback, _fnSize);
    _cachedCallback = nullptr;
  }
}

void CompiledExpression::setExpression(SafeExpression const& expr) {
  freeStoredPointer();
  _expr = expr;
}

SafeExpression CompiledExpression::getExpression() const {
  return _expr;
}

void CompiledExpression::rewriteCallbacks() {

  if (!_cachedCallback) {
    return;
  }

  for (unsigned int i = 0; i < _unresolvedCallList.size(); i++) {
    
    auto basicList = std::vector<SafeCompiledExpression>();
    basicList.push_back(_unresolvedCallList[i].first);

    auto foundCallback = _unresolvedCallList[i].first->getCompiled(basicList);

    if (foundCallback) {
      Helper::updateAddress(_cachedCallback, _unresolvedCallList[i].second, (void*) foundCallback);
      _unresolvedCallList.erase(_unresolvedCallList.begin() + i);
      //Drop back by 1 as the next item will now hold this items index
      i--;
    }
  }
}

JFPTR CompiledExpression::getCompiled(std::vector<SafeCompiledExpression> const& currentCalls) {
  
  if (!_cachedCallback) {
    prepare(_numArgs, currentCalls);
  }

  return _cachedCallback;
}