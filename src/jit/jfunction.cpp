#include "jfunction.h"
#include "jhelper.h"

using namespace JIT;
using namespace Assembler;
using namespace Expressions;

Function::Function(std::string const& name, SafeExpression const& stmt, size_t numArgs) {
  _storedFn = nullptr;
  _stmt = stmt;
  _numArgs = numArgs;
  _stmt->setMarker(name);
}

Function::~Function() {
  if (_storedFn) {
  	Helper::freeFunctionPointer(_storedFn, _fnSize);
  }
}

void Function::prepare(SafeExpression const& stmt) {
  ByteBuffer buffer;

  Helper::insertPrologue(buffer);

  //Push all the args so they sit left to right from ebp
  Helper::functionEntryPushArgs(_numArgs, buffer);
  stmt->write(buffer, _unresolvedCallList);
  Helper::popResult(buffer);
  Helper::functionExitDiscardArgs(_numArgs, buffer);
  Helper::insertEpilogue(buffer);
  
  _storedFn = Helper::prepareFunctionPointer(buffer);
  _fnSize = buffer.current();
}

size_t Function::getNumArgs() const {
	return _numArgs;
}

SafeExpression Function::expression() const {
  return _stmt;
}

void Function::rewriteCallbacks() {

	if (!_storedFn) {
		return;
	}

	for (unsigned int i = 0; i < _unresolvedCallList.size(); i++) {
		if (_unresolvedCallList[i].first->getCallback()) {
			Helper::updateAddress(_storedFn, _unresolvedCallList[i].second, _unresolvedCallList[i].first->getCallback());
			_unresolvedCallList.erase(_unresolvedCallList.begin() + i);
			//Drop back by 1 as the next item will now hold this items index
			i--;
		}
	}
}

ExpressionCheckResult Function::checkResultType(std::vector<Type> const& storedTypes, std::vector<Expressions::MethodCall>& potentialMethods) {
  
  if (_numArgs != storedTypes.size()) {
    return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Boolean)};
  }

  return _stmt->checkResultType(storedTypes, potentialMethods);
}

int64_t Function::run() {
  return getFnPtr()();
}

JFPTR Function::getFnPtr() {
	if (!_storedFn) {
		prepare(_stmt);
	}
	return _storedFn;
}
