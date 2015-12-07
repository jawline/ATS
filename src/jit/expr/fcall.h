#ifndef _EXPR_FCALL_DEF_H_
#define _EXPR_FCALL_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class FCall: public Expression {
		private:
			bool isRecursion(std::vector<Type> const& storedTypes, std::vector<Type> const& argTypes, std::vector<SafeExpression> const& potentiallyCalledFunctions);
        public:
        	FCall(std::vector<SafeExpression> const& args);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
		};
	}
}

#endif