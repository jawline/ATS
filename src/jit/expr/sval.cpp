#include "sval.h"
#include "../jhelper.h"

using namespace JIT::Expressions;

SVal::SVal(unsigned int index) : Expression(Stored, std::vector<SafeExpression>()) {
  _index = index;
}

void SVal::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls) {
	Helper::pushArgumentTop(_index, buffer);
}

BaseCheckResult SVal::getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
	return BaseCheckResult{false, storedTypes[_index]};
}

ExpressionCheckResult SVal::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
	return ExpressionCheckResult{ExpressionCheckResult::Valid, storedTypes[_index]};
}