#ifndef _EXPR_ATOM_DEF_H_
#define _EXPR_ATOM_DEF_H_
#include "jexpr.h"

namespace JIT {
	namespace Expressions {
		class Atom: public Expression {
		private:
			TypeIdentifier _atomType;
        	int64_t _val;
        public:
        	Atom(int64_t v);
        	Atom(bool v);

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList);

        	BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
		};
	}
}

#endif