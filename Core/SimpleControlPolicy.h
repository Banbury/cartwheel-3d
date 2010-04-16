#pragma once
#include "policy.h"
#include <Core/CompositeController.h>

/**
	This class is a very simple example of a simple policy. This method holds a parameter that is used to define which controllers
	are active, and the weight that each one should get.
*/
class SimpleControlPolicy : public Policy{
public:
	//this is the variable that decides the outcome
	double val;
public:
	SimpleControlPolicy(CompositeController* con);
	~SimpleControlPolicy(void);

	/**
		this method is used to select the primary and secondary controllers to be run based on the user input.
	*/
	void applyAction();
};


