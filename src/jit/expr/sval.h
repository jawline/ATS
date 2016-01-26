#ifndef _EXPR_SVAL_DEF_H_
#define _EXPR_SVAL_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class SVal: public Expression {
		private:
			unsigned int _index;

        public:
        	SVal(unsigned int index);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls);
        	
        	BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
		};
	}
}

#endif