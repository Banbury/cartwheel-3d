#pragma once
#include "policy.h"
#include <Core/CompositeController.h>

/**
	This class is a very simple example of a simple policy. The user can manipulate the desired speed of the character, and this policy is used
	to map that to a certain setting in the composite controller.
*/
class UserInteractionPolicy : public Policy{

public:
	UserInteractionPolicy(CompositeController* con);
	~UserInteractionPolicy(void);

	/**
		this method is used to select the primary and secondary controllers to be run based on the user input.
	*/
	void applyAction();
};


