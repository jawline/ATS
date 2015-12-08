#include "arith.h"
#include "../jhelper.h"

using namespace JIT::Expressions;

Arithmetic::Arithmetic(ExpressionType type, std::vector<SafeExpression> const& args) : Expression(type, args) {}

void Arithmetic::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
	switch (_type) {
		case Add:
	      _args[0]->write(buffer, unresolvedList);
	      _args[1]->write(buffer, unresolvedList);
	      Helper::addTopTwoStack(buffer);
	      break;
	    case Subtract:
	      _args[0]->write(buffer, unresolvedList);
	      _args[1]->write(buffer, unresolvedList);
	      Helper::subTopTwoStack(buffer);
	      break;
	    case Multiply:
	      _args[0]->write(buffer, unresolvedList);
	      _args[1]->write(buffer, unresolvedList);
	      Helper::mulTopTwoStack(buffer);
	      break;
	    case Divide:
	      _args[0]->write(buffer, unresolvedList);
	      _args[1]->write(buffer, unresolvedList);
	      Helper::divTopTwoStack(buffer);
	      break;
	};
}

BaseCheckResult Arithmetic::getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
	auto lhsType = _args[0]->getBaseType(storedTypes, potentiallyCalledFunctions);
	auto rhsType = _args[1]->getBaseType(storedTypes, potentiallyCalledFunctions);

	if (!lhsType.recursion) {
		return lhsType;
	}

	if (!rhsType.recursion) {
		return rhsType;
	}

	return BaseCheckResult{false, TypeIdentifier::Unknown};
}

ExpressionCheckResult Arithmetic::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
      auto lhsCheck = _args[0]->checkResultType(storedTypes, potentiallyCalledFunctions);
      auto rhsCheck = _args[1]->checkResultType(storedTypes, potentiallyCalledFunctions);
      
      if (lhsCheck.result == ExpressionCheckResult::Invalid || rhsCheck.result == ExpressionCheckResult::Invalid) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};
      }

      if (lhsCheck.resultType.getTypeID() != TypeIdentifier::Integer || rhsCheck.resultType.getTypeID() != TypeIdentifier::Integer) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};  
      }

      return ExpressionCheckResult{ExpressionCheckResult::Valid, Type(TypeIdentifier::Integer)};
}