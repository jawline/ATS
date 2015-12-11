#include "simplify.h"

using namespace JIT;
using namespace JIT::Expressions;

SafeExpression Simplifier::simplify(SafeExpression expression) const {
	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(simplify(expression->getArguments()[i]));
	}

	return remakeExpression(expression, simplifiedArguments);
}

SafeExpression Simplifier::remakeExpression(JIT::Expressions::SafeExpression expression, std::vector<JIT::Expressions::SafeExpression> const& remadeArguments) const {
	return expression;
}

bool Simplifier::isAtomic(SafeExpression expression) const {
	return expression->getExpressionType() == ExpressionType::AtomType;
}