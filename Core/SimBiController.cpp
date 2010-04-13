/*
	Simbicon 1.5 Controller Editor Framework, 
	Copyright 2009 Stelian Coros, Philippe Beaudoin and Michiel van de Panne.
	All rights reserved. Web: www.cs.ubc.ca/~van/simbicon_cef

	This file is part of the Simbicon 1.5 Controller Editor Framework.

	Simbicon 1.5 Controller Editor Framework is free software: you can 
	redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Simbicon 1.5 Controller Editor Framework is distributed in the hope 
	that it will be useful, but WITHOUT ANY WARRANTY; without even the 
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Simbicon 1.5 Controller Editor Framework. 
	If not, see <http://www.gnu.org/licenses/>.
*/

#include "SimBiController.h"
#include <Utils/Utils.h>
#include "SimGlobals.h"
#include "ConUtils.h"

SimBiController::SimBiController(Character* b) : PoseController(b){
	if (b == NULL)
		throwError("Cannot create a SIMBICON controller if there is no associated biped!!");
	//characters controlled by a simbicon controller are assumed to have: 2 feet
	lFoot = b->getARBByName("lFoot");
	rFoot = b->getARBByName("rFoot");

	if (rFoot == NULL || lFoot == NULL){
		lFoot = b->getARBByName("lFoot2");
		rFoot = b->getARBByName("rFoot2");
	}

	if (rFoot == NULL || lFoot == NULL)
		throwError("The biped must have the rigid bodies lFoot and rFoot!");
	
	//and two hips connected to the root
	Joint* lHip = b->getJointByName("lHip");
	Joint* rHip = b->getJointByName("rHip");

	lHipIndex = b->getJointIndex("lHip");
	rHipIndex = b->getJointIndex("rHip");

	if (rFoot == NULL || lFoot == NULL)
		throwError("The biped must have the joints lHip and rHip!");

	root = b->getRoot();
	
	if (lHip->getParent() != rHip->getParent() || lHip->getParent() != root)
		throwError("The biped's hips must have a common parent, which should be the root of the figure!");

	setStance(LEFT_STANCE);
	phi = 0;
	desiredHeading = 0;

	setFSMStateTo(-1);

	stanceHipDamping = -1;
	stanceHipMaxVelocity = 4;
	rootPredictiveTorqueScale = 0;

	bodyTouchedTheGround = false;
	
	startingState = 0;
	startingStance = LEFT_STANCE;
	initialBipState[0] = '\0';
}

/**
	This method is used to set the current FSM state of the controller to that of the index that
	is passed in.
*/
void SimBiController::setFSMStateTo(int index){
	if (index<0)
		FSMStateIndex = -1;
	else if( (uint)index>=states.size() )
		FSMStateIndex = states.size()-1;
	else
		FSMStateIndex = index;
}

SimBiController::~SimBiController(void){
	for (uint i=0;i<states.size();i++)
		delete states[i];
}

/**
	This method is used to set the stance 
*/
void SimBiController::setStance(int newStance){
	stance = newStance;
	if (stance == LEFT_STANCE){
		stanceFoot = lFoot;
		swingFoot = rFoot;
		swingHipIndex = rHipIndex;
		stanceHipIndex = lHipIndex;
	}else{
		stanceFoot = rFoot;
		swingFoot = lFoot;
		swingHipIndex = lHipIndex;
		stanceHipIndex = rHipIndex;
	}
}

/**
	This method is used to populate the structure that is passed in with the current state
	of the controller;
*/
void SimBiController::getControllerState(SimBiControllerState* cs){
	cs->stance = this->stance;
	cs->phi = this->phi;
	cs->FSMStateIndex = this->FSMStateIndex;
	cs->bodyGroundContact = this->bodyTouchedTheGround;
}

/**
	This method is used to populate the state of the controller, using the information passed in the
	structure
*/
void SimBiController::setControllerState(const SimBiControllerState &cs){
	this->setStance(cs.stance);
	this->phi = cs.phi;
	this->setFSMStateTo(cs.FSMStateIndex);
	this->bodyTouchedTheGround = cs.bodyGroundContact;
}

/**
	This method should be called when the controller transitions to this state.
*/
void SimBiController::transitionToState(int stateIndex){
	setFSMStateTo(stateIndex);
	setStance(states[FSMStateIndex]->getStateStance(this->stance));
//	tprintf("Transition to state: %d (stance = %s) (phi = %lf)\n", stateIndex, (stance == LEFT_STANCE)?("left"):("right"), phi);
	//reset the phase...
	this->phi = 0;
}

