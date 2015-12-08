#include "fcall.h"
#include "../jhelper.h"
#include "../jcallbacks.h"

using namespace JIT::Expressions;

FCall::FCall(std::vector<SafeExpression> const& args) : Expression(FunctionCall, args) {}

void FCall::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
      //TODO: SCARY! This will break with over 6 args, work out a nice way to do this
      //printf("TODO: FunctionCall arg set is broken with over 6 args\n");
      for (unsigned int i = 0; i < _args.size(); i++) {
        _args[i]->write(buffer, unresolvedList);
      }

      for (int i = _args.size(); i > 0; i--) {
        Helper::setArgumentStackTop(i - 1, buffer);
      }

      size_t addressStart = Helper::callFunction(_callbackLocation ? _callbackLocation : ((void*)Callbacks::unresolved), buffer);

      if (_callbackLocation == nullptr) {
        unresolvedList.push_back(std::pair<Expression*, size_t>(this, addressStart));
      }
}

bool FCall::isRecursion(std::vector<Type> const& storedTypes, std::vector<Type> const& argTypes, std::vector<MethodCall> const& potentiallyCalledFunctions) const {
  for (unsigned int i = 0; i < potentiallyCalledFunctions.size(); i++) {
    if (potentiallyCalledFunctions[i].stmt.get() == _callbackExpression.get()) {

        if (argTypes.size() == potentiallyCalledFunctions[i].calledWith.size()) {

          bool isMatch = true;
          for (unsigned int r = 0; r < argTypes.size(); r++) {
            if (!potentiallyCalledFunctions[i].calledWith[r].equals(argTypes[r])) {
              isMatch = false;
              break;
            }
          }

          if (isMatch) {
            return true;
          }
        }

        //TODO: DO TYPE CHECKING ON ARGS
        //Calling a function with 1 sig does not verify for all
    }
  }

  return false;
}

BaseCheckResult FCall::getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {

      std::vector<Type> argTypes;

      for (unsigned int i = 0; i < _args.size(); i++) {
        auto checkResult = _args[i]->getBaseType(storedTypes, potentiallyCalledFunctions);

        if (checkResult.recursion) {
          return BaseCheckResult{true, Unknown};
        }

        argTypes.push_back(checkResult.type);
      } 

      if (isRecursion(storedTypes, argTypes, potentiallyCalledFunctions)) {
        return BaseCheckResult{true, Unknown};
      }

      potentiallyCalledFunctions.push_back(MethodCall{_callbackExpression, argTypes});
      return _callbackExpression->getBaseType(storedTypes, potentiallyCalledFunctions);
}

ExpressionCheckResult FCall::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {

      if (_callbackExpression == nullptr) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};  
      }

      std::vector<Type> argTypes;

      for (unsigned int i = 0; i < _args.size(); i++) {
        auto checkResult = _args[i]->checkResultType(storedTypes, potentiallyCalledFunctions);

        if (checkResult.result != ExpressionCheckResult::Valid) {
          return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Unknown)};
        }

        argTypes.push_back(checkResult.resultType);
      }

      //Is this a recursion?
      if (isRecursion(storedTypes, argTypes, potentiallyCalledFunctions)) {
        auto calledFunctionsCopy = potentiallyCalledFunctions;
        auto baseTypeCheck = _callbackExpression->getBaseType(argTypes, calledFunctionsCopy);
        
        if (baseTypeCheck.recursion) {
          printf("TODO: Recursion error handling\n");
        }

        return ExpressionCheckResult{ExpressionCheckResult::InfinateRecursion, baseTypeCheck.type};
      }

      potentiallyCalledFunctions.push_back(MethodCall{_callbackExpression, argTypes});
      return _callbackExpression->checkResultType(argTypes, potentiallyCalledFunctions);
}