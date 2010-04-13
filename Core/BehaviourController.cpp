#include "BehaviourController.h"
#include <MathLib/Trajectory.h>

BehaviourController::BehaviourController(Character* b, IKVMCController* llc, WorldOracle* w){
	this->bip = b;
	this->lowLCon = llc;
	this->wo = w;

	//we should estimate these from the character info...
	legLength = 1;
	ankleBaseHeight = 0.04;
	shouldPreventLegIntersections = true;

	desiredHeading = 0;
	ubSagittalLean = 0;
	ubCoronalLean = 0;
	ubTwist = 0;
	duckWalk = 0;
	duckWalk = 0;
	velDSagittal = 0;
	velDCoronal = 0;
	kneeBend = 0;
	coronalStepWidth = 0.1;

	stepTime = 0.6;
	stepHeight = 0;
}

BehaviourController::~BehaviourController(void){

}

/**
	ask for a heading...
*/
void BehaviourController::requestHeading(double v){
	desiredHeading = v;
}

void BehaviourController::setElbowAngles(double leftElbowAngle, double rightElbowAngle){
	double stanceElbowAngle = (lowLCon->stance == LEFT_STANCE)?(leftElbowAngle):(rightElbowAngle);
	double swingElbowAngle = (lowLCon->stance == LEFT_STANCE)?(rightElbowAngle):(leftElbowAngle);

	SimBiConState* curState = lowLCon->states[lowLCon->FSMStateIndex];
	Trajectory* tmpTraj = curState->getTrajectory("STANCE_Elbow");
	if (tmpTraj != NULL)
		tmpTraj->components[0]->offset = stanceElbowAngle;
	tmpTraj = curState->getTrajectory("SWING_Elbow");
	if (tmpTraj != NULL)
		tmpTraj->components[0]->offset = swingElbowAngle * -1;
}

void BehaviourController::setShoulderAngles(double leftTwist, double rightTwist, double leftAdduction, double rightAdduction, double leftSwing, double rightSwing){
	double stanceTwist = (lowLCon->stance == LEFT_STANCE)?(leftTwist):(rightTwist);
	double stanceAdd = (lowLCon->stance == LEFT_STANCE)?(leftAdduction):(rightAdduction);
	double stanceSwing = (lowLCon->stance == LEFT_STANCE)?(leftSwing):(rightSwing);

	double swingTwist = (lowLCon->stance == RIGHT_STANCE)?(leftTwist):(rightTwist);
	double swingAdd = (lowLCon->stance == RIGHT_STANCE)?(leftAdduction):(rightAdduction);
	double swingSwing = (lowLCon->stance == RIGHT_STANCE)?(leftSwing):(rightSwing);

	SimBiConState* curState = lowLCon->states[lowLCon->FSMStateIndex];
	Trajectory* tmpTraj = curState->getTrajectory("STANCE_Shoulder");
	if (tmpTraj != NULL){
		tmpTraj->components[0]->offset = stanceTwist;
		tmpTraj->components[1]->offset = stanceAdd;
		tmpTraj->components[2]->offset = stanceSwing;

	}
	tmpTraj = curState->getTrajectory("SWING_Shoulder");
	if (tmpTraj != NULL){
		tmpTraj->components[0]->offset = swingTwist;
		tmpTraj->components[1]->offset = swingAdd * -1;
		tmpTraj->components[2]->offset = swingSwing;
	}
}

/**
	a set of useful virtual functions, whose behavior can be overridden
*/
void BehaviourController::setUpperBodyPose(double leanSagittal, double leanCoronal, double twist){
	int sign = (lowLCon->stance == LEFT_STANCE)?(-1):(1);
	SimBiConState* curState = lowLCon->states[lowLCon->getFSMState()];
	Trajectory* tmpTraj = curState->getTrajectory("root");
	if (tmpTraj != NULL){
		tmpTraj->components[2]->offset = leanSagittal;
		tmpTraj->components[1]->offset = leanCoronal * sign;
		tmpTraj->components[0]->offset = twist * sign * 0;
	}
	tmpTraj = curState->getTrajectory("pelvis_lowerback");
	if (tmpTraj != NULL){
		tmpTraj->components[2]->offset = leanSagittal*1.5;
		tmpTraj->components[1]->offset = leanCoronal *1.5* sign;
		tmpTraj->components[0]->offset = twist * 1.5 * sign;
	}
	tmpTraj = curState->getTrajectory("lowerback_torso");
	if (tmpTraj != NULL){
		tmpTraj->components[2]->offset = leanSagittal * 2.5;
		tmpTraj->components[1]->offset = leanCoronal * 2.5* sign;
		tmpTraj->components[0]->offset = twist * sign;
	}
	tmpTraj = curState->getTrajectory("torso_head");
	if (tmpTraj != NULL){
		tmpTraj->components[2]->offset = leanSagittal * 3.0;
		tmpTraj->components[1]->offset = leanCoronal * 1 * sign;
		tmpTraj->components[0]->offset = twist * 3.0 * sign;
	}
}

