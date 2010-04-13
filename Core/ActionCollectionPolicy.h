#pragma once

#include "policy.h"
#include <Core/CompositeController.h>

/**
	We define a composite action by providing the indices to the primary and secondary controllers and
	an interpolation parameter. In addition, we'll add a weight, which can be application-dependent.
*/
class CompositeAction{
public:
	int pIndex;
	int sIndex;
	double t;
	double w;

	CompositeAction(int p, int s, double t, double w = 0){
		this->pIndex = p;
		this->sIndex = s;
		this->t = t;
		this->w = w;
	}
};

class ControllerSwap{
public:
	int aIndex;
	int bIndex;
	ControllerSwap(int a, int b){
		this->aIndex = a;
		this->bIndex = b;
	}
};

/**
	This class is used to store a list of actions (triples of the form: primary controller index, secondary controller index, interpValue).
	This list is loaded from a file. Also loaded from the file are a set of mappings that indicate weather or not the controllers used should
	be stance dependent. For instance, one of the actions could be a turn to the left when the character is in a left stance, or turn to the
	right when the character is in a right stance.
*/
class ActionCollectionPolicy : public Policy{
friend class RLSimulationControlProcess;
	//this is the list of actions
	DynamicArray<CompositeAction> actions;
	//and this is the list of controllers that need to be swapped
	DynamicArray<ControllerSwap> conSwapList;
	//this is the selected action
	int actionIndex;
public:
	ActionCollectionPolicy(CompositeController* con);
	~ActionCollectionPolicy(void);

	inline void setActionIndex(int index){
		this->actionIndex = index;
	}

	/**
		this method is used to read the action list from a file. The method returns the number of actions read.
	*/
	int loadActionsFromFile(char* fName);

	/**
		this method is used to select the primary and secondary controllers to be run based on the user input.
	*/
	void applyAction();

	/**
		this method is used to select the primary and secondary controllers to be run based on the user input.
		Does not swap the action
	*/
	void applyActionNoSwap();
};


