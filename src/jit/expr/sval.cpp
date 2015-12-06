#include "sval.h"
#include "../jhelper.h"

using namespace JIT::Expressions;

SVal::SVal(unsigned int index) : Expression(Stored, std::vector<SafeExpression>()) {
  _index = index;
}

void SVal::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
	Helper::pushArgumentTop(_index, buffer);
}

ExpressionCheckResult SVal::checkResultType(std::vector<Type> const& storedTypes) {
	return ExpressionCheckResult{ExpressionCheckResult::Valid, storedTypes[_index]};
}