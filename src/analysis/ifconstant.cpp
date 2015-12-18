#include "ifconstant.h"
#include "utils.h"

using namespace JIT;
using namespace JIT::Expressions;

bool IfConstant::willEvaluateToTrue(JIT::Expressions::SafeExpression expression) const {
	return AnalysisUtils::getValueActual(expression) != 0;
}

JIT::Expressions::SafeExpression IfConstant::doConstantAnalysis(JIT::Expressions::SafeExpression expression) const {

	//If the statement is an if which always evaluates to a constant then remove the if statement entirely
	if (expression->getExpressionType() != ExpressionType::IfType) {
		return expression;
	}

	if (!AnalysisUtils::isAtomic(expression->getArguments()[0])) {
		return expression;
	}

	printf("Simplifying an if which always evaluates to a constant\n");

	if (willEvaluateToTrue(expression->getArguments()[0])) {
		return expression->getArguments()[1];
	} else {
		return expression->getArguments()[2];
	}
}

JIT::Expressions::SafeExpression IfConstant::doAnalysis(JIT::Expressions::SafeExpression expression) const {
	
	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(doAnalysis(expression->getArguments()[i]));
	}

	expression->setArguments(simplifiedArguments);

	//First see if it will always evaluate to true or false and always simplify to that path
	return doConstantAnalysis(expression);
}