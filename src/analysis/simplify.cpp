#include "simplify.h"

using namespace JIT;
using namespace JIT::Expressions;

SafeExpression Simplifier::simplify(SafeExpression expression) const {

	if (!knowHowToSimplify(expression)) {
		printf("Simplifier doesn't know how\n");
		return expression;
	}

	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(simplify(expression->getArguments()[i]));
	}

	return remakeExpression(expression, simplifiedArguments);
}

bool Simplifier::knowHowToSimplify(SafeExpression expression) const {
	return expression->getExpressionType() == ExpressionType::AtomType;
}

SafeExpression Simplifier::remakeExpression(JIT::Expressions::SafeExpression expression, std::vector<JIT::Expressions::SafeExpression> const& remadeArguments) const {
	return expression;
}

bool Simplifier::isAtomic(SafeExpression expression) const {
	return expression->getExpressionType() == ExpressionType::AtomType;
}