#pragma once

#include <MathLib/Vector3d.h>
#include <Physics/ODEWorld.h>


#define LEFT_STANCE 0
#define RIGHT_STANCE 1

/**
	This class is used as a container for all the constants that are pertinent for the physical simulations, the controllers, etc.
*/

class SimGlobals {
public:
	//if this is set to true, then the heading of the character is controlled, otherwise it is free to do whatever it wants
	static int forceHeadingControl;
	//this variable is used to specify the desired heading of the character
	static double desiredHeading;
	//and this is the desired time interval for each simulation timestep (does not apply to animations that are played back).
	static double dt;

	static World* activeRbEngine;

	//temp..
	static double targetPos;
	static double targetPosX;
	static double targetPosZ;

	static double conInterpolationValue;
	static double bipDesiredVelocity;

	static int constraintSoftness;

	static int CGIterCount;
	static int linearizationCount;

	static double rootSagittal;
	static double stanceKnee;
/*
	static double rootLateral;
	static double swingHipSagittal;
	static double swingHipLateral;
	static double stanceAngleSagittal;
	static double stanceAngleLateral;

	static double style;

	static double VDelSagittal;
	static double stepHeight;
	static double stepTime;
	static double duckWalk;
	static double upperBodyTwist;

	static double coronalStepWidth;

	static double COMOffsetX;
	static double COMOffsetZ;
*/

	SimGlobals(void){
	}
	~SimGlobals(void){
	}

	inline static World* getRBEngine(){
		if (activeRbEngine == NULL)
			activeRbEngine = new ODEWorld();
		return activeRbEngine;
	}

};
