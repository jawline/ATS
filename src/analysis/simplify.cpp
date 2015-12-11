#include "simplify.h"

using namespace JIT;
using namespace JIT::Expressions;

SafeExpression Simplifier::simplify(SafeExpression expression) {
	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(simplify(expression->getArguments()[i]));
	}

	return expression;
}

bool Simplifier::isAtomic(SafeExpression expression) {
	return expression->getExpressionType() == ExpressionType::AtomType;
}