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

void Atom::write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls) {
     Helper::pushNumber(_val, buffer);
}

BaseCheckResult Atom::getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
	return BaseCheckResult{false, Type(_atomType)};
}

ExpressionCheckResult Atom::checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) {
	return ExpressionCheckResult{ExpressionCheckResult::Valid, Type(_atomType)};
}

int64_t Atom::getValueActual() const {
	return _val;
}