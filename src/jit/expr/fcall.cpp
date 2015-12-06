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

ExpressionCheckResult FCall::checkResultType(std::vector<Type> const& storedTypes, unsigned int level) {
      if (_callbackExpression == nullptr) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};  
      }

      std::vector<Type> argTypes;

      for (unsigned int i = 0; i < _args.size(); i++) {
        auto checkResult = _args[i]->checkResultType(storedTypes, level);

        if (checkResult.result != ExpressionCheckResult::Valid) {
          return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};
        }

        argTypes.push_back(checkResult.resultType);
      }

      if (_callbackExpression.get() == _entryRef.get()) {
        if (level > 0 ) {
          printf("TODO: UNCAUGHT RECURSION\n");
          return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};
        } else {
          return _callbackExpression->checkResultType(argTypes, level + 1);
        }
      }

      return _callbackExpression->checkResultType(argTypes, level);
}