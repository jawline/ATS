#ifndef _CHAINER_DEF_H_
#define _CHAINER_DEF_H_
#include "analysis.h"
#include <vector>

class Chainer {
private:
	std::vector<SafeAnalysis> _analysis;
public:
	Chainer(std::vector<SafeAnalysis> const& analysis);
	JIT::Expressions::SafeExpression doAnalysis(JIT::Expressions::SafeExpression expression) const;
};

#endif //_CHAINER_DEF_H_