void BehaviourController::setKneeBend(double v, bool swingAlso){
	SimBiConState* curState = lowLCon->states[lowLCon->FSMStateIndex];
	Trajectory* tmpTraj = curState->getTrajectory("root");

	tmpTraj = curState->getTrajectory("STANCE_Knee");
	tmpTraj->components[0]->offset = v;

	if (swingAlso == true){
		tmpTraj = curState->getTrajectory("SWING_Knee");
		tmpTraj->components[0]->offset = v;
	}
}

void BehaviourController::setDuckWalkDegree(double v){
	boundToRange(&v, -2, 2);
	Vector3d rotAxis = Vector3d(-1,0,0).rotate(v, Vector3d(0,1,0));
	lowLCon->swingLegPlaneOfRotation = rotAxis;
}

void BehaviourController::requestVelocities(double velDS, double velDC){
	velDSagittal = velDS;
	velDCoronal = velDC;
}

/**
	sets a bunch of parameters to some default initial value
*/
void BehaviourController::initializeDefaultParameters(){
	lowLCon->updateDAndV();
	desiredHeading = 0;
}

void BehaviourController::setVelocities(double velDS, double velDC){
	lowLCon->velDSagittal = velDS;
	lowLCon->velDCoronal = velDC;
}

void BehaviourController::setDesiredHeading(double v){
	lowLCon->desiredHeading = v;
}

void BehaviourController::requestCoronalStepWidth(double corSW) {
	coronalStepWidth = corSW;
}

void BehaviourController::adjustStepHeight(){
	lowLCon->unplannedForHeight = 0;
	if (wo != NULL)
		//the trajectory of the foot was generated without taking the environment into account, so check to see if there are any un-planned bumps (at somepoint in the near future)
		lowLCon->unplannedForHeight = wo->getWorldHeightAt(lowLCon->swingFoot->state.position + lowLCon->swingFoot->state.velocity * 0.1) * 1.5;

	//if the foot is high enough, we shouldn't do much about it... also, if we're close to the start or end of the
	//walk cycle, we don't need to do anything... the thing below is a quadratic that is 1 at 0.5, 0 at 0 and 1...
	double panicIntensity = -4 * lowLCon->phi * lowLCon->phi + 4 * lowLCon->phi;
	panicIntensity *= getPanicLevel();
	lowLCon->panicHeight = panicIntensity * 0.05;
}

/**
	this method gets called at every simulation time step
*/
void BehaviourController::simStepPlan(double dt){
	lowLCon->updateSwingAndStanceReferences();
	if (lowLCon->phi <= 0.01)
		swingFootStartPos = lowLCon->swingFoot->getWorldCoordinates(bip->joints[lowLCon->swingAnkleIndex]->cJPos);

	//compute desired swing foot location...
	setDesiredSwingFootLocation();

	//set some of these settings
	setUpperBodyPose(ubSagittalLean, ubCoronalLean, ubTwist);
	setKneeBend(kneeBend);
	setDuckWalkDegree((lowLCon->stance == LEFT_STANCE)?(-duckWalk):(duckWalk));
	setDesiredHeading(desiredHeading);
	setVelocities(velDSagittal, velDCoronal);

	//adjust for panic mode or unplanned terrain...
	adjustStepHeight();

	//and see if we're really in trouble...
//	if (shouldAbort()) onAbort();
}

void BehaviourController::requestStepTime(double stepTime){
	this->stepTime = stepTime;
}

void BehaviourController::requestStepHeight(double stepHeight){
	this->stepHeight = stepHeight;
}

