#ifndef _JIT_FUNCTION_DEF_H_
#define _JIT_FUNCTION_DEF_H_
#include "../utils/bytebuffer.h"
#include "expr/jexpr.h"
#include "jhelper.h"

namespace JIT {
	class Function {
	private:
		void prepare(Expressions::SafeExpression const& stmt);
		JFPTR _storedFn;
		std::vector<std::pair<Expressions::Expression*, size_t>> _unresolvedCallList;
		size_t _fnSize;
		size_t _numArgs;
		Expressions::SafeExpression _stmt;

	public:
		Function(Expressions::SafeExpression const& stmt, size_t numArgs);
		~Function();

		void rewriteCallbacks();
		int64_t run();
		JFPTR getFnPtr();

		Expressions::SafeExpression expression() const;
		Expressions::ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes);

		size_t getNumArgs() const;
	};
	
	typedef std::shared_ptr<Function> SafeFunction;
}

#endif //_JIT_FUNCTION_DEF_H_
