// An object that can be reference counted
#pragma once 

#include <Utils/UtilsDll.h>
#include <Utils/Utils.h>

class UTILS_DECLSPEC RefCountedObj {

private:

	int refCount;

public:

	RefCountedObj() : refCount(0) {}

	virtual ~RefCountedObj() {
		assert(refCount==0);
	}

	inline int ref() {
		refCount++;
		return refCount;
	}

	inline int unref() {
		if (refCount == 1) {
			refCount = 0;
			delete this;
			return 0;
		} 
		refCount--;
		return refCount;
	}

};