/**
	this method gets called every time the controller transitions to a new state
*/
void BehaviourController::conTransitionPlan(){
	lowLCon->updateSwingAndStanceReferences();
	lowLCon->updateDAndV();
	lowLCon->states[0]->stateTime = stepTime;

	lowLCon->updateSwingAndStanceReferences();
	swingFootStartPos = lowLCon->swingFoot->getCMPosition();

	//now prepare the step information for the following step:
	lowLCon->swingFootHeightTrajectory.clear();
	lowLCon->swingFootTrajectoryCoronal.clear();
	lowLCon->swingFootTrajectorySagittal.clear();

	lowLCon->swingFootHeightTrajectory.addKnot(0, ankleBaseHeight);
	lowLCon->swingFootHeightTrajectory.addKnot(0.5, ankleBaseHeight + 0.01 + 0.1 + 0 + stepHeight);
	lowLCon->swingFootHeightTrajectory.addKnot(1, ankleBaseHeight + 0.01);

	lowLCon->swingFootTrajectoryCoronal.addKnot(0,0);
	lowLCon->swingFootTrajectoryCoronal.addKnot(1,0);

	lowLCon->swingFootTrajectorySagittal.addKnot(0,0);
	lowLCon->swingFootTrajectorySagittal.addKnot(1,0);
}

/**
	returns a panic level which is 0 if val is between minG and maxG, 1 if it's
	smaller than minB or larger than maxB, and linearly interpolated 
*/
double getValueInFuzzyRange(double val, double minB, double minG, double maxG, double maxB){
	if (val <= minB || val >= maxB)
		return 1;
	if (val >= minG && val <= maxG)
		return 0;
	if (val > minB && val < minG)
		return (minG - val) / (minG - minB);
	if (val > maxG && val < maxB)
		return (val - maxG) / (maxB - maxG);
	//the input was probably wrong, so return panic...
	return 1;
}


/**
	this method determines the degree to which the character should be panicking
*/
double BehaviourController::getPanicLevel(){
	//the estimate of the panic is given, roughly speaking by the difference between the desired and actual velocities
	double panicEstimate = 0;
	panicEstimate += getValueInFuzzyRange(lowLCon->v.z, lowLCon->velDSagittal-0.4, lowLCon->velDSagittal-0.3, lowLCon->velDSagittal+0.3, lowLCon->velDSagittal+0.4);
	panicEstimate += getValueInFuzzyRange(lowLCon->v.x, lowLCon->velDCoronal-0.3, lowLCon->velDCoronal-0.2, lowLCon->velDCoronal+0.2, lowLCon->velDCoronal+0.3);
//	boundToRange(&panicEstimate, 0, 1);
	return panicEstimate/2;
}

/**
	this method determines if the character should aim to abort the given plan, and do something else instead (like maybe transition to the
	next state of the FSM early).
*/
bool BehaviourController::shouldAbort(){
	Vector3d step(lowLCon->getStanceFootPos(), lowLCon->getSwingFootPos());
	step = lowLCon->getCharacterFrame().inverseRotate(step);

	//TODO: MIGHT NEED TO MAKE THESE CONSTANTS A FUNCTION OF THE LEG LENGTH AT SOME POINT!!!!!
	if ((step.z > 0.6 && lowLCon->v.z > 0.2) || (step.z < -0.55 && lowLCon->v.z < -0.2))
		return true;
	
	if (lowLCon->stance == LEFT_STANCE){
		if ((step.x < -0.45 && lowLCon->v.x < -0.2) || (step.x > 0.35 && lowLCon->v.x > 0.2))
			return true;
	}else{
		if ((step.x > 0.45 && lowLCon->v.x > 0.2) || (step.x < -0.35 && lowLCon->v.x < -0.2))
			return true;
	}

	return false;
}

/**
	this method is used to indicate what the behaviour of the character should be, once it decides to abort its plan.
*/
void BehaviourController::onAbort(){
	//force a premature switch to the next controller state
	if (lowLCon->phi > 0.2){
		lowLCon->phi = 1;
		tprintf("PANIC!!!\n");
	}
//	Globals::animationRunning = false;
//	return;
}

