#include "jstatement.h"
#include "jhelper.h"
#include "jcallbacks.h"

using namespace JIT;

Statement::Statement(int64_t val) {
  _type = Atom;
  _val = val;
}

Statement::Statement(StatementType type, size_t storedIndex) {
  _type = type;
  _storedIndex = storedIndex;
}

Statement::Statement(StatementType type, std::vector<SafeStatement> const& args) {
  _type = type;
  _args = args;
}

Statement::Statement(StatementType type, void* callback, std::vector<SafeStatement> const& args) {
  _type = type;
  _callback = callback;
  _args = args;
}

void Statement::updateCallback(void* callback) {
  _callback = callback;
}

void* Statement::getCallback() const {
  return _callback;
}

int Statement::getNumArgs() const {
  return _args.size();
}

void Statement::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Statement*, size_t>>& unresolvedList) {
  switch (_type) {
    case Atom:
      Helper::pushNumber(_val, buffer);
      break;
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
    case NativeCallback: {

      //TODO: SCARY! This will break with over 6 args, work out a nice way to do this
      //printf("TODO: NativeCallback arg set is broken with over 6 args\n");
      for (unsigned int i = 0; i < _args.size(); i++) {
        _args[i]->write(buffer, unresolvedList);
      }

      for (int i = _args.size(); i > 0; i--) {
        Helper::setArgumentStackTop(i - 1, buffer);
      }

      size_t addressStart = Helper::callFunction(_callback ? _callback : ((void*)Callbacks::unresolved), buffer);

      if (_callback == nullptr) {
        unresolvedList.push_back(std::pair<Statement*, size_t>(this, addressStart));
      }

      break;
    }
    default:
      printf("Could not JIT\n");
      break;
  }
}

StatementCheckResult Statement::checkResultType(std::vector<Type> const& storedTypes) {

  switch (_type) {
    case Atom:
      return StatementCheckResult{StatementCheckResult::Valid, Type(TypeIdentifier::Integer)};

    case Add:
    case Subtract:
    case Multiply:
    case Divide: {
      auto lhsCheck = _args[0]->checkResultType(storedTypes);
      auto rhsCheck = _args[1]->checkResultType(storedTypes);
      
      if (lhsCheck.result != StatementCheckResult::Valid || rhsCheck.result != StatementCheckResult::Valid) {
        return StatementCheckResult{StatementCheckResult::Invalid, Type(TypeIdentifier::Integer)};
      }

      if (lhsCheck.resultType.getTypeID() != TypeIdentifier::Integer || rhsCheck.resultType.getTypeID() != TypeIdentifier::Integer) {
        return StatementCheckResult{StatementCheckResult::Invalid, Type(TypeIdentifier::Integer)};  
      }

      return StatementCheckResult{StatementCheckResult::Valid, Type(TypeIdentifier::Integer)};
    }

    case Stored:
      return StatementCheckResult{StatementCheckResult::Valid, storedTypes[_storedIndex]};

    default:
      return StatementCheckResult{StatementCheckResult::Invalid, Type(TypeIdentifier::Boolean)};
  }
}
