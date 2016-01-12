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
  _callbackEntry = nullptr;
}

void Expression::setCallbackEntry(SafeCompiledStatement const& stmt) {
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

void Expression::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList, std::vector<SafeCompiledStatement> const& currentCalls) {
  printf("JIT shouldnt be called on base type\n");
}

ExpressionCheckResult Expression::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
  return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};
}

SafeExpression Expression::getCallbackExpression() const {
  return _callbackEntry->getExpression();
}

SafeCompiledStatement Expression::getCallbackCompiledStatement() const {
  return _callbackEntry;
}

CompiledStatement::CompiledStatement(SafeExpression expr, size_t numArgs) {
  _expr = expr;
  _cachedCallback = nullptr;
  _numArgs = numArgs;
}

CompiledStatement::~CompiledStatement() {
  if (_cachedCallback) {
    Helper::freeFunctionPointer(_cachedCallback, _fnSize);
  }
}

void CompiledStatement::prepare(size_t numArgs, std::vector<SafeCompiledStatement> const& currentCalls) {
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

void CompiledStatement::setExpression(SafeExpression const& expr) {
  _expr = expr;
}

SafeExpression CompiledStatement::getExpression() const {
  return _expr;
}

void CompiledStatement::rewriteCallbacks() {

  if (!_cachedCallback) {
    return;
  }

  printf("TODO: Rewriting callbacks still doesn't work\n");/*
  for (unsigned int i = 0; i < _unresolvedCallList.size(); i++) {
    if (_unresolvedCallList[i].first->getCallbackExpression().get() {
      Helper::updateAddress(_storedFn, _unresolvedCallList[i].second, _unresolvedCallList[i].first->getCallback());
      _unresolvedCallList.erase(_unresolvedCallList.begin() + i);
      //Drop back by 1 as the next item will now hold this items index
      i--;
    }
  }*/
}

JFPTR CompiledStatement::getCompiled(std::vector<SafeCompiledStatement> const& currentCalls) {
  
  if (!_cachedCallback) {
    prepare(_numArgs, currentCalls);
  }

  return _cachedCallback;
}