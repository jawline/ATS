#ifndef _CHAINER_DEF_H_
#define _CHAINER_DEF_H_
#include "analysis.h"
#include <vector>

class Chainer {
private:
	std::vector<SafeAnalysis> _analysis;
public:

	Chainer(std::vector<SafeAnalysis> const& analysis);
};

#endif //_CHAINER_DEF_H_