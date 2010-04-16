#include "UserInteractionPolicy.h"

/**
	constructor
*/
UserInteractionPolicy::UserInteractionPolicy(CompositeController* con) : Policy(con){

}

/**
	destructor
*/
UserInteractionPolicy::~UserInteractionPolicy(void){

}

/**
	this method is used to select the primary and secondary controllers to be run based on the user input.
*/
void UserInteractionPolicy::applyAction(){
	//this is the interpolation parameter - if it is 0, index1 has full control. If it is 1, index2 has full control.
	double t = SimGlobals::bipDesiredVelocity;

	if (t >= con->getControllerCount() -1)
		t = con->getControllerCount() - 1 - 0.00001;

	if (t<0)
		t = 0;


	//these are the indices of the two controllers that we're interpolating between
	int index1 = (int)t;
	int index2 = (int)t+1;
	t = t - (int)t;

	if (t < 0.5)
		con->setControllerInput(index1, index2, 1-t);
	else
		con->setControllerInput(index2, index1, t);

	//done...
}

