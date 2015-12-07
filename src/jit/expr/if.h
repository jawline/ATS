#ifndef _EXPR_IF_DEF_H_
#define _EXPR_IF_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class If: public Expression {
		private:
        public:
        	If(std::vector<SafeExpression> const& args);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList);

        	BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
		};
	}
}

#endif