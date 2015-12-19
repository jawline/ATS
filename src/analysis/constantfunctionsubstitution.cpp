#include "constantfunctionsubstitution.h"
#include "utils.h"

using namespace JIT::Expressions;

JIT::Expressions::SafeExpression ConstantFunctionSubstitution::doAnalysis(JIT::Expressions::SafeExpression expression) const {

	if (expression->getExpressionType() == FunctionCall && AnalysisUtils::isAtomic(expression->getCallbackExpression())) {
		printf("Replacing a function by an atomic\n");
		return expression->getCallbackExpression();
	} else {
		std::vector<SafeExpression> simplifiedArguments;
		
		for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
			simplifiedArguments.push_back(doAnalysis(expression->getArguments()[i]));
		}

		expression->setArguments(simplifiedArguments);
	}

	return expression;
}