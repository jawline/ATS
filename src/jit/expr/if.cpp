#include "if.h"
#include "../jhelper.h"

using namespace JIT::Expressions;

If::If(std::vector<SafeExpression> const& args) : Expression(IfType, args) {}

void If::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {

      //Execute condition
      _args[0]->write(buffer, unresolvedList);
      
      //Write a jump instruction with BS location
      size_t elseAddr = Helper::jumpRelativeTopEqualZero(buffer, 0xDEAD);
      _args[1]->write(buffer, unresolvedList);
      
      //Write a jump location to avoid the else
      size_t exitAddr = Helper::jumpRelative(buffer, 0xDEAD);
      
      size_t elseLocation = buffer.current();
      
      //If the if comes with an else then write it otherwise write 0
      if (_args.size() == 3) {
        _args[2]->write(buffer, unresolvedList);
      } else {
        printf("ERROR EXPECTED 3 ARGS\n");
      }
      
      //Rewrite the dummy relative locations to be the actual exit
      size_t exitLocation = buffer.current();

      auto elseJmpNextInstruction = elseAddr + sizeof(int32_t);
      auto exitJmpNextInstruction = exitAddr + sizeof(int32_t);

      //In retrospect, maybe this isn't so dirty
      buffer.insert((int32_t)(elseLocation - elseJmpNextInstruction), elseAddr);
      buffer.insert((int32_t)(exitLocation - exitJmpNextInstruction), exitAddr);
}

BaseCheckResult If::getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
  auto lhsType = _args[1]->getBaseType(storedTypes, potentiallyCalledFunctions);
  auto rhsType = _args[2]->getBaseType(storedTypes, potentiallyCalledFunctions);

  if (!lhsType.recursion) {
    return lhsType;
  }

  if (!rhsType.recursion) {
    return rhsType;
  }

  return BaseCheckResult{false, Unknown};
}

ExpressionCheckResult If::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {

      //TODO: This may not work? potentiallyCalledFunctions may be changed by one of the branches leading to a reduction in exploration
      auto lhsCheck = _args[1]->checkResultType(storedTypes, potentiallyCalledFunctions);
      auto rhsCheck = _args[2]->checkResultType(storedTypes, potentiallyCalledFunctions);

      //TODO: args[0] check
      //Handle general cases
      if (lhsCheck.result == ExpressionCheckResult::Valid && rhsCheck.result == ExpressionCheckResult::Valid) {
        
        if (lhsCheck.resultType.getTypeID() != rhsCheck.resultType.getTypeID()) {
          return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};  
        }

        return ExpressionCheckResult{ExpressionCheckResult::Valid, lhsCheck.resultType};
      }

      if (lhsCheck.result == ExpressionCheckResult::Invalid) {
        return lhsCheck;
      }

      if (rhsCheck.result == ExpressionCheckResult::Invalid) {
        return rhsCheck;
      }

      //Handle the case where one or both sides infinately recurse
      ExpressionCheckResult::ExpressionCheckResult goodResult = ExpressionCheckResult{ExpressionCheckResult::InfinateRecursion, Type(TypeIdentifier::Unknown)};

      //If either side MAY not infinately recurse then return a good result
      if (lhsCheck.result != ExpressionCheckResult::InfinateRecursion) {
        goodResult = lhsCheck;
      } else if (rhsCheck.result != ExpressionCheckResult::InfinateRecursion) {
        goodResult = rhsCheck;
      }

      return goodResult;
}