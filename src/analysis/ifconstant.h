#ifndef _IF_SUMMARIZE_DEF_H_
#define _IF_SUMMARIZE_DEF_H_
#include "analysis.h"

/**
 * Analysis attempts to summarize expressions
 */
class IfConstant: public Analysis {
private:
	bool willEvaluateToTrue(JIT::Expressions::SafeExpression expression) const;
	JIT::Expressions::SafeExpression doConstantAnalysis(JIT::Expressions::SafeExpression expression) const;

public:
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif