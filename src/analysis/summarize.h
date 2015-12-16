#ifndef _SUMMARIZE_DEF_H_
#define _SUMMARIZE_DEF_H_
#include "analysis.h"

/**
 * Analysis attempts to summarize expressions
 */
class Summarize: public Analysis {
public:
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif //_SUMMARIZE_DEF_H_