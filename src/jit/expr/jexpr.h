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
        Mod,
        IfType,
        FunctionCall
    };

    struct ExpressionCheckResult {
      enum { Valid, Invalid, InfinateRecursion } result;
      Type resultType;
    };

    struct BaseCheckResult {
      bool recursion;
      Type type;
    };

    typedef std::shared_ptr<class Expression> SafeExpression;

    struct MethodCall {
      SafeExpression stmt;
      std::vector<Type> calledWith;
    };

    class Expression {
        protected:
          ExpressionType _type;

          void* _callbackLocation;
          SafeExpression _callbackExpression;

          std::string _marker;
          
          std::vector<SafeExpression> _args;

        public:
          Expression(ExpressionType type, std::vector<SafeExpression> const& args);
          void setMarker(std::string const& marker);
          std::string getMarker() const;

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
          virtual BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) { return BaseCheckResult{false, Unknown}; }
          virtual ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
    };
  }
}

#endif //_jexpr_DEF_H_
