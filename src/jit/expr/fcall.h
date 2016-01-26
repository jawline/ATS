#ifndef _EXPR_FCALL_DEF_H_
#define _EXPR_FCALL_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class FCall: public Expression {
		private:
			bool isRecursion(std::vector<Type> const& storedTypes, std::vector<Type> const& argTypes, std::vector<MethodCall> const& potentiallyCalledFunctions) const;
        public:
        	FCall(std::vector<SafeExpression> const& args);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledExpression, size_t>>& unresolvedList, std::vector<SafeCompiledExpression> const& currentCalls);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
			BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
		};
	}
}

#endif