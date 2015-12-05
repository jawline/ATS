#include "jtype.h"

using namespace JIT;

Type::Type(TypeIdentifier id) {
	_id = id;
}

Type::~Type() {}

TypeIdentifier Type::getTypeID() const {
	return _id;
}
