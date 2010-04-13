#pragma once

#include <Core/BehaviourController.h>

/**
	A two-step, arbitrary velocity to arbitrary velocity, parameterizable rotation controller.
*/

class TurnController : public BehaviourController{
protected:
	double initialHeading;
	double finalHeading;
	double turnAngle;
	bool stillTurning;
	Vector3d desiredVelocity;
	Vector3d initialVelocity;
	//some quaternions that will be useful..
	Quaternion currentHeadingQ, currentDesiredHeadingQ, finalHeadingQ, upperBodyTwistQ, tmpQ, tmpQ2;

	bool headingRequested;
	double requestedHeadingValue;

	//keep a copy of the body twist and desired heading here, so that they can't be messed with somewhere else...
	double turningBodyTwist;
	double turningDesiredHeading;
	double initialTiming;

	/**
		commence the turn...
	*/
	void initiateTurn(double finalHeading);

public:
	TurnController(Character* b, IKVMCController* llc, WorldOracle* w = NULL);
	virtual ~TurnController(void);

	/**
		sets a bunch of parameters to some default initial value
	*/
	virtual void initializeDefaultParameters();

	/**
		this method gets called at every simulation time step
	*/
	virtual void simStepPlan(double dt);

	/**
		this method gets called every time the controller transitions to a new state
	*/
	virtual void conTransitionPlan();


	/**
		this method determines the degree to which the character should be panicking
	*/
	virtual double getPanicLevel();

	/**
		ask for a heading...
	*/
	virtual void requestHeading(double v);
};


