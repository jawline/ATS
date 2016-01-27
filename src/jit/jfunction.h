#ifndef _JIT_FUNCTION_DEF_H_
#define _JIT_FUNCTION_DEF_H_
#include "../utils/bytebuffer.h"
#include "../analysis/analysis.h"
#include "expr/jexpr.h"
#include "jhelper.h"

namespace JIT {
	class Function {
	private:
		unsigned int rewriteCallbacks();
		Expressions::SafeCompiledExpression _stmt;
		size_t _numArgs;

	public:
		Function(std::string const& name, Expressions::SafeExpression const& stmt, size_t numArgs);
		~Function();

		int64_t run(char const*& errorMessage);
		JFPTR getFnPtr();

		Expressions::SafeCompiledExpression getCompiledExpression() const;
		Expressions::SafeExpression expression() const;
		Expressions::ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<Expressions::MethodCall>& potentialMethods);

		size_t getNumArgs() const;
		void simplify(SafeAnalysis analysis);
	};
	
	typedef std::shared_ptr<Function> SafeFunction;
}

#endif //_JIT_FUNCTION_DEF_H_