/**
	This method returns the net force on the body rb, acting from the ground
*/
Vector3d SimBiController::getForceOn(RigidBody* rb, DynamicArray<ContactPoint> *cfs){
	Vector3d fNet = Vector3d();
	for (uint i=0;i<cfs->size();i++){
		if ((*cfs)[i].rb1 == rb)
			fNet += (*cfs)[i].f;
		if ((*cfs)[i].rb2 == rb)
			fNet -= (*cfs)[i].f;
	}
	return fNet;
}





/**
	check to see if rb is the same as whichBody or any of its children
*/
bool SimBiController::haveRelationBetween(RigidBody* rb, RigidBody* whichBody){
	//check against the feet
	if (rb == whichBody)
		return true;
	for (uint j=0;j<((ArticulatedRigidBody*)whichBody)->cJoints.size();j++)
		if (((ArticulatedRigidBody*)whichBody)->cJoints[j]->child == rb)
			return true;
	return false;
}

/**
	This method is used to determine if the rigid body that is passed in as a parameter is a
	part of a foot
*/
bool SimBiController::isFoot(RigidBody* rb){
	//check against the feet
	return haveRelationBetween(rb, lFoot) || haveRelationBetween(rb, rFoot);

	if (rb == lFoot || rb == rFoot)
		return true;
	//and against the toes
	for (uint j=0;j<((ArticulatedRigidBody*)lFoot)->cJoints.size();j++)
		if (((ArticulatedRigidBody*)lFoot)->cJoints[j]->child == rb)
			return true;
	for (uint j=0;j<((ArticulatedRigidBody*)rFoot)->cJoints.size();j++)
		if (((ArticulatedRigidBody*)rFoot)->cJoints[j]->child == rb)
			return true;

	return false;

}

/**
	This method returns true if the rigid body that is passed in as a parameter is a swing foot, false otherwise
*/
bool SimBiController::isSwingFoot(RigidBody* rb){
	//check against the feet
	if (rb == swingFoot)
		return true;
	//and against the toes
	for (uint j=0;j<((ArticulatedRigidBody*)swingFoot)->cJoints.size();j++)
		if (((ArticulatedRigidBody*)swingFoot)->cJoints[j]->child == rb)
			return true;
	return false;
}

/**
	This method returns true if the rigid body that is passed in as a parameter is a swing foot, false otherwise
*/
bool SimBiController::isStanceFoot(RigidBody* rb){
	//check against the feet
	if (rb == stanceFoot)
		return true;
	//and against the toes
	for (uint j=0;j<((ArticulatedRigidBody*)stanceFoot)->cJoints.size();j++)
		if (((ArticulatedRigidBody*)stanceFoot)->cJoints[j]->child == rb)
			return true;
	return false;
}

/**
	This method returns the net force on the body rb, acting from the ground
*/
Vector3d SimBiController::getForceOnFoot(RigidBody* foot, DynamicArray<ContactPoint> *cfs){
	Vector3d fNet = getForceOn(foot, cfs);

	//we will also look at all children of the foot that is passed in (to take care of toes).
	for (uint i=0;i<((ArticulatedRigidBody*)foot)->cJoints.size();i++){
		fNet += getForceOn(((ArticulatedRigidBody*)foot)->cJoints[i]->child, cfs);
	}
	return fNet;
}

/**
	This method is used to advance the controller in time. It takes in a list of the contact points, since they might be
	used to determine when to transition to a new state. This method returns -1 if the controller does not advance to a new state,
	or the index of the state that it transitions to otherwise.
*/
int SimBiController::advanceInTime(double dt, DynamicArray<ContactPoint> *cfs){
	if( FSMStateIndex < 0 )
		setFSMStateTo( startingState );

	if( dt <= 0 )
		return -1;

	if (FSMStateIndex >= (int)states.size()){
		tprintf("Warning: no FSM state was selected in the controller!\n");
		return -1;
	}

	bodyTouchedTheGround = false;
	//see if anything else other than the feet touch the ground...
	for (uint i=0;i<cfs->size();i++){
		//if neither of the bodies involved are articulated, it means they are just props so we can ignore them
		if ((*cfs)[i].rb1->isArticulated() == false && (*cfs)[i].rb2->isArticulated() == false)
			continue;
			
		if (isFoot((*cfs)[i].rb1) || isFoot((*cfs)[i].rb2))
			continue;

		bodyTouchedTheGround = true;
	}

	//advance the phase of the controller
	this->phi += dt/states[FSMStateIndex]->getStateTime();

	//see if we have to transition to the next state in the FSM, and do it if so...
	if (states[FSMStateIndex]->needTransition(phi, fabs(getForceOnFoot(swingFoot, cfs).dotProductWith(PhysicsGlobals::up)), fabs(getForceOnFoot(stanceFoot, cfs).dotProductWith(PhysicsGlobals::up)))){
		int newStateIndex = states[FSMStateIndex]->getNextStateIndex();
		transitionToState(newStateIndex);
		return newStateIndex;
	}

	//if we didn't transition to a new state...
	return -1;
}

