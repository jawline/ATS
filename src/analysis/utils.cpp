#include "utils.h"

using namespace JIT;
using namespace JIT::Expressions;

bool AnalysisUtils::isAtomic(JIT::Expressions::SafeExpression const& expression) {
	return expression->getExpressionType() == ExpressionType::AtomType;
}