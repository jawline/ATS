#ifndef _EXPR_IF_DEF_H_
#define _EXPR_IF_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class If: public Expression {
		private:
        public:
        	If(std::vector<SafeExpression> const& args);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls);

        	BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
		};
	}
}

#endif