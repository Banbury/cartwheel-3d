// Abstract class (interface)
// An observer can be notified by any observable
// Based on the java.util.Observer class

#pragma once 

#include <Utils/UtilsDll.h>

class Observable;
class UTILS_DECLSPEC Observer {

public:

	Observer() {}
	virtual ~Observer() {}

	virtual void update( void* data = NULL ) = 0;

};

UTILS_TEMPLATE( std::vector< Observer* > )

