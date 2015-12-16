#ifndef _ANALYSIS_DEF_H_
#define _ANALYSIS_DEF_H_
#include "../jit/expr/jexpr.h"

class Analysis {
private:
public:
	virtual JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const = 0;
};

#endif