/**
	This method is used to return the ratio of the weight that is supported by the stance foot.
*/
double SimBiController::getStanceFootWeightRatio(DynamicArray<ContactPoint> *cfs){
	Vector3d stanceFootForce = getForceOnFoot(stanceFoot, cfs);
	Vector3d swingFootForce = getForceOnFoot(swingFoot, cfs);
	double totalYForce = (stanceFootForce + swingFootForce).dotProductWith(PhysicsGlobals::up);

	if (IS_ZERO(totalYForce))
		return -1;
	else
		return stanceFootForce.dotProductWith(PhysicsGlobals::up) / totalYForce;
}




/**
	This method makes it possible to evaluate the debug pose at any phase angle
*/
void SimBiController::updateTrackingPose(ReducedCharacterStateArray& trackingPose, double phiToUse, int stanceToUse){
	if( FSMStateIndex < 0 )
		setFSMStateTo( startingState );

	if( phiToUse < 0 )
		phiToUse = phi;
	if( phiToUse > 1 )
		phiToUse = 1;
	if( stanceToUse < 0 )
		stanceToUse = stance;
	if( stanceToUse > 1 )
		stanceToUse = 1;

	trackingPose.clear();
	this->character->getState(&trackingPose);
	
	ReducedCharacterState debugRS(&trackingPose);

	//always start from a neutral desired pose, and build from there...
	for (int i=0;i<jointCount;i++){
		debugRS.setJointRelativeOrientation(Quaternion(1, 0, 0, 0), i);
		debugRS.setJointRelativeAngVelocity(Vector3d(), i);
		controlParams[i].relToFrame = false;
	}

	//and this is the desired orientation for the root
	Quaternion qRootD(1, 0, 0, 0);

	SimBiConState* curState = states[FSMStateIndex];

	for (int i=0;i<curState->getTrajectoryCount();i++){
		//now we have the desired rotation angle and axis, so we need to see which joint this is intended for
		int jIndex = curState->sTraj[i]->getJointIndex(stanceToUse);
		
		//if the index is -1, it must mean it's the root's trajectory. Otherwise we should give an error
		if (curState->sTraj[i]->relToCharFrame == true || jIndex == swingHipIndex)
			controlParams[jIndex].relToFrame = true;
		Vector3d d0, v0; 
		computeD0( phiToUse, &d0 );
		computeV0( phiToUse, &v0 );
//		Quaternion newOrientation = curState->sTraj[i]->evaluateTrajectory(this, character->getJoint(jIndex), stanceToUse, phiToUse, d - d0, v - v0);
		Quaternion newOrientation = curState->sTraj[i]->evaluateTrajectory(this, character->getJoint(jIndex), stanceToUse, phiToUse, Vector3d(0,0,0), Vector3d(0,0,0));
		if (jIndex == -1){
			qRootD = newOrientation;
		}else{
			debugRS.setJointRelativeOrientation(newOrientation, jIndex);
		}
	}

//	debugRS.setOrientation(qRootD);
	debugRS.setOrientation(Quaternion());

	//now, we'll make one more pass, and make sure that the orientations that are relative to the character frame are drawn that way
	for (int i=0;i<jointCount;i++){
		if (controlParams[i].relToFrame){
			Quaternion temp;
			Joint* j = character->getJoint(i);
			ArticulatedRigidBody* parent = j->getParent();
			while (parent != root){
				j = parent->getParentJoint();
				parent = j->getParent();
				temp = debugRS.getJointRelativeOrientation(character->getJointIndex(j->name)) * temp;
			}
	
			temp = qRootD * temp;
			temp = temp.getComplexConjugate() * debugRS.getJointRelativeOrientation(i);
			debugRS.setJointRelativeOrientation(temp, i);
		}
	}





}



/**
	This method is used to return a pointer to a rigid body, based on its name and the current stance of the character
*/
RigidBody* SimBiController::getRBBySymbolicName(char* sName){
	RigidBody* result;
	char resolvedName[100];
	//deal with the SWING/STANCE_XXX' case
	if (strncmp(sName , "SWING_", strlen("SWING_"))==0){
		strcpy(resolvedName+1, sName + strlen("SWING_"));
		if (stance == LEFT_STANCE)
			resolvedName[0] = 'r';
		else
			resolvedName[0] = 'l';
	}else
		if (strncmp(sName , "STANCE_", strlen("STANCE_"))==0){
			strcpy(resolvedName+1, sName + strlen("STANCE_"));
			if (stance == LEFT_STANCE)
				resolvedName[0] = 'l';
			else
				resolvedName[0] = 'r';
	}else
		strcpy(resolvedName, sName);

	result = character->getARBByName(resolvedName);

	if (result == NULL)
		throwError("Could not find RB \'%s\\%s\'\n", resolvedName, sName);

	return result;

}


