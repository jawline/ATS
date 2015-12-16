#include "parser.h"
#include <vector>
#include <stdio.h>

#include "analysis/chain.h"
#include "analysis/summarize.h"
#include "analysis/simplify.h"

#include "jit/expr/arith.h"
#include "jit/expr/atom.h"
#include "jit/expr/sval.h"
#include "jit/expr/fcall.h"
#include "jit/expr/if.h"
#include "jit/jcallbacks.h"

using namespace Assembler;
using namespace JIT;
using namespace Expressions;

Parser::Parser() {
	std::vector<SafeAnalysis> anls;
	anls.push_back(SafeAnalysis(new Simplifier()));
	anls.push_back(SafeAnalysis(new Summarize()));
	_chainer = SafeAnalysis(new Chainer(anls));
}

Parser::~Parser() {}

#define CHECK(x) if (x == nullptr) { return nullptr; }

SafeExpression Parser::parseAtom(char const*& input) {
	auto token = _tokeniser.nextToken(input);

	if (token.id() == NUM) {
		return SafeExpression(new Atom((int64_t)token.asInt()));
	} else if (token.id() == BOOL) {
		return SafeExpression(new Atom(token.asBool()));
	} else {
		return nullptr;
	}
}

bool Parser::resolveAll() {
	for (unsigned int i = 0; i < _unresolved.size(); i++) {
		if (_functions.find(_unresolved[i].first) != _functions.end()) {
			if (_unresolved[i].second->getNumArgs() != _functions[_unresolved[i].first]->getNumArgs()) {
				printf("The number of arguments supplied to the function call was %i but %li was expected\n",
					_unresolved[i].second->getNumArgs(),
					_functions[_unresolved[i].first]->getNumArgs());
				return false;
			}
			
			_unresolved[i].second->updateCallback((void*)_functions[_unresolved[i].first]->getFnPtr(), _functions[_unresolved[i].first]->expression());
		
			//Remove updated item and offset i to compensate
			_unresolved.erase(_unresolved.begin() + i);
			i--;
		}
	}
	return true;
}

SafeExpression Parser::parseFunctionCall(char const*& input, std::vector<std::string> const& argList) {

	//Get function call name
	Token next = _tokeniser.nextToken(input);

	Token nameToken = next;
	string name = next.asString();

	std::vector<SafeExpression> args;

	while (true) {
		next = _tokeniser.peekToken(input);
		
		if (next.id() == LPAREN) {
			SafeExpression arg = parseBlock(input, argList);
			CHECK(arg);
			args.push_back(arg);
		} else if (next.id() == NUM || next.id() == BOOL) {
			SafeExpression arg = parseAtom(input);
			CHECK(arg);
			args.push_back(arg);
		} else if (next.id() == ID) {
			
			//Peek the name
			next = _tokeniser.peekToken(input);
			SafeExpression arg = nullptr;

			if (getArg(next.asString(), argList) != -1) {
				arg = parseArg(input, argList);
			} else {
				printf("Cannot reach %s in this call on line %i (Perhaps it needs to be surrounded by parenthesis)\n", next.asString(), next.line());
			}

			CHECK(arg);
			args.push_back(arg);
		} else {
			break;
		}
	}
	
	ExpressionType type;
	void* callback = nullptr;
	bool arith = false;
	bool ifExpr = false;
	int numExpectedArgs;
	
	if (name.compare("+") == 0) {
		type = Add;
		arith = true;
		numExpectedArgs = 2;
	} else if (name.compare("-") == 0) {
		type = Subtract;
		arith = true;
		numExpectedArgs = 2;
	} else if (name.compare("*") == 0) {
		type = Multiply;
		arith = true;
		numExpectedArgs = 2;
	} else if (name.compare("/") == 0) {
		type = Divide;
		arith = true;
		numExpectedArgs = 2;
	} else if (name.compare("%") == 0) {
		type = Mod;
		arith = true;
		numExpectedArgs = 2;
	} else if (name.compare("if") == 0) {
		type = IfType;
		ifExpr = true;
		numExpectedArgs = 3;
	} else {
		type = FunctionCall;
		callback = nullptr;
		numExpectedArgs = -1;
	}
	
	if (numExpectedArgs != -1 && args.size() != numExpectedArgs) {
		printf("Expected %i args got %li near %s\n", numExpectedArgs, args.size(), nameToken.debugInfo().c_str());
		return nullptr;
	}

	SafeExpression result;
	if (arith) {
		result = SafeExpression(new Arithmetic(type, args));
	} else if (ifExpr) {
		result = SafeExpression(new If(args));
	} else {
		//Will be resolved later on in parsing with the actual fn it points to
		result = SafeExpression(new FCall(args));
	}

	if (type == FunctionCall && callback == nullptr) {
		_unresolved.push_back(pair<string, SafeExpression>(name, result));
	}

	return result;
}

int Parser::getArg(std::string arg, std::vector<std::string> const& argList) {
	for (unsigned int i = 0; i < argList.size(); i++) {
		if (argList[i].compare(arg) == 0) {
			return i;
		}
	}
	return -1;
}

SafeExpression Parser::parseArg(char const*& input, std::vector<std::string> const& argList) {
	Token next = _tokeniser.nextToken(input);
	return SafeExpression(new SVal(getArg(next.asString(), argList)));
}

