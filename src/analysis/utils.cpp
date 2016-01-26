#include "utils.h"
#include "../jit/expr/atom.h"

using namespace JIT;
using namespace JIT::Expressions;

bool AnalysisUtils::isAtomic(JIT::Expressions::SafeExpression const& expression) {
	return expression->getExpressionType() == ExpressionType::AtomType;
}

int64_t AnalysisUtils::getValueActual(JIT::Expressions::SafeExpression const& expr) {
	return ((Atom*) expr.get())->getValueActual();
}