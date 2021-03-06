#include "simplify.h"
#include "../jit/expr/atom.h"
#include "utils.h"
#include <sstream>

using namespace JIT;
using namespace JIT::Expressions;

SafeExpression Simplifier::doAnalysis(SafeExpression expression) const {

	std::vector<SafeExpression> simplifiedArguments;
	
	for (unsigned int i = 0; i < expression->getArguments().size(); i++) {
		simplifiedArguments.push_back(doAnalysis(expression->getArguments()[i]));
	}

	expression->setArguments(simplifiedArguments);

	return remakeExpression(expression);
}

bool Simplifier::knowHowToSimplify(SafeExpression expression) const {
	return expression->getExpressionType() == ExpressionType::AtomType || expression->getExpressionType() == ExpressionType::Add || ExpressionType::Subtract || ExpressionType::Multiply || ExpressionType::Divide;
}

SafeExpression Simplifier::remakeExpression(JIT::Expressions::SafeExpression expression) const {
	
	if (!knowHowToSimplify(expression)) {
		return expression;
	}

	if (isArith(expression) && AnalysisUtils::isAtomic(expression->getArguments()[0]) && AnalysisUtils::isAtomic(expression->getArguments()[1])) {
		auto type = expression->getExpressionType();

		auto lhsVal = AnalysisUtils::getValueActual(expression->getArguments()[0]);
		auto rhsVal = AnalysisUtils::getValueActual(expression->getArguments()[1]);

		int64_t atomicValue;

		if (type == ExpressionType::Add) {
			atomicValue = lhsVal + rhsVal;
		} else if (type == ExpressionType::Subtract) {
			atomicValue = lhsVal - rhsVal;
		} else if (type == ExpressionType::Multiply) {
			atomicValue = lhsVal * rhsVal;
		} else if (type == ExpressionType::Divide) {
			atomicValue = lhsVal / rhsVal;
		} else if (type == ExpressionType::Mod) {
			atomicValue = lhsVal % rhsVal;
		} else {
			printf("ERROR SIMPLIFYING\n");
			return nullptr;
		}
		std::stringstream debug;
		debug << "Simplified arithmetic to " << atomicValue << " from (" << lhsVal << ", " << rhsVal << ")";
		ANote(debug.str());
		return SafeExpression(new Atom(atomicValue));
	}

	return expression;
}

bool Simplifier::isArith(JIT::Expressions::SafeExpression expression) const {
	auto type = expression->getExpressionType();
	return type == ExpressionType::Add || type == ExpressionType::Subtract || type == ExpressionType::Multiply || type == ExpressionType::Divide || type == ExpressionType::Mod;
}