SafeExpression Parser::parseBlock(char const*& input, std::vector<std::string> const& argList) {

	//Discard lparen
	Token next = _tokeniser.nextToken(input);
	next = _tokeniser.peekToken(input);
	SafeExpression result;

	if (next.id() == ID) {

		//Peek the name
		next = _tokeniser.peekToken(input);

		if (getArg(next.asString(), argList) != -1) {
			result = parseArg(input, argList);
		} else {
			result = parseFunctionCall(input, argList);
		}

	} else if (next.id() == NUM || next.id() == BOOL) {
		result = parseAtom(input);
	} else {
		printf("Expected ID or NUM or BOOL in block near %s\n", next.debugInfo().c_str());
		result = nullptr;
	}

	CHECK(result);

	next = _tokeniser.nextToken(input);

	if (next.id() != RPAREN) {
		printf("Expected RPAREN near %s\n", next.debugInfo().c_str());
		return nullptr;
	}

	return result;
}

bool Parser::parseFunctionArguments(char const*& input, std::vector<std::string>& argList) {
	
	Token next = _tokeniser.nextToken(input);
	
	if (next.id() != ID) {
		argList.push_back(next.asString());
	}
	
	argList.push_back(next.asString());
	
	next = _tokeniser.nextToken(input);
	
	if (next.id() == RPAREN) {
		return true;
	} else if (next.id() == COMMA) {
		return parseFunctionArguments(input, argList);
	} else {
		printf("Expected RPAREN or COMMA near %s\n", next.debugInfo().c_str());
		return false;
	}
}

bool Parser::parseFunction(char const*& input, std::map<std::string, SafeFunction>& functionList) {
	
	Token next = _tokeniser.nextToken(input);
	next = _tokeniser.nextToken(input);
	
	if (next.id() != ID) {
		printf("Expected function name not %s near %s\n", next.asString(), next.debugInfo().c_str());
		return false;
	}

	std::string name = next.asString();

	//Parse args if any are supplied
	std::vector<std::string> args;

	next = _tokeniser.peekToken(input);

	if (next.id() == LPAREN) {
		_tokeniser.nextToken(input);
		if (!parseFunctionArguments(input, args)) {
			return false;
		}
	}
		
	next = _tokeniser.nextToken(input);
	
	if (next.id() != ARROW) {
		printf("Expected arrow near %s\n", next.debugInfo().c_str());
		return false;
	}
	
	SafeExpression block = parseBlock(input, args);
	CHECK(block);
	functionList[name] = SafeFunction(new Function(name, _chainer->doAnalysis(block), args.size()));

	return true;
}

bool Parser::innerParse(char const*& input) {
	
	Token next = _tokeniser.peekToken(input);

	if (next.id() == TOKEN_EOF) {
		return true;
	}

	auto pcf = std::vector<MethodCall>();

	if (next.id() == LPAREN) {
		SafeExpression block = parseBlock(input, std::vector<std::string>());
		CHECK(block);
		
		Function fn = Function("anonymous", _chainer->doAnalysis(block), 0);
		
		if (!resolveAll()) {
			return false;
		}

		fn.rewriteCallbacks();

		std::vector<Type> argTypes;
		auto checkResult = fn.checkResultType(argTypes, pcf);

		if (checkResult.result != ExpressionCheckResult::Valid) {
			if (checkResult.result == ExpressionCheckResult::InfinateRecursion) {
				printf("This line could never feasibly terminate\n");
			} else {
				printf("Cannot run fn because of type error\n");
			}
		} else {
			
			if (checkResult.resultType.getTypeID() == TypeIdentifier::Integer) {
				printf("Line Result: %li\n", fn.run());
			} else if (checkResult.resultType.getTypeID() == TypeIdentifier::Boolean) {
				printf("Line Result: %s\n", fn.run() != 0 ? "true" : "false");
			} else {
				fn.run();
				printf("TODO: I don't know how to print this line");
			}
		}

	} else if (next.id() == QUERY) {
		auto typeOfQuery = _tokeniser.nextToken(input);
		next = _tokeniser.peekToken(input);

		if (next.id() == LPAREN) {
			SafeExpression block = parseBlock(input, std::vector<std::string>());
			CHECK(block);

			Function fn = Function("anonymous", block, 0);

			if (!resolveAll()) {
				return false;
			}

			fn.rewriteCallbacks();

			std::vector<Type> argTypes;
			auto checkResult = fn.checkResultType(argTypes, pcf);

			std::string name = typeOfQuery.asString();

			if (name.compare(":t") == 0) {
				printf("Type: %s\n", checkResult.resultType.toString().c_str());
			} else if (name.compare(":pc") == 0) {
				printf("Potentially Called: ");
				for (unsigned int i = 0; i < pcf.size(); i++) {
					if (i) {
						printf(", ");
					}
					printf("%s", pcf[i].stmt->getMarker().c_str());
				}
				printf("\n");
			}
		} else {
			printf("Error, expected LPAREN after :t\n");
			return nullptr;
		}

	} else if (next.id() == FUNCTION) {

		if (!parseFunction(input, _functions)) {
			return false;
		}

		if (!resolveAll()) {
			return false;
		}

		for (auto it = _functions.begin(); it != _functions.end(); it++) {
			it->second->rewriteCallbacks();
		}

	} else {
		printf("Expected LPAREN near %s\n", next.debugInfo().c_str());
		return false;
	}

	return innerParse(input);
}

bool Parser::parse(char const* input) {
	_tokeniser.resetLines();
	return innerParse(input);
}
