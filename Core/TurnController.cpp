#include "TurnController.h"

TurnController::TurnController(Character* b, IKVMCController* llc, WorldOracle* w) : BehaviourController(b, llc, w){
	headingRequested = false;
	stillTurning = false;
	requestedHeadingValue = 0;
	turningBodyTwist = 0;
	initialTiming = 0;
}

/**
	ask for a heading...
*/
void TurnController::requestHeading(double v){
	stillTurning = false;
	headingRequested = true;
	requestedHeadingValue = v;

	//if the turn angle is pretty small, then just turn right away (or the old way... who knows).
	currentHeadingQ = bip->getHeading();
	finalHeadingQ.setToRotationQuaternion(v, PhysicsGlobals::up);
	tmpQ.setToProductOf(finalHeadingQ, currentHeadingQ, false, true);
	turnAngle = tmpQ.getRotationAngle(PhysicsGlobals::up);
	tprintf("turnAngle: %lf\n", turnAngle);

	initialTiming = stepTime;

	if (fabs(turnAngle) < 1.0)
		initiateTurn(v);
}

TurnController::~TurnController(void){

}

/**
	this method gets called at every simulation time step
*/
void TurnController::simStepPlan(double dt){
	BehaviourController::simStepPlan(dt);
	if (stillTurning == false)
		return;

	//this is this guy's equivalent of panic management...
	double vLength = lowLCon->v.length();
	if (vLength > 1.5*initialVelocity.length() && vLength > 1.5*desiredVelocity.length() && vLength > 0.5){
		lowLCon->states[lowLCon->FSMStateIndex]->stateTime *= 0.99;
//		tprintf("velocity is too large... changing transition time to: %lf\n", lowLCon->states[lowLCon->FSMStateIndex]->stateTime);
	}else
		lowLCon->states[lowLCon->FSMStateIndex]->stateTime = initialTiming;

	//2 rads per second...
	double turnRate = dt * 2;

	//TODO: get all those limits on the twisting deltas to be related to dt, so that we get uniform turning despite the
	//having different dts

	currentHeadingQ = bip->getHeading();
	currentDesiredHeadingQ.setToRotationQuaternion(turningDesiredHeading, PhysicsGlobals::up);
	finalHeadingQ.setToRotationQuaternion(finalHeading, PhysicsGlobals::up);
	//the *3 below is because the head rotates thrice more than the body, and we don't want it to go past the target...
	upperBodyTwistQ.setToRotationQuaternion(turningBodyTwist * 3, PhysicsGlobals::up);

	//this is the angle between the current heading and the final desired heading...
	tmpQ.setToProductOf(currentHeadingQ, finalHeadingQ, false, true);
	double curToFinal = tmpQ.getRotationAngle(PhysicsGlobals::up);
	//this is the difference between the current and final orientation, offset by the current turningBodyTwist - to know how much more we
	//should twist into the rotation...
	tmpQ2.setToProductOf(tmpQ, upperBodyTwistQ, false, true);
	double deltaTwist = tmpQ2.getRotationAngle(PhysicsGlobals::up);
	//this is the angle between the set desired heading and the final heading - adding this to the current desired heading would reach finalHeading in one go...
	tmpQ.setToProductOf(finalHeadingQ, currentDesiredHeadingQ, false, true);
	double desToFinal = tmpQ.getRotationAngle(PhysicsGlobals::up);

	//do we still need to increase the desired heading?
	if (fabs(desToFinal) > 0.01){
		boundToRange(&desToFinal, -turnRate, turnRate);
		turningDesiredHeading += desToFinal;
	}

	boundToRange(&deltaTwist, -turnRate*4, turnRate*4);
	turningBodyTwist += deltaTwist;
	boundToRange(&turningBodyTwist, -0.5, 0.5);
	double footAngleRelToBody = (lowLCon->stance == LEFT_STANCE)?(-duckWalk):(duckWalk);
	double t = (lowLCon->phi) - 0.2;
	boundToRange(&t, 0, 1);
	double twistCoef = 2*(desiredVelocity.length() + 0.5);
	boundToRange(&twistCoef, 0, 2.5);
	footAngleRelToBody = (1-t) * footAngleRelToBody + t * -turningBodyTwist*twistCoef;
	setDuckWalkDegree(footAngleRelToBody);
	setUpperBodyPose(ubSagittalLean + 0.01, ubCoronalLean - turningBodyTwist/10, turningBodyTwist);

	//are we there yet (or close enough)?
	if (fabs(curToFinal) < 0.2){
		tprintf("done turning!\n");
		turningBodyTwist = 0;
		desiredHeading = turningDesiredHeading = finalHeading;
		//reset everything...
		setUpperBodyPose(ubSagittalLean, ubCoronalLean, ubTwist);
		setDesiredHeading(desiredHeading);
		setDuckWalkDegree(duckWalk);
		setVelocities(velDSagittal, velDCoronal);
		lowLCon->states[lowLCon->FSMStateIndex]->stateTime = initialTiming;
		stillTurning = false;
	}else{
		//still turning... so we need to still specify the desired velocity, in character frame...
		t = fabs(curToFinal/turnAngle) - 0.3;
		boundToRange(&t, 0, 1);
		Vector3d vD = initialVelocity*t + desiredVelocity*(1-t);
		//Vector3d vD = desiredVelocity;
		vD = lowLCon->characterFrame.inverseRotate(vD);
		lowLCon->velDSagittal = vD.z;
		lowLCon->velDCoronal = vD.x;
	}

	setDesiredHeading(turningDesiredHeading);
}

