#ifndef _COREASM_PARSER_DEF_H_
#define _COREASM_PARSER_DEF_H_
#include "../jit/jfunction.h"
#include "../analysis/analysis.h"
#include "tokens.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace Assembler {
	class Parser {
	private:
		Tokeniser _tokeniser;
		map<string, JIT::SafeFunction> _functions;
		vector<pair<string, JIT::Expressions::SafeExpression>> _unresolved;

		SafeAnalysis _chainer;

		JIT::Expressions::SafeExpression parseAtom(char const*& input);
		JIT::Expressions::SafeExpression parseArg(char const*& input, std::vector<std::string> const& argList);
		JIT::Expressions::SafeExpression parseFunctionCall(char const*& input, std::vector<std::string> const& argList);
		JIT::Expressions::SafeExpression parseBlock(char const*& input, std::vector<std::string> const& argList);
		
		bool resolveAll();
		int getArg(std::string arg, std::vector<std::string> const& argList);
		bool parseFunctionArguments(char const*& input, std::vector<std::string>& argList);
		bool parseFunction(char const*& input, map<string, JIT::SafeFunction>& functionList);
		bool innerParse(char const*& input);
	public:
		Parser();
		~Parser();

		bool parse(char const* input);
	};
}

#endif //_COREASM_PARSER_DEF_H_
