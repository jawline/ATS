#ifndef _SIMPLIFY_TREE_DEF_H_
#define _SIMPLIFY_TREE_DEF_H_
#include "../jit/expr/jexpr.h"

class Simplifier {
private:
	bool isAtomic(JIT::Expressions::SafeExpression expression) const;
	bool isArith(JIT::Expressions::SafeExpression expression) const;
	bool knowHowToSimplify(JIT::Expressions::SafeExpression expression) const;
	JIT::Expressions::SafeExpression remakeExpression(JIT::Expressions::SafeExpression expression) const;

public:
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif //_SIMPLIFY_TREE_DEF_H_