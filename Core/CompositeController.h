#pragma once
#include <Core/Controller.h>
#include <Core/SimBiController.h>
#include <Physics/ContactPoint.h>


/**
	This structure is used to store the state of a simbicon controller. It can be used to save/load a controller's
	states, where the state here does not refer to the states in the Finite State Machine. The state refers to the
	phase in the current FSM state, and also the stance.
*/
typedef struct {
	int primaryControllerIndex;
	int secondaryControllerIndex;
	double interpValue;
	bool synchronizeControllers;
	DynamicArray<SimBiControllerState> controllerStates;
} CompositeControllerState;


/**
	This class is used as a collection of SimBiCon controllers. This class can be used to synchronize the controllers, interpolate the torques of any 
	two controllers, or use the results of any one controller. One assumption is made however: if the controllers are to be synchronized, then they must 
	all share the same structure - they must all have the same number of states, and each state must correspond to a similar sort of motion (for instance,
	state 0 should be for a left-stance action, and state 1 for a right-stance action, etc). 

	It is assumed that there exists some external policy that selects the controllers to be played.
*/
class CompositeController :	public Controller{
friend class RLSimulationControlProcess;
friend class TestControllerPlayer2;
friend class RL_Bip_GTL;
friend class RL_Bip_SSFW;
friend class RL_BigBird_HMV;
friend class RL_BigBird_GTP;
friend class BigBird_Knockdown;

friend class ControllerOptimizer;
protected:
	//this is a list of controllers that this composition framework is responsible for
	DynamicArray<SimBiController*> controllers;
	//there is always one main controller that is active. This controller decides when to switch controller states, etc.
	int primaryControllerIndex;
	//and this is the secondary controller, that is used in the interpolation
	int secondaryControllerIndex;
	//and this is the interpolation value. If it is 1, then the primary controller has the say. If it is 0, then the secondary one has full control,
	//and anything in between results in interpolation at the torque level.
	double interpValue;
	//if this variable is set to true, then all the controllers in the collection will be synchronized (in terms of stance, phase, and FSM state switches).
	bool synchronizeControllers;
public:
	//pass as a parameter the character that the torques will be applied to, and also the file that has the list of
	//simbicon controllers to be used
	CompositeController(Character* ch, char* input);
	virtual ~CompositeController(void);


	/**
		This method is used to set the stance for all the controllers in the collection.
	*/
	void setStance(int newStance);

	/**
		This method is used to compute the torques that are to be applied at the next step.
	*/
	virtual void computeTorques(DynamicArray<ContactPoint> *cfs);

	/**
		This method is used to advance the controller in time. It takes in a list of the contact points, since they might be
		used to determine when to transition to a new state. This method returns -1 if the controller does not advance to a new state,
		or the index of the state that it transitions to otherwise.
	*/
	int advanceInTime(double dt, DynamicArray<ContactPoint> *cfs);

	/**
		This method returns the position of the CM of the stance foot, in world coordinates
	*/
	inline Point3d getStanceFootPos(){
		if (primaryControllerIndex >= 0 && primaryControllerIndex < (int)controllers.size())
			return controllers[primaryControllerIndex]->getStanceFootPos();
		return Point3d(0,0,0);
	}

	/**
		This method returns the position of the CM of the swing foot, in world coordinates
	*/
	inline Point3d getSwingFootPos(){
		if (primaryControllerIndex >= 0 && primaryControllerIndex < (int)controllers.size())
			return controllers[primaryControllerIndex]->getSwingFootPos();
		return Point3d(0,0,0);
	}

	inline Quaternion getCharacterFrame(){
		if (primaryControllerIndex >= 0 && primaryControllerIndex < (int)controllers.size())
			return controllers[primaryControllerIndex]->getCharacterFrame();
		return Quaternion();
	}

	/**
		This method is used to populate the structure that is passed in with the current state
		of the controller;
	*/
	void getControllerState(CompositeControllerState* cs);

	/**
		This method is used to populate the state of the controller, using the information passed in the
		structure
	*/
	void setControllerState(const CompositeControllerState &cs);

	/**
		this method returns the phase of the locomotion cycle
	*/
	inline double getLocomotionPhase(){
		if (primaryControllerIndex >= 0 && primaryControllerIndex < (int)controllers.size())
			return controllers[primaryControllerIndex]->getPhase();
		return 0;
	}

	/**
		this method returns the stance of the character
	*/
	inline int getStance(){
		if (primaryControllerIndex >= 0 && primaryControllerIndex < (int)controllers.size())
			return controllers[primaryControllerIndex]->stance;
		return LEFT_STANCE;
	}

	/**
		this method returns true if some part of the character other than the foot touches the ground, false otherwise
	*/
	inline bool isBodyInContactWithTheGround(){
		if (primaryControllerIndex >= 0 && primaryControllerIndex < (int)controllers.size())
			return controllers[primaryControllerIndex]->isBodyInContactWithTheGround();
		return true;
	}

	/**
		this method is used to return the number of controllers used in this composite controller
	*/
	inline int getControllerCount(){
		return controllers.size();
	}

	/**
		this method is used to set the control input for this composite controller
	*/
	inline void setControllerInput(int primaryConIndex, int secondaryConIndex, double interpValue){
		this->primaryControllerIndex = primaryConIndex;
		this->secondaryControllerIndex = secondaryConIndex;
		this->interpValue = interpValue;
	}

	/**
		this method returns a pointer to the nth simbicon controller in this composite controller framework.
	*/
	inline SimBiController* getController(int i){
		if (i < 0 || i >= (int)controllers.size())
			return NULL;
		return controllers[i];
	}

};



