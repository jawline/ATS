#include "jfunction.h"
#include "jhelper.h"

using namespace JIT;
using namespace Assembler;
using namespace Expressions;

Function::Function(std::string const& name, SafeExpression const& stmt, size_t numArgs) {
  _stmt = SafeCompiledExpression(new CompiledExpression(stmt, numArgs));
  _numArgs = numArgs;
}

Function::~Function() {}

size_t Function::getNumArgs() const {
  return _numArgs;
}

SafeExpression Function::expression() const {
  return _stmt->getExpression();
}

unsigned int Function::rewriteCallbacks() {
  
  unsigned int unresolved;

  unresolved += _stmt->rewriteCallbacks();

  //Rewrite functions this function calls
  std::vector<Type> storedTypes;
  auto callList = std::vector<Expressions::MethodCall>();
  checkResultType(storedTypes, callList);

  for (unsigned int i = 0; i < callList.size(); i++) {
    unresolved += callList[i].cexpr->rewriteCallbacks();
  }

  return unresolved;
}

Expressions::SafeCompiledExpression Function::getCompiledExpression() const {
  return _stmt;
}

ExpressionCheckResult Function::checkResultType(std::vector<Type> const& storedTypes, std::vector<Expressions::MethodCall>& potentialMethods) {
  
  if (_numArgs != storedTypes.size()) {
    return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Boolean)};
  }

  return _stmt->getExpression()->checkResultType(storedTypes, potentialMethods);
}

void Function::simplify(SafeAnalysis analysis) {

  //TODO: Ugly
  //Simplify all functions that this function calls
  //Back to front to simplify deepest methods first
  std::vector<Type> storedTypes;
  auto callList = std::vector<Expressions::MethodCall>();
  checkResultType(storedTypes, callList);

  for (int i = callList.size() - 1; i >= 0; i--) {
    callList[i].cexpr->setExpression(analysis->doAnalysis(callList[i].cexpr->getExpression()));
  }

  _stmt->setExpression(analysis->doAnalysis(_stmt->getExpression()));
}

int64_t Function::run(char const*& errorMessage) {
  errorMessage = nullptr;

  auto fnPointer = getFnPtr();
  
  if (rewriteCallbacks()) {
    errorMessage = "unresolved functions means this method cannot be run";
  } else {
    return fnPointer();
  }
}

JFPTR Function::getFnPtr() {
  auto callList = std::vector<SafeCompiledExpression>();
  callList.push_back(_stmt);
  return _stmt->getCompiled(callList);
}