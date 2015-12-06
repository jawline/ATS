#include "jexpr.h"
#include "../jhelper.h"
#include "../jcallbacks.h"

using namespace JIT;
using namespace Expressions;

//TODO: callbackExpression can cause a leak as the ref counter will never deref
//TODO: entryRef will do the same

Expression::Expression(ExpressionType type, size_t storedIndex) {
  _type = type;
  _storedIndex = storedIndex;
}

Expression::Expression(ExpressionType type, std::vector<SafeExpression> const& args) {
  _type = type;
  _args = args;
}

Expression::Expression(ExpressionType type, void* callback, std::vector<SafeExpression> const& args) {
  _type = type;
  _callbackLocation = callback;
  _callbackExpression = nullptr;
  _args = args;
}

void Expression::updateCallback(void* callback, SafeExpression callbackExpression) {
  _callbackLocation = callback;
  _callbackExpression = callbackExpression;
}

void* Expression::getCallback() const {
  return _callbackLocation;
}

int Expression::getNumArgs() const {
  return _args.size();
}

void Expression::setEntry(SafeExpression stmt) {
  _entryRef = stmt;
  for (unsigned int i = 0; i < _args.size(); i++) {
    _args[i]->setEntry(stmt);
  }
}

void Expression::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
  switch (_type) {
    case Stored:
      Helper::pushArgumentTop(_storedIndex, buffer);
      break;

    case FunctionCall: {

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

      break;
    }
    default:
      printf("Could not JIT\n");
      break;
  }
}

ExpressionCheckResult Expression::checkResultType(std::vector<Type> const& storedTypes, unsigned int level) {

  switch (_type) {

    case FunctionCall: {
      
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

    case Stored:
      return ExpressionCheckResult{ExpressionCheckResult::Valid, storedTypes[_storedIndex]};

    default:
      return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Boolean)};
  }
}
