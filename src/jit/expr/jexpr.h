#ifndef _jexpr_DEF_H_
#define _jexpr_DEF_H_
#include <memory>
#include <vector>
#include "../jtype.h"
#include "../../utils/bytebuffer.h"

namespace JIT {

    enum StatementType {
        Atom,
        Stored,
        Add,
        Subtract,
        Multiply,
        Divide,
        If,
        FunctionCall
    };

    struct StatementCheckResult {
      enum { Valid, Invalid } result;
      Type resultType;
    };

    typedef std::shared_ptr<class Statement> SafeStatement;

    class Statement {
        private:
          StatementType _type;

          TypeIdentifier _atomType;
          int64_t _val;

          void* _callbackLocation;
          SafeStatement _callbackStatement;

          SafeStatement _entryRef;
          
          size_t _storedIndex;
          
          std::vector<SafeStatement> _args;
        public:
          Statement(int64_t val);
          Statement(bool val);
          Statement(StatementType type, size_t argNum);
          Statement(StatementType type, std::vector<SafeStatement> const& args);
          Statement(StatementType type, void* callback, std::vector<SafeStatement> const& args);
          void write(Assembler::ByteBuffer& buffer, std::vector<std::pair<Statement*, size_t>>& unresolvedList);
          void* getCallback() const;
          int getNumArgs() const;
          void updateCallback(void* newCallback, SafeStatement callbackStatement);

          void setEntry(SafeStatement stmt);

          StatementCheckResult checkResultType(std::vector<Type> const& storedTypes, unsigned int level);
    };
}

#endif //_jexpr_DEF_H_
