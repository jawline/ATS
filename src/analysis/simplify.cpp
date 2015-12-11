#include "simplify.h"
#include "../jit/expr/atom.h"

using namespace JIT;
using namespace JIT::Expressions;

SafeExpression Simplifier::simplify(SafeExpression expression) const {

	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(simplify(expression->getArguments()[i]));
	}

	expression->setArguments(simplifiedArguments);

	return remakeExpression(expression);
}

bool Simplifier::knowHowToSimplify(SafeExpression expression) const {
	return expression->getExpressionType() == ExpressionType::AtomType || expression->getExpressionType() == ExpressionType::Add;
}

SafeExpression Simplifier::remakeExpression(JIT::Expressions::SafeExpression expression) const {
	
	if (!knowHowToSimplify(expression)) {
		printf("Simplifier doesn't know how\n");
		return expression;
	}

	if (expression->getExpressionType() == ExpressionType::Add) {
		printf("Simplifying add\n");

		if (isAtomic(expression->getArguments()[0]) && isAtomic(expression->getArguments()[1])) {
			auto lhs = (Atom*) expression->getArguments()[0].get();
			auto rhs = (Atom*) expression->getArguments()[1].get();
			return SafeExpression(new Atom(lhs->getValueActual() + rhs->getValueActual()));
		} else {
			printf("Non atomic children, cannot simplify\n");
		}
	}

	return expression;
}

bool Simplifier::isAtomic(SafeExpression expression) const {
	return expression->getExpressionType() == ExpressionType::AtomType;
}