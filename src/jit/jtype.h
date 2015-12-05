#ifndef _JTYPE_DEF_H_
#define _JTYPE_DEF_H_

namespace JIT {
	
	enum TypeIdentifier {
		Integer,
		Boolean
	};

	class Type {
	private:
		TypeIdentifier _id;
	
	public:
		Type(TypeIdentifier);
		~Type();
		TypeIdentifier getTypeID() const;
	};
}

#endif