#ifndef _EXPR_ARITH_DEF_H_
#define _EXPR_ARITH_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class Arithmetic: public Expression {
		private:
        public:
        	Arithmetic(ExpressionType type, std::vector<SafeExpression> const& args);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
		};
	}
}

#endif