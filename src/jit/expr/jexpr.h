#ifndef _jexpr_DEF_H_
#define _jexpr_DEF_H_
#include <memory>
#include <vector>
#include "../jtype.h"
#include "../../utils/bytebuffer.h"

namespace JIT {
  namespace Expressions {
    enum ExpressionType {
        Atom,
        Stored,
        Add,
        Subtract,
        Multiply,
        Divide,
        If,
        FunctionCall
    };

    struct ExpressionCheckResult {
      enum { Valid, Invalid } result;
      Type resultType;
    };

    typedef std::shared_ptr<class Expression> SafeExpression;

    class Expression {
        private:
          ExpressionType _type;

          TypeIdentifier _atomType;
          int64_t _val;

          void* _callbackLocation;
          SafeExpression _callbackExpression;

          SafeExpression _entryRef;
          
          size_t _storedIndex;
          
          std::vector<SafeExpression> _args;
        public:
          Expression(int64_t val);
          Expression(bool val);
          Expression(ExpressionType type, size_t argNum);
          Expression(ExpressionType type, std::vector<SafeExpression> const& args);
          Expression(ExpressionType type, void* callback, std::vector<SafeExpression> const& args);
          void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList);
          void* getCallback() const;
          int getNumArgs() const;
          void updateCallback(void* newCallback, SafeExpression callbackExpression);

          void setEntry(SafeExpression stmt);

          ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, unsigned int level);
    };
  }
}

#endif //_jexpr_DEF_H_