/**
	This method is used to compute the distribution of forces between the two feet
*/
void SimBiController::computeToeAndHeelForces(DynamicArray<ContactPoint> *cfs){
	forceStanceToe.setValues(0,0,0);
	forceSwingToe.setValues(0,0,0);
	forceStanceHeel.setValues(0,0,0);
	forceSwingHeel.setValues(0,0,0);

	toeSwingPos.setValues(0,0,0);
	toeStancePos.setValues(0,0,0);
	heelSwingPos.setValues(0,0,0);
	heelStancePos.setValues(0,0,0);

	swingToeInContact = false;
	stanceToeInContact = false;
	swingHeelInContact = false;
	stanceHeelInContact = false;

	if (cfs == NULL || cfs->size() == 0){
		haveToeAndHeelInformation = false;
		return;
	}
	haveToeAndHeelInformation = true;

	int swingToeCount = 0, stanceToeCount = 0, swingHeelCount = 0, stanceHeelCount = 0;

	for (uint i=0;i<cfs->size();i++){
		if (isStanceFoot((*cfs)[i].rb1) || isStanceFoot((*cfs)[i].rb2)){
			//need to determine if this is a heel or the toe...
			Point3d localPoint = stanceFoot->getLocalCoordinates((*cfs)[i].cp);
			if (localPoint.z > 0){
				forceStanceToe += (*cfs)[i].f;
				toeStancePos += (*cfs)[i].cp;
				stanceToeCount++;
				stanceToeInContact = true;
			}else{
				forceStanceHeel += (*cfs)[i].f;
				heelStancePos += (*cfs)[i].cp;
				stanceHeelCount++;
				stanceHeelInContact = true;
			}
		}

		if (isSwingFoot((*cfs)[i].rb1) || isSwingFoot((*cfs)[i].rb2)){
			//need to determine if this is a heel or the toe...
			Point3d localPoint = swingFoot->getLocalCoordinates((*cfs)[i].cp);
			if (localPoint.z > 0){
				forceSwingToe += (*cfs)[i].f;
				toeSwingPos += (*cfs)[i].cp;
				swingToeCount++;
				swingToeInContact = true;
			}else{
				forceSwingHeel += (*cfs)[i].f;
				heelSwingPos += (*cfs)[i].cp;
				swingHeelCount++;
				swingHeelInContact = true;
			}
		}
	}

	if (stanceToeCount > 0)
		toeStancePos /= stanceToeCount;

	if (stanceHeelCount > 0)
		heelStancePos /= stanceHeelCount;

	if (swingToeCount > 0)
		toeSwingPos /= swingToeCount;

	if (swingHeelCount > 0)
		heelSwingPos /= swingHeelCount;
}

