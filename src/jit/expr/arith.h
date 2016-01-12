#ifndef _EXPR_ARITH_DEF_H_
#define _EXPR_ARITH_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class Arithmetic: public Expression {
		private:
        public:
        	Arithmetic(ExpressionType type, std::vector<SafeExpression> const& args);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledStatement, size_t>>& unresolvedList, std::vector<SafeCompiledStatement> const& currentCalls);

        	BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
		};
	}
}

#endif