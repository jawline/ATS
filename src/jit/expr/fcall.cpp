#include "fcall.h"
#include "../jhelper.h"
#include "../jcallbacks.h"

using namespace JIT::Expressions;

FCall::FCall(std::vector<SafeExpression> const& args) : Expression(FunctionCall, args) {}

void FCall::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls) {
      
      //TODO: SCARY! This will break with over 6 args, work out a nice way to do this
      //printf("TODO: FunctionCall arg set is broken with over 6 args\n");
      for (unsigned int i = 0; i < _args.size(); i++) {
        _args[i]->write(buffer, unresolvedList, currentCalls);
      }

      for (int i = _args.size(); i > 0; i--) {
        Helper::setArgumentStackTop(i - 1, buffer);
      }

      //TODO: I NEED ERROR PROPAGATION THROUGH HERE
      //TODO: OR THE TYPE CHECKER NEEDS TO VERIFY ALL CALLBACKS ARE RESOLVED
      void* callbackLocation = (void*) Callbacks::unresolved;
      bool recursion = false;

      for (unsigned int i = 0; i < currentCalls.size(); i++) {
        if (currentCalls[i].get() == _callbackEntry.get()) {
          recursion = true;
        }
      }

      if (!recursion && _callbackEntry.get()) {
        auto newCalls = currentCalls;
        newCalls.push_back(_callbackEntry);
        callbackLocation = (void*) _callbackEntry->getCompiled(newCalls);
      }

      size_t addressStart = Helper::callFunction(callbackLocation, buffer);

      if (recursion || !_callbackEntry.get()) {
        //printf("ERROR: Unresolved callback\n");
        unresolvedList.push_back(std::pair<SafeCompiledExpression, size_t>(_callbackEntry, addressStart));
      }
}

bool FCall::isRecursion(std::vector<Type> const& storedTypes, std::vector<Type> const& argTypes, std::vector<MethodCall> const& potentiallyCalledFunctions) const {
  for (unsigned int i = 0; i < potentiallyCalledFunctions.size(); i++) {
    if (potentiallyCalledFunctions[i].cexpr->getExpression().get() == getCallbackExpression().get()) {

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

      potentiallyCalledFunctions.push_back(MethodCall{_callbackEntry, argTypes});
      return getCallbackExpression()->getBaseType(storedTypes, potentiallyCalledFunctions);
}

ExpressionCheckResult FCall::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {

      if (getCallbackExpression() == nullptr) {
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
        auto baseTypeCheck = getCallbackExpression()->getBaseType(argTypes, calledFunctionsCopy);
        
        if (baseTypeCheck.recursion) {
          printf("TODO: Recursion error handling\n");
        }

        return ExpressionCheckResult{ExpressionCheckResult::InfinateRecursion, baseTypeCheck.type};
      }

      potentiallyCalledFunctions.push_back(MethodCall{_callbackEntry, argTypes});
      return getCallbackExpression()->checkResultType(argTypes, potentiallyCalledFunctions);
}