void SimBiController::evaluateJointTargets(){
	ReducedCharacterState poseRS(&desiredPose);

	//d and v are specified in the rotation (heading) invariant coordinate frame
	updateDAndV();

	//there are two stages here. First we will compute the pose (i.e. relative orientations), using the joint trajectories for the current state
	//and then we will compute the PD torques that are needed to drive the links towards their orientations - here the special case of the
	//swing and stance hips will need to be considered

	//always start from a neutral desired pose, and build from there...
	for (int i=0;i<jointCount;i++){
		if (controlParams[i].qRelExternallyComputed == false){
			poseRS.setJointRelativeOrientation(Quaternion(1, 0, 0, 0), i);
			poseRS.setJointRelativeAngVelocity(Vector3d(), i);
		}
		controlParams[i].controlled = true;
		controlParams[i].relToFrame = false;
	}

	//and this is the desired orientation for the root
	qRootD = Quaternion(1, 0, 0, 0);

	double phiToUse = phi;
	//make sure that we only evaluate trajectories for phi values between 0 and 1
	if (phiToUse>1)
		phiToUse = 1;

	SimBiConState* curState = states[FSMStateIndex];
	Quaternion newOrientation;
	Vector3d force, torque;

	// First compute external forces
	characterFrame = character->getHeading();
	for (int i=0;i<curState->getExternalForceCount();i++){
		ExternalForce* ef = curState->sExternalForces[i];
		ArticulatedRigidBody* arb = ef->getARB(stance);

		force.x = ef->forceX.evaluate_catmull_rom( phiToUse );
		force.y = ef->forceY.evaluate_catmull_rom( phiToUse );
		force.z = ef->forceZ.evaluate_catmull_rom( phiToUse );
		force = characterFrame.rotate(force);
		arb->setExternalForce( force );

		torque.x = ef->torqueX.evaluate_catmull_rom( phiToUse );
		torque.y = ef->torqueY.evaluate_catmull_rom( phiToUse );
		torque.z = ef->torqueZ.evaluate_catmull_rom( phiToUse );
		torque = characterFrame.rotate(torque);
		arb->setExternalTorque( torque );

	}

	for (int i=0;i<curState->getTrajectoryCount();i++){
		//now we have the desired rotation angle and axis, so we need to see which joint this is intended for
		int jIndex = curState->sTraj[i]->getJointIndex(stance);

		if (jIndex > -1 && controlParams[jIndex].qRelExternallyComputed)
			continue;

		//get the desired joint orientation to track - include the feedback if necessary/applicable
		Vector3d d0, v0; 
		computeD0( phiToUse, &d0 );
		computeV0( phiToUse, &v0 );	
		newOrientation = curState->sTraj[i]->evaluateTrajectory(this, character->getJoint(jIndex), stance, phiToUse, d - d0, v - v0);

		//if the index is -1, it must mean it's the root's trajectory. Otherwise we should give an error
		if (jIndex == -1){
			qRootD = newOrientation;
			rootControlParams.strength = curState->sTraj[i]->evaluateStrength(phiToUse);
		}else{
			if (curState->sTraj[i]->relToCharFrame == true || jIndex == swingHipIndex){
				controlParams[jIndex].relToFrame = true;
				controlParams[jIndex].frame = characterFrame;
				controlParams[jIndex].frameAngularVelocityInWorld = Vector3d(0,0,0);
			}
			poseRS.setJointRelativeOrientation(newOrientation, jIndex);
			controlParams[jIndex].strength = curState->sTraj[i]->evaluateStrength(phiToUse);
		}
	}
}

/**
	This method is used to compute the PD torques, given the target trajectories
*/
void SimBiController::computePDTorques(DynamicArray<ContactPoint> *cfs){
	//compute the torques now, using the desired pose information - the hip torques will get overwritten below
	PoseController::computeTorques(cfs);
}

/**
	This method is used to lower the torques as the character gets closer to the ground (so that it doesn't try to keep walking).
*/
void SimBiController::blendOutTorques(){
	double h = character->getRoot()->getCMPosition().y;

	double hMax = 0.4;
	double hMin = 0.2;

	if (h > hMax)
		h = hMax;

	if ( h < hMin)
		h = hMin;

	h = (h-hMin) / (hMax-hMin);

	for (uint i=0;i<torques.size();i++){
		torques[i] = torques[i]*h + Vector3d(0,0,0) * (1-h);
	}
}

/**
	This method is used to compute the torques that are to be applied at the next step.
*/
void SimBiController::computeTorques(DynamicArray<ContactPoint> *cfs){
	if( FSMStateIndex < 0 )
		setFSMStateTo( startingState );

	if (FSMStateIndex >= (int)states.size()){
		tprintf("Warning: no FSM state was selected in the controller!\n");
		return;
	}

	evaluateJointTargets();
	computePDTorques(cfs);
	//and now separetely compute the torques for the hips - together with the feedback term, this is what defines simbicon
	computeHipTorques(qRootD, getStanceFootWeightRatio(cfs), Vector3d());
	blendOutTorques();
}

