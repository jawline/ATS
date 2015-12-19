#ifndef _CONSTANT_FUNCTION_SUBSTITUTION_
#define _CONSTANT_FUNCTION_SUBSTITUTION_
#include "analysis.h"

class ConstantFunctionSubstitution: public Analysis {
public:
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif //_CONSTANT_FUNCTION_SUBSTITUTION_