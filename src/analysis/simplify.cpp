#include "simplify.h"

using namespace JIT;
using namespace JIT::Expressions;

SafeExpression Simplifier::simplify(SafeExpression expression) {
}

bool Simplifier::isAtomic(SafeExpression expression) {
	return expression->getExpressionType() == ExpressionType::AtomType;
}