/**
	This method is used to compute the torques that need to be applied to the stance and swing hips, given the
	desired orientation for the root and the swing hip.
*/
void SimBiController::computeHipTorques(const Quaternion& qRootD, double stanceHipToSwingHipRatio, Vector3d ffRootTorque){
	//compute the total torques that should be applied to the root and swing hip, keeping in mind that
	//the desired orientations are expressed in the character frame
	Vector3d rootTorque;
	Vector3d swingHipTorque;

	if (stanceHipToSwingHipRatio < 0)
		rootControlParams.strength = 0;

	//this is the desired orientation in world coordinates
	Quaternion qRootDW;

//	if (SimGlobals::forceHeadingControl == false){
		//qRootD is specified in the character frame, so just maintain the current heading
//		qRootDW = characterFrame * qRootD;
//	}else{
		//qRootDW needs to also take into account the desired heading
	qRootDW = Quaternion::getRotationQuaternion(desiredHeading, PhysicsGlobals::up) * qRootD;
//	}

	double rootStrength = rootControlParams.strength;
	if (rootStrength < 0)
		rootStrength = 0;
	if (rootStrength > 1)
		rootStrength = 1;

	rootControlParams.strength = 1;

	//so this is the net torque that the root wants to see, in world coordinates
	rootTorque = computePDTorque(root->getOrientation(), qRootDW, root->getAngularVelocity(), Vector3d(0,0,0), &rootControlParams);

	rootTorque += ffRootTorque;

	RigidBody* swingHip = character->getJoint(swingHipIndex)->getChild();

	swingHipTorque = torques[swingHipIndex];

	//we need to compute the total torque that needs to be applied at the hips so that the final net torque on the root is rootTorque
	Vector3d rootMakeupTorque;
	for (int i=0;i<jointCount;i++)
		if (character->getJoint(i)->getParent() == root)
			rootMakeupTorque -= torques[i];
	rootMakeupTorque -= rootTorque;

	//add to the root makeup torque the predictive torque as well (only consider the effect of the torque in the lateral plane).
	Vector3d rootPredictiveTorque(0, 0, rootPredictiveTorqueScale*9.8*d.x);
	rootMakeupTorque += characterFrame.rotate(rootPredictiveTorque);

	//assume the stance foot is in contact...
	Vector3d stanceHipTorque = torques[stanceHipIndex];

	//now, based on the ratio of the forces the feet exert on the ground, and the ratio of the forces between the two feet, we will compute the forces that need to be applied
	//to the two hips, to make up the root makeup torque - which means the final torque the root sees is rootTorque!
	stanceHipTorque += rootMakeupTorque * stanceHipToSwingHipRatio * rootStrength;
	swingHipTorque += rootMakeupTorque * (1-stanceHipToSwingHipRatio) * rootStrength;


	if( stanceHipDamping > 0 ) {
		Vector3d wRel = root->getAngularVelocity() - character->joints[stanceHipIndex]->child->getAngularVelocity();
		double wRelLen = wRel.length();
		if (wRelLen > stanceHipMaxVelocity ) wRel = wRel * (stanceHipMaxVelocity/wRelLen);
		stanceHipTorque -=  wRel * (stanceHipDamping * wRelLen);
	}

	//now transform the torque to child coordinates, apply torque limits and then change it back to world coordinates
	Quaternion qStanceHip = character->getJoint(stanceHipIndex)->getChild()->getOrientation();
	stanceHipTorque = qStanceHip.getComplexConjugate().rotate(stanceHipTorque);
	limitTorque(&stanceHipTorque, &controlParams[stanceHipIndex]);
	stanceHipTorque = qStanceHip.rotate(stanceHipTorque);

	Quaternion qSwingHip = character->getJoint(swingHipIndex)->getChild()->getOrientation();
	swingHipTorque = qSwingHip.getComplexConjugate().rotate(swingHipTorque);
	limitTorque(&swingHipTorque, &controlParams[swingHipIndex]);
	swingHipTorque = qSwingHip.rotate(swingHipTorque);

	//and done...
	torques[stanceHipIndex] = stanceHipTorque;
	torques[swingHipIndex] = swingHipTorque;
}


/**
	This method is used to obtain the d and v parameters, using the current postural information of the biped
*/
void SimBiController::updateDAndV(){
	characterFrame = character->getHeading();

	comPosition = character->getCOM();

	comVelocity = character->getCOMVelocity();

	d = Vector3d(stanceFoot->getCMPosition(), comPosition);
	//d is now in world coord frame, so we'll represent it in the 'character frame'
	d = characterFrame.inverseRotate(d);
	//compute v in the 'character frame' as well
	v = characterFrame.inverseRotate(comVelocity);


	//and now compute the vector from the COM to the center of midpoint between the feet, again expressed in world coordinates
	Point3d feetMidpoint = (stanceFoot->getCMPosition() + swingFoot->getCMPosition());
	feetMidpoint /= 2.0;

	//now we have to compute the difference between the current COM and the desired COMPosition, in world coordinates
	doubleStanceCOMError = Vector3d(comPosition, feetMidpoint);
	//and add the user specified offset
//	doubleStanceCOMError += characterFrame.rotate(Vector3d(SimGlobals::COMOffsetX, 0, SimGlobals::COMOffsetZ));
	doubleStanceCOMError.y = 0;

}

