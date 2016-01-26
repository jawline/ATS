#ifndef _jexpr_DEF_H_
#define _jexpr_DEF_H_
#include <memory>
#include <vector>
#include "../jhelper.h"
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


    class CompiledStatement;
    typedef std::shared_ptr<class CompiledStatement> SafeCompiledStatement;

    /**
     * Structure returned by type checker for method calls
     */
    struct MethodCall {
      SafeCompiledStatement cexpr;
      std::vector<Type> calledWith;
    };

    class Expression {
        protected:
          ExpressionType _type;

          SafeCompiledStatement _callbackEntry;

          /**
           * Marker used to identify the statement when printing (often "")
           */
          std::string _marker;
          
          std::vector<SafeExpression> _args;

        public:
          Expression(ExpressionType type, std::vector<SafeExpression> const& args);

          void setMarker(std::string const& marker);
          std::string getMarker() const;

          ExpressionType getExpressionType() const;
          std::vector<SafeExpression> getArguments() const;
          void setArguments(std::vector<SafeExpression> const& args);

          /**
           * Relates to the address of the method being called
           */
          void setCallbackEntry(SafeCompiledStatement const& stmt);
          SafeCompiledStatement getCallbackCompiledStatement() const;
          SafeExpression getCallbackExpression() const;

          int getNumArgs() const;

          /**
           * Does the actual JITTIng
           */
          virtual void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<SafeCompiledStatement, size_t>>& unresolvedList, std::vector<SafeCompiledStatement> const& currentCalls);

          /**
           * Type checker
           */
          virtual BaseCheckResult getBaseType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions) { return BaseCheckResult{false, Unknown}; }
          virtual ExpressionCheckResult checkResultType(std::vector<Type> const& storedTypes, std::vector<MethodCall>& potentiallyCalledFunctions);
    };

    class CompiledStatement {
      private:
        SafeExpression _expr;

        std::vector<std::pair<SafeCompiledStatement, size_t>> _unresolvedCallList;

        JFPTR _cachedCallback;
        size_t _fnSize;
        size_t _numArgs;

        void prepare(size_t numArgs, std::vector<SafeCompiledStatement> const& currentCalls);

      public:
          CompiledStatement(SafeExpression expression, size_t nArgs);
          virtual ~CompiledStatement();

          void setExpression(SafeExpression const& expression);
          SafeExpression getExpression() const;

          JFPTR getCompiled(std::vector<SafeCompiledStatement> const& currentCalls);

          //This method links one call into another then mutual recursion means that memory locations are not both known on write
          void rewriteCallbacks();
    };
  }
}

#endif //_jexpr_DEF_H_
