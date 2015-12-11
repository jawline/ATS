#ifndef _SIMPLIFY_TREE_DEF_H_
#define _SIMPLIFY_TREE_DEF_H_
#include "../jit/expr/jexpr.h"

class Simplifier {
private:
	bool isAtomic(JIT::Expressions::SafeExpression expression);
	JIT::Expressions::SafeExpression remakeExpression(JIT::Expressions::SafeExpression expression, std::vector<JIT::Expressions::SafeExpression> const& remadeArguments);

public:
	JIT::Expressions::SafeExpression simplify(JIT::Expressions::SafeExpression expression);
};

#endif //_SIMPLIFY_TREE_DEF_H_