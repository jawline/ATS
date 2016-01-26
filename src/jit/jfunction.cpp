#include "jfunction.h"
#include "jhelper.h"

using namespace JIT;
using namespace Assembler;
using namespace Expressions;

Function::Function(std::string const& name, SafeExpression const& stmt, size_t numArgs) {
  _stmt = SafeCompiledExpression(new CompiledStatement(stmt, numArgs));
  _numArgs = numArgs;
}

Function::~Function() {}

size_t Function::getNumArgs() const {
  return _numArgs;
}

SafeExpression Function::expression() const {
  return _stmt->getExpression();
}

void Function::rewriteCallbacks() {
  
  _stmt->rewriteCallbacks();

  //Rewrite functions this function calls
  std::vector<Type> storedTypes;
  auto callList = std::vector<Expressions::MethodCall>();
  checkResultType(storedTypes, callList);

  for (unsigned int i = 0; i < callList.size(); i++) {
    
  }
}

Expressions::SafeCompiledExpression Function::getCompiledStatement() const {
  return _stmt;
}

ExpressionCheckResult Function::checkResultType(std::vector<Type> const& storedTypes, std::vector<Expressions::MethodCall>& potentialMethods) {
  
  if (_numArgs != storedTypes.size()) {
    return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Boolean)};
  }

  return _stmt->getExpression()->checkResultType(storedTypes, potentialMethods);
}

void Function::simplify(SafeAnalysis analysis) {
  _stmt->setExpression(analysis->doAnalysis(_stmt->getExpression()));
}

int64_t Function::run() {
  auto fnPointer = getFnPtr();
  rewriteCallbacks();
  return fnPointer();
}

JFPTR Function::getFnPtr() {
  auto callList = std::vector<SafeCompiledExpression>();
  callList.push_back(_stmt);
  return _stmt->getCompiled(callList);
}