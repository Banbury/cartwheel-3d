#pragma once

#include <Core/Character.h>
#include <Core/SimBiController.h>

/**
	This class uses a balance controller that is switched to a walk controller when large enough disturbances are intorduced.
*/
class BipV3BalanceController{
private:
	//this is the active controller that is used for the character
	SimBiController* con;
	//these are the controllers used for balance and stepping
	SimBiController* steppingController;
	SimBiController* standingBalanceController;
	//and this is the controller that will be controlled
	Character* bip;
	World* world;

public:
	BipV3BalanceController(World* w, Character* b);
	~BipV3BalanceController();

	/**
		this method is used to set the offsets for any and all components of trajectories..
	*/
	void applyTrajectoryOffsets();

	bool runASimulationStep();

	SimBiController* getActiveController(){return con;}

};

