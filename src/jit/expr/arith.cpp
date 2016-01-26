#include "arith.h"
#include "../jhelper.h"

using namespace JIT::Expressions;

Arithmetic::Arithmetic(ExpressionType type, std::vector<SafeExpression> const& args) : Expression(type, args) {}

void Arithmetic::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls) {
	switch (_type) {
		case Add:
	      _args[0]->write(buffer, unresolvedList, currentCalls);
	      _args[1]->write(buffer, unresolvedList, currentCalls);
	      Helper::addTopTwoStack(buffer);
	      break;
	    case Subtract:
	      _args[0]->write(buffer, unresolvedList, currentCalls);
	      _args[1]->write(buffer, unresolvedList, currentCalls);
	      Helper::subTopTwoStack(buffer);
	      break;
	    case Multiply:
	      _args[0]->write(buffer, unresolvedList, currentCalls);
	      _args[1]->write(buffer, unresolvedList, currentCalls);
	      Helper::mulTopTwoStack(buffer);
	      break;
	    case Mod:
	      _args[0]->write(buffer, unresolvedList, currentCalls);
	      _args[1]->write(buffer, unresolvedList, currentCalls);
	      Helper::modTopTwoStack(buffer);
	      break;	    	
	    case Divide:
	      _args[0]->write(buffer, unresolvedList, currentCalls);
	      _args[1]->write(buffer, unresolvedList, currentCalls);
	      Helper::divTopTwoStack(buffer);
	      break;
	    default:
	      //Should never be reached, indicates parsing error
	      printf("ERROR: THIS IS NOT ARITHMETIC\n");
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