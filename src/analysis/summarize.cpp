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

JIT::Expressions::SafeExpression Summarize::doAnalysis(JIT::Expressions::SafeExpression expression) const {
	
	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(doAnalysis(expression->getArguments()[i]));
	}

	expression->setArguments(simplifiedArguments);

	if (canSummarize(expression)) {
		printf("I may be able to do something here\n");
	}

	return expression;
}