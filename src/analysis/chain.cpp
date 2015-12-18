#include "chain.h"

Chainer::Chainer(std::vector<SafeAnalysis> const& analysis) {
	_analysis = analysis;
}

JIT::Expressions::SafeExpression Chainer::doAnalysis(JIT::Expressions::SafeExpression expression) const {

	for (unsigned int i = 0; i < _analysis.size(); i++) {
		expression = _analysis[i]->doAnalysis(expression);
	}

	return expression;
}