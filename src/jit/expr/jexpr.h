#ifndef _jexpr_DEF_H_
#define _jexpr_DEF_H_
#include <memory>
#include <vector>
#include "../jtype.h"
#include "../../utils/bytebuffer.h"

namespace JIT {
  namespace Expressions {
    enum ExpressionType {
        AtomType,
        Stored,
        Add,
        Subtract,
        Multiply,
        Divide,
        IfType,
        FunctionCall
    };

    struct ExpressionCheckResult {
      enum { Valid, Invalid, InfinateRecursion } result;
      Type resultType;
    };

    typedef std::shared_ptr<class Expression> SafeExpression;

    class Expression {
        protected:
          ExpressionType _type;

          void* _callbackLocation;
          SafeExpression _callbackExpression;

          SafeExpression _entryRef;
          
          std::vector<SafeExpression> _args;

        public:
          Expression(ExpressionType type, std::vector<SafeExpression> const& args);
          void setEntry(SafeExpression stmt);

          /**
           * Relates to the vmm location of the method being called
           */
          void* getCallback() const;
          void updateCallback(void* newCallback, SafeExpression callbackExpression);

          int getNumArgs() const;

          /**
           * Does the actual JITTIng
           */
          virtual void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Expression*, size_t>>& unresolvedList);

          /**
           * Type checker
           */
          virtual ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<SafeExpression>& potentiallyCalledFunctions);
    };
  }
}

#endif //_jexpr_DEF_H_