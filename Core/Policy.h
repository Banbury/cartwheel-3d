#pragma once
#include <Core/CompositeController.h>

/**
	This class is used to define an interface for policies that are to be used with composite controllers. 
*/
class Policy{
protected:
	//this is the composite controller that this policy operates on
	CompositeController* con;
public:
	/**
		base constructor
	*/
	Policy(CompositeController* con){
		this->con = con;
	}
	
	/**
		destructor
	*/
	~Policy(void){
	}

	/**
		this method should be implemented by specific types of policies, and it should set the primary (and secondary+interpolation value if needed)
		controller to be run by the composite controller.
	*/
	virtual void applyAction() = 0;

};