/**
	modify the coronal location of the step so that the desired step width results.
*/
double BehaviourController::adjustCoronalStepLocation(double IPPrediction){
	//nothing to do if it's the default value...
	if (coronalStepWidth < 0.01)
		return IPPrediction;

	double stepWidth = coronalStepWidth / 2;
	stepWidth = (lowLCon->stance == LEFT_STANCE)?(-stepWidth):(stepWidth);

	//now for the step in the coronal direction - figure out if the character is still doing well - panic = 0 is good, panic = 1 is bad...
	double panicLevel = 1;
	if (lowLCon->stance == LEFT_STANCE){
		panicLevel = getValueInFuzzyRange(lowLCon->d.x, 1.15 * stepWidth, 0.5 * stepWidth, 0.25 * stepWidth, -0.25 * stepWidth);
		panicLevel += getValueInFuzzyRange(lowLCon->v.x, 2*stepWidth, stepWidth, -stepWidth, -stepWidth*1.5);
	}
	else{
		panicLevel = getValueInFuzzyRange(lowLCon->d.x, -0.25 * stepWidth, 0.25 * stepWidth, 0.5 * stepWidth, 1.15 * stepWidth);
		panicLevel += getValueInFuzzyRange(lowLCon->v.x, -stepWidth*1.5, -stepWidth, stepWidth, 2*stepWidth);
	}
	boundToRange(&panicLevel, 0, 1);
	Trajectory1d offsetMultiplier;
	offsetMultiplier.addKnot(0.05, 0); offsetMultiplier.addKnot(0.075, 1/2.0);
	double offset = stepWidth * offsetMultiplier.evaluate_linear(fabs(stepWidth));
//	if (IPPrediction * stepWidth < 0) offset = 0;
	//if it's doing well, use the desired step width...
	IPPrediction = panicLevel * (IPPrediction + offset) + (1-panicLevel) * stepWidth;
	lowLCon->comOffsetCoronal = (1-panicLevel) * stepWidth;

//	if (panicLevel >= 1)
//		tprintf("panic level: %lf; d.x = %lf\n", panicLevel, lowLCon->d.x);

	return IPPrediction;
}

void BehaviourController::requestUpperBodyPose(double leanS, double leanC, double twist){
	this->ubSagittalLean = leanS;
	this->ubCoronalLean = leanC;
	this->ubTwist = twist;
}

void BehaviourController::requestKneeBend(double kb){
	this->kneeBend = kb;
}

void BehaviourController::requestDuckFootedness(double df){
	this->duckWalk = df;
}

/**
	determines weather a leg crossing is bound to happen or not, given the predicted final desired position	of the swing foot.
	The suggested via point is expressed in the character frame, relative to the COM position...The via point is only suggested
	if an intersection is detected.
*/
bool BehaviourController::detectPossibleLegCrossing(const Vector3d& swingFootPos, Vector3d* viaPoint){
	//first, compute the world coords of the swing foot pos, since this is in the char. frame 
	Point3d desSwingFootPos = lowLCon->characterFrame.rotate(swingFootPos) + lowLCon->comPosition;
	//now, this is the segment that starts at the current swing foot pos and ends at the final
	//swing foot position

	Segment swingFootTraj(lowLCon->swingFoot->state.position, desSwingFootPos); swingFootTraj.a.y = 0; swingFootTraj.b.y = 0;
	
	//and now compute the segment originating at the stance foot that we don't want the swing foot trajectory to pass...
	Vector3d segDir = Vector3d(100, 0, 0);
	if (lowLCon->stance == RIGHT_STANCE) segDir.x = -segDir.x;
	segDir = lowLCon->stanceFoot->getWorldCoordinates(segDir); segDir.y = 0;
	Segment stanceFootSafety(lowLCon->stanceFoot->state.position, lowLCon->stanceFoot->state.position + segDir);
	stanceFootSafety.a.y = 0; stanceFootSafety.b.y = 0;

	//now check to see if the two segments intersect...
	Segment intersect;
	stanceFootSafety.getShortestSegmentTo(swingFootTraj, &intersect);

/*
	predSwingFootPosDebug = desSwingFootPos;predSwingFootPosDebug.y = 0;
	swingSegmentDebug = swingFootTraj;
	crossSegmentDebug = stanceFootSafety;
	viaPointSuggestedDebug.setValues(0,-100,0);
*/

	//now, if this is too small, then it means the swing leg will cross the stance leg...
	double safeDist = 0.02;
	if (Vector3d(intersect.a, intersect.b).length() < safeDist){
		if (viaPoint != NULL){
			*viaPoint = lowLCon->stanceFoot->state.position + segDir.unit() * -0.05;
			(*viaPoint) -= Vector3d(lowLCon->comPosition);
			*viaPoint = lowLCon->characterFrame.inverseRotate(*viaPoint);
			viaPoint->y = 0;
/*
			viaPointSuggestedDebug = lowLCon->characterFrame.rotate(*viaPoint) + lowLCon->comPosition;
			viaPointSuggestedDebug.y = 0;
*/
		}
		return true;
	}
	
	return false;
}

