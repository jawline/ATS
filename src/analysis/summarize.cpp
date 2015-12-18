#include "summarize.h"
#include "utils.h"

using namespace JIT;
using namespace JIT::Expressions;

bool Summarize::canSummarize(SafeExpression expression) const {
	
	if (expression->getExpressionType() != ExpressionType::IfType) {
		return false;
	}

	if (!AnalysisUtils::isAtomic(expression->getArguments()[2])) {
		return false;
	}

	return true;
}

bool Summarize::willEvaluateToTrue(JIT::Expressions::SafeExpression expression) const {
	return AnalysisUtils::getValueActual(expression) != 0;
}

JIT::Expressions::SafeExpression Summarize::doConstantAnalysis(JIT::Expressions::SafeExpression expression) const {
	return expression;
}

JIT::Expressions::SafeExpression Summarize::doAnalysis(JIT::Expressions::SafeExpression expression) const {
	
	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(doAnalysis(expression->getArguments()[i]));
	}

	expression->setArguments(simplifiedArguments);

	//First see if it will always evaluate to true or false and always simplify to that path
	expression = doConstantAnalysis(expression);

	if (canSummarize(expression)) {
		printf("I may be able to do something here\n");
	}

	return expression;
}