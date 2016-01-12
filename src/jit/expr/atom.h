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

        	int64_t getValueActual() const;

        	void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledStatement, size_t>>& unresolvedList, std::vector<SafeCompiledStatement> const& currentCalls);

        	BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
			ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
		};
	}
}

#endif