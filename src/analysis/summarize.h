#ifndef _SUMMARIZE_DEF_H_
#define _SUMMARIZE_DEF_H_
#include "analysis.h"

/**
 * Analysis attempts to summarize expressions
 */
class Summarize: public Analysis {
private:
	bool canSummarize(JIT::Expressions::SafeExpression expression) const;
	bool willEvaluateToTrue(JIT::Expressions::SafeExpression expression) const;

	JIT::Expressions::SafeExpression doConstantAnalysis(JIT::Expressions::SafeExpression expression) const;

public:
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif //_SUMMARIZE_DEF_H_