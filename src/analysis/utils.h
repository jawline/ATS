#ifndef _ANALYSIS_UTILS_DEF_H_
#define _ANALYSIS_UTILS_DEF_H_
#include "../jit/expr/jexpr.h"

class AnalysisUtils {
public:
	static bool isAtomic(JIT::Expressions::SafeExpression const& expr);
};

#endif //_ANALYSIS_UTILS_DEF_H_