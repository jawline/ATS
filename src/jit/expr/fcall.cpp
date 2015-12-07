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

bool FCall::isRecursion(std::vector<Type> const& storedTypes, std::vector<Type> const& argTypes, std::vector<SafeExpression> const& potentiallyCalledFunctions) {
  for (unsigned int i = 0; i < potentiallyCalledFunctions.size(); i++) {
    if (potentiallyCalledFunctions[i].get() == _callbackExpression.get()) {
        printf("TODO: Returning true on recursion even though type signatures arent verified\n");
        //TODO: DO TYPE CHECKING ON ARGS
        //Calling a function with 1 sig does not verify for all
        return true;
    }
  }

  return false;
}

ExpressionCheckResult FCall::checkResultType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions) {

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
        return ExpressionCheckResult{ExpressionCheckResult::InfinateRecursion, Type(TypeIdentifier::Unknown)};
      }

      potentiallyCalledFunctions.push_back(_callbackExpression);
      return _callbackExpression->checkResultType(argTypes, potentiallyCalledFunctions);
}