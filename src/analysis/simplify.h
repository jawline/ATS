#ifndef _SIMPLIFY_TREE_DEF_H_
#define _SIMPLIFY_TREE_DEF_H_
#include "analysis.h"

class Simplifier: public Analysis {
private:
	bool isArith(JIT::Expressions::SafeExpression expression) const;
	bool knowHowToSimplify(JIT::Expressions::SafeExpression expression) const;
	JIT::Expressions::SafeExpression remakeExpression(JIT::Expressions::SafeExpression expression) const;

public:
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif //_SIMPLIFY_TREE_DEF_H_