#ifndef _JTYPE_DEF_H_
#define _JTYPE_DEF_H_
#include <string>

namespace JIT {
	
	enum TypeIdentifier {
		Integer,
		Boolean,
		Unknown
	};

	class Type {
	private:
		TypeIdentifier _id;
	
	public:
		Type(TypeIdentifier);
		~Type();
		TypeIdentifier getTypeID() const;

		bool equals(Type const& other) const { return _id == other._id; }

		std::string toString() {
			switch (_id) {
				case Integer:
					return "int";
				case Boolean:
					return "bool";
				case Unknown:
					return "unknown";
			}
		} const;
	};
}

#endif