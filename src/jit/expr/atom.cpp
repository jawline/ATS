#include "atom.h"
#include "../jhelper.h"

using namespace JIT::Expressions;

Atom::Atom(int64_t v) : Expression(ExpressionType::AtomType, std::vector<SafeExpression>()) {
	_atomType = TypeIdentifier::Integer;
	_val = v;
}

Atom::Atom(bool v) : Expression(ExpressionType::AtomType, std::vector<SafeExpression>()) {
	_atomType = TypeIdentifier::Boolean;
	_val = v;
}

void Atom::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList) {
     Helper::pushNumber(_val, buffer);
}

ExpressionCheckResult Atom::checkResultType(std::vector<Type> const& storedTypes) {
	return ExpressionCheckResult{ExpressionCheckResult::Valid, Type(_atomType)};
}