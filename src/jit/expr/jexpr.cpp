#include "jexpr.h"
#include "../jhelper.h"
#include "../jcallbacks.h"

using namespace JIT;
using namespace Expressions;

Expression::Expression(ExpressionType type) {
  _type = type;
}

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
    case Stored:
      Helper::pushArgumentTop(_storedIndex, buffer);
      break;
    case If: {
      
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
        Helper::pushNumber(0, buffer);
      }
      
      //Rewrite the dummy relative locations to be the actual exit
      size_t exitLocation = buffer.current();

      auto elseJmpNextInstruction = elseAddr + sizeof(int32_t);
      auto exitJmpNextInstruction = exitAddr + sizeof(int32_t);

      //In retrospect, maybe this isn't so dirty
      buffer.insert((int32_t)(elseLocation - elseJmpNextInstruction), elseAddr);
      buffer.insert((int32_t)(exitLocation - exitJmpNextInstruction), exitAddr);
      break;
    }

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

    case Add:
    case Subtract:
    case Multiply:
    case Divide: {
      auto lhsCheck = _args[0]->checkResultType(storedTypes, level);
      auto rhsCheck = _args[1]->checkResultType(storedTypes, level);
      
      if (lhsCheck.result != ExpressionCheckResult::Valid || rhsCheck.result != ExpressionCheckResult::Valid) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};
      }

      if (lhsCheck.resultType.getTypeID() != TypeIdentifier::Integer || rhsCheck.resultType.getTypeID() != TypeIdentifier::Integer) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};  
      }

      return ExpressionCheckResult{ExpressionCheckResult::Valid, Type(TypeIdentifier::Integer)};
    }

    case If: {
      auto lhsCheck = _args[1]->checkResultType(storedTypes, level);
      auto rhsCheck = _args[2]->checkResultType(storedTypes, level);

      if (lhsCheck.result != ExpressionCheckResult::Valid || rhsCheck.result != ExpressionCheckResult::Valid) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};
      }

      if (lhsCheck.resultType.getTypeID() != rhsCheck.resultType.getTypeID()) {
        return ExpressionCheckResult{ExpressionCheckResult::Invalid, Type(TypeIdentifier::Integer)};  
      }

      return ExpressionCheckResult{ExpressionCheckResult::Valid, lhsCheck.resultType};
    }

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
