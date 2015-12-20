#include "jfunction.h"
#include "jhelper.h"

using namespace JIT;
using namespace Assembler;
using namespace Expressions;

Function::Function(std::string const& name, SafeExpression const& stmt, size_t numArgs) {
  _stmt = SafeCompiledStatement(new CompiledStatement(stmt));
  _numArgs = numArgs;
}

Function::~Function() {}

size_t Function::getNumArgs() const {
	return _numArgs;
}

SafeExpression Function::expression() const {
  return _stmt->getExpression();
}

void Function::rewriteCallbacks() {}

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
  return getFnPtr()();
}

JFPTR Function::getFnPtr() {
	return _stmt->getCompiled(_numArgs);
}