/**
	determine the estimate desired location of the swing foot, given the etimated position of the COM, and the phase
*/
Vector3d BehaviourController::computeSwingFootLocationEstimate(const Point3d& comPos, double phase){
	Vector3d step = lowLCon->computeIPStepLocation();

	//applying the IP prediction would make the character stop, so take a smaller step if you want it to walk faster, or larger
	//if you want it to go backwards
	step.z -= lowLCon->velDSagittal / 20;
	//and adjust the stepping in the coronal plane in order to account for desired step width...
	step.x = adjustCoronalStepLocation(step.x);

	boundToRange(&step.z, -0.4 * legLength, 0.4 * legLength);
	boundToRange(&step.x, -0.4 * legLength, 0.4 * legLength);

	Vector3d result;
	Vector3d initialStep(comPos, swingFootStartPos);
	initialStep = lowLCon->characterFrame.inverseRotate(initialStep);
	//when phi is small, we want to be much closer to where the initial step is - so compute this quantity in character-relative coordinates
	//now interpolate between this position and initial foot position - but provide two estimates in order to provide some gradient information
	double t = (1-phase);
	t = t * t;
	boundToRange(&t, 0, 1);

	Vector3d suggestedViaPoint;
	alternateFootTraj.clear();
	bool needToStepAroundStanceAnkle = false;
	if (phase < 0.8 && shouldPreventLegIntersections && getPanicLevel() < 0.5)
		needToStepAroundStanceAnkle = detectPossibleLegCrossing(step, &suggestedViaPoint);
	if (needToStepAroundStanceAnkle){
		//use the via point...
		Vector3d currentSwingStepPos(comPos, lowLCon->swingFoot->state.position);
		currentSwingStepPos = lowLCon->characterFrame.inverseRotate(initialStep);currentSwingStepPos.y = 0;		
		//compute the phase for the via point based on: d1/d2 = 1-x / x-phase, where d1 is the length of the vector from
		//the via point to the final location, and d2 is the length of the vector from the swing foot pos to the via point...
		double d1 = (step - suggestedViaPoint).length(); double d2 = (suggestedViaPoint - currentSwingStepPos).length(); if (d2 < 0.0001) d2 = d1 + 0.001;
		double c =  d1/d2;
		double viaPointPhase = (1+phase*c)/(1+c);
		//now create the trajectory...
		alternateFootTraj.addKnot(0, initialStep);
		alternateFootTraj.addKnot(viaPointPhase, suggestedViaPoint);
		alternateFootTraj.addKnot(1, step);
		//and see what the interpolated position is...
		result = alternateFootTraj.evaluate_catmull_rom(1-t);
//		tprintf("t: %lf\n", 1-t);
	}else{
		result.addScaledVector(step, 1-t);
		result.addScaledVector(initialStep, t);
	}

	result.y = 0;

/*
	suggestedFootPosDebug = result;
*/
	return result;
}

/**
	determines the desired swing foot location
*/
void BehaviourController::setDesiredSwingFootLocation(){
	Vector3d step = computeSwingFootLocationEstimate(lowLCon->comPosition, lowLCon->phi);
	lowLCon->swingFootTrajectoryCoronal.setKnotValue(0, step.x);
	lowLCon->swingFootTrajectorySagittal.setKnotValue(0, step.z);

	double dt = 0.001;
	step = computeSwingFootLocationEstimate(lowLCon->comPosition + lowLCon->comVelocity * dt, lowLCon->phi+dt);
	lowLCon->swingFootTrajectoryCoronal.setKnotValue(1, step.x);
	lowLCon->swingFootTrajectorySagittal.setKnotValue(1, step.z);
	//to give some gradient information, here's what the position will be a short time later...

	lowLCon->swingFootTrajectorySagittal.setKnotPosition(0, lowLCon->phi);
	lowLCon->swingFootTrajectorySagittal.setKnotPosition(1, lowLCon->phi+dt);

	lowLCon->swingFootTrajectoryCoronal.setKnotPosition(0, lowLCon->phi);
	lowLCon->swingFootTrajectoryCoronal.setKnotPosition(1, lowLCon->phi+dt);
}