/**
	This method is used to resolve the names (map them to their index) of the joints.
*/
void SimBiController::resolveJoints(SimBiConState* state){
	char tmpName[100];
	for (uint i=0;i<state->sExternalForces.size();i++){
		ExternalForce* ef = state->sExternalForces[i];
	
		//deal with the SWING_XXX' case
		if (strncmp(ef->bName, "SWING_", strlen("SWING_"))==0){
			strcpy(tmpName+1, ef->bName + strlen("SWING_"));
			tmpName[0] = 'r';
			ef->leftStanceARB = character->getARBByName(tmpName);
			if (ef->leftStanceARB==NULL)
				throwError("Cannot find arb %s\n", tmpName);
			tmpName[0] = 'l';
			ef->rightStanceARB = character->getARBByName(tmpName);
			if (ef->rightStanceARB==NULL)
				throwError("Cannot find arb %s\n", tmpName);
			continue;
		}
		//deal with the STANCE_XXX' case
		if (strncmp(ef->bName, "STANCE_", strlen("STANCE_"))==0){
			strcpy(tmpName+1, ef->bName + strlen("STANCE_"));
			tmpName[0] = 'l';
			ef->leftStanceARB = character->getARBByName(tmpName);
			if (ef->leftStanceARB==NULL)
				throwError("Cannot find arb %s\n", tmpName);
			tmpName[0] = 'r';
			ef->rightStanceARB = character->getARBByName(tmpName);
			if (ef->rightStanceARB==NULL)
				throwError("Cannot find arb %s\n", tmpName);
			continue;
		}
		//if we get here, it means it is just the name...
		ef->leftStanceARB = character->getARBByName(ef->bName);
		if (ef->leftStanceARB==NULL)
			throwError("Cannot find joint %s\n", ef->bName);
		ef->rightStanceARB = ef->leftStanceARB;
	
	}

	for (uint i=0;i<state->sTraj.size();i++){
		Trajectory* jt = state->sTraj[i];
		//deal with the 'root' special case
		if (strcmp(jt->jName, "root") == 0){
			jt->leftStanceIndex = jt->rightStanceIndex = -1;
			continue;
		}
		//deal with the SWING_XXX' case
		if (strncmp(jt->jName, "SWING_", strlen("SWING_"))==0){
			strcpy(tmpName+1, jt->jName + strlen("SWING_"));
			tmpName[0] = 'r';
			jt->leftStanceIndex = character->getJointIndex(tmpName);
			if (jt->leftStanceIndex<0)
				throwError("Cannot find joint %s\n", tmpName);
			tmpName[0] = 'l';
			jt->rightStanceIndex = character->getJointIndex(tmpName);
			if (jt->rightStanceIndex<0)
				throwError("Cannot find joint %s\n", tmpName);
			continue;
		}
		//deal with the STANCE_XXX' case
		if (strncmp(jt->jName, "STANCE_", strlen("STANCE_"))==0){
			strcpy(tmpName+1, jt->jName + strlen("STANCE_"));
			tmpName[0] = 'l';
			jt->leftStanceIndex = character->getJointIndex(tmpName);
			if (jt->leftStanceIndex<0)
				throwError("Cannot find joint %s\n", tmpName);
			tmpName[0] = 'r';
			jt->rightStanceIndex = character->getJointIndex(tmpName);
			if (jt->rightStanceIndex<0)
				throwError("Cannot find joint %s\n", tmpName);
			continue;
		}
		//if we get here, it means it is just the name...
		jt->leftStanceIndex = character->getJointIndex(jt->jName);
		if (jt->leftStanceIndex<0)
			throwError("Cannot find joint %s\n", jt->jName);
		jt->rightStanceIndex = jt->leftStanceIndex;
	}
}


/**
	This method is used to write the details of the current controller to a file
*/
void SimBiController::writeToFile(char* fileName, char* stateFileName){
	FILE* f;
	if (fileName == NULL || (f = fopen(fileName, "w")) == NULL)
		return;

	fprintf( f, "%s\n", getConLineString(CON_PD_GAINS_START) );	
	fprintf( f, "#        joint name              Kp      Kd      MaxTorque    ScaleX        ScaleY        ScaleZ\n" );
	fprintf( f, "    root\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", 
					rootControlParams.kp,
					rootControlParams.kd,
					rootControlParams.maxAbsTorque,
					rootControlParams.scale.x,
					rootControlParams.scale.y,
					rootControlParams.scale.z );
	writeGains(f);

	fprintf( f, "%s\n", getConLineString(CON_PD_GAINS_END) );

	fprintf( f, "\n" );
	
	if( stanceHipDamping > 0 ) {
		fprintf( f, "%s %lf\n", getConLineString(CON_STANCE_HIP_DAMPING), stanceHipDamping );
		fprintf( f, "%s %lf\n", getConLineString(CON_STANCE_HIP_MAX_VELOCITY), stanceHipMaxVelocity );
	}

	fprintf( f, "\n" );
	

	for( uint i=0; i<states.size(); ++i ) {
	
		fprintf( f, "\n\n" );
		states[i]->writeState( f, i );

	}

	fprintf( f, "\n\n" );

	fprintf( f, "%s %d\n", getConLineString(CON_START_AT_STATE), startingState );
	fprintf( f, "%s %s\n", getConLineString(CON_STARTING_STANCE), 
		(stanceFoot == lFoot)?"left":"right" );
	if( stateFileName == NULL )
		fprintf( f, "%s %s\n", getConLineString(CON_CHARACTER_STATE), initialBipState );
	else
		fprintf( f, "%s %s\n", getConLineString(CON_CHARACTER_STATE), stateFileName );

	fclose(f);
}