/**
	this method gets called every time the controller transitions to a new state
*/
void TurnController::conTransitionPlan(){
	BehaviourController::conTransitionPlan();
	if (headingRequested)
		initiateTurn(requestedHeadingValue);
}

/**
	sets a bunch of parameters to some default initial value
*/
void TurnController::initializeDefaultParameters(){
	BehaviourController::initializeDefaultParameters();
}


/**
	commence turning...
*/
void TurnController::initiateTurn(double finalDHeading){
	if (stillTurning == false){
		turningBodyTwist = 0;
		turningDesiredHeading = bip->getHeadingAngle();;
	}

	headingRequested = false;
	stillTurning = true;

	currentHeadingQ = bip->getHeading();
	finalHeadingQ.setToRotationQuaternion(finalDHeading, PhysicsGlobals::up);
	tmpQ.setToProductOf(finalHeadingQ, currentHeadingQ, false, true);

	turnAngle = tmpQ.getRotationAngle(PhysicsGlobals::up);
	finalHeading = finalHeadingQ.getRotationAngle(PhysicsGlobals::up);
	initialHeading = currentHeadingQ.getRotationAngle(PhysicsGlobals::up);

	tprintf("turnAngle: %lf. InitialHeading: %lf. Final Heading: %lf\n", turnAngle, initialHeading, finalHeading);

	initialVelocity = bip->getCOMVelocity();
	double finalVDSagittal = velDSagittal;
	boundToRange(&finalVDSagittal, -0.5, 0.6);
	if (fabs(turnAngle) > 2.5)
		boundToRange(&finalVDSagittal, -0.2, 0.3);
	desiredVelocity = Vector3d(0,0,finalVDSagittal).rotate(finalHeading, Vector3d(0,1,0));

	if (((lowLCon->stance == LEFT_STANCE && turnAngle < -1.5) || (lowLCon->stance == RIGHT_STANCE && turnAngle > 1.5)) && finalVDSagittal >=0){
		tprintf("this is the bad side... try a smaller heading first...\n");
		if (lowLCon->stance == LEFT_STANCE)	initiateTurn(initialHeading - 1.4);
		else initiateTurn(initialHeading + 1.4);
		desiredVelocity /= 0.5;
		headingRequested = true;
		requestedHeadingValue = finalDHeading;
	}
}

/**
	this method determines the degree to which the character should be panicking
*/
double TurnController::getPanicLevel(){
	//don't panic during turning, if the vel's are not there yet... or at least, panic in a different way.
	if (stillTurning)
		return 0;

	return BehaviourController::getPanicLevel();
}

