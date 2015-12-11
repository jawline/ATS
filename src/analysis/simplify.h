#ifndef _SIMPLIFY_TREE_DEF_H_
#define _SIMPLIFY_TREE_DEF_H_
#include "../jit/expr/jexpr.h"

class Simplifier {
private:
	bool isAtomic(JIT::Expressions::SafeExpression expression);

public:
	JIT::Expressions::SafeExpression simplify(JIT::Expressions::SafeExpression expression);
};

#endif //_SIMPLIFY_TREE_DEF_H_