/**
	This method is used to parse the information passed in the string. This class knows how to read lines
	that have the name of a joint, followed by a list of the pertinent parameters. If this assumption is not held,
	then classes extended this one are required to provide their own implementation of this simple parser
*/
void SimBiController::parseGainLine(char* line){
	double kp, kd, tMax, scX, scY, scZ;
	char jName[100];
	if (sscanf(line, "%s", jName) !=1)
		throwError("To specify the PD gains, you need tp specify the jointName/root --> \'%s\'", line);
	if (strcmp(jName, "root") == 0){
		if (sscanf(line, "%s %lf %lf %lf %lf %lf %lf\n", jName, &kp, &kd, &tMax, &scX, &scY, &scZ) !=7)
			throwError("To specify the PD gains, you need: 'jointName Kp Kd Tmax scaleX scaleY scaleZ'! --> \'%s\'", line);

		rootControlParams.kp = kp;
		rootControlParams.kd = kd;
		rootControlParams.maxAbsTorque = tMax;
		rootControlParams.scale = Vector3d(scX, scY, scZ);
	}else
		PoseController::parseGainLine(line);
}


/**
	This method loads all the pertinent information regarding the simbicon controller from a file.
*/
void SimBiController::loadFromFile(char* fName){
	if (fName == NULL)
		throwError("NULL file name provided.");
	FILE *f = fopen(fName, "r");
	if (f == NULL)
		throwError("Could not open file: %s", fName);

	//to be able to load multiple controllers from multiple files,
	//we will use this offset to make sure that the state numbers
	//mentioned in each input file are updated correctly
	int stateOffset = this->states.size();
	SimBiConState* tempState;
	int tempStateNr = -1;

	//have a temporary buffer used to read the file line by line...
	char buffer[200];
	//this is where it happens.

	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (feof(f))
			break;
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		switch (lineType) {
			case CON_PD_GAINS_START:
				readGains(f);
				break;
			case CON_STATE_START:
				tempState = new SimBiConState();
				sscanf(line, "%d", &tempStateNr);
				if (tempStateNr != stateOffset + this->states.size())
					throwError("Inccorect state offset specified: %d", tempStateNr);
				states.push_back(tempState);
				tempState->readState(f, stateOffset);
				//now we have to resolve all the joint names (i.e. figure out which joints they apply to).
				resolveJoints(tempState);
				break;
			case CON_STANCE_HIP_DAMPING:
				sscanf(line, "%lf", &stanceHipDamping);
				break;
			case CON_STANCE_HIP_MAX_VELOCITY:
				sscanf(line, "%lf", &stanceHipMaxVelocity);
				break;
			case CON_ROOT_PRED_TORQUE_SCALE:
				sscanf(line, "%lf", &rootPredictiveTorqueScale);
				break;
			case CON_CHARACTER_STATE:
				character->loadReducedStateFromFile(trim(line));
				strcpy(initialBipState, trim(line));
				break;
			case CON_START_AT_STATE:
				if (sscanf(line, "%d", &tempStateNr) != 1)
					throwError("A starting state must be specified!");
				transitionToState(tempStateNr);
				startingState = tempStateNr;
				break;
			case CON_COMMENT:
				break;
			case CON_STARTING_STANCE:
				if (strncmp(trim(line), "left", 4) == 0){
					setStance(LEFT_STANCE);
					startingStance = LEFT_STANCE;
				}
				else if (strncmp(trim(line), "right", 5) == 0){
					setStance(RIGHT_STANCE);
					startingStance = RIGHT_STANCE;
				}
				else 
					throwError("When using the \'reverseTargetOnStance\' keyword, \'left\' or \'right\' must be specified!");
				break;
			case CON_NOT_IMPORTANT:
				tprintf("Ignoring input line: \'%s\'\n", line);
				break;
			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
}


/**
	This makes it possible to externally access the states of this controller
	Returns null if the index is out of range
 */
SimBiConState* SimBiController::getState( uint idx ) {
	if( idx >= states.size() ) return NULL;
	return states[idx];
}
