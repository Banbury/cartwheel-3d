#include "IKVMCController.h"

#include <Core/BehaviourController.h>
#include <Core/TwoLinkIK.h>


/**
	constructor
*/
IKVMCController::IKVMCController(Character* b) : SimBiController(b){
	lKneeIndex = character->getJointIndex("lKnee");
	rKneeIndex = character->getJointIndex("rKnee");
	lAnkleIndex = character->getJointIndex("lAnkle");
	rAnkleIndex = character->getJointIndex("rAnkle");

	velDSagittal = 0;
	velDCoronal = 0;

	comOffsetSagittal = 0;
	comOffsetCoronal = 0;

	panicHeight = 0;
	unplannedForHeight = 0;

	swingLegPlaneOfRotation = Vector3d(-1,0,0);

	doubleStanceMode = false;
	vmc = new VirtualModelController(b);

	behaviour = NULL;
}

/**
	destructor
*/
IKVMCController::~IKVMCController(void){
	if( behaviour != NULL )
		delete behaviour;
	delete vmc;
}

/**
	This method is used to compute the desired orientation and angular velocity for a parent RB and a child RB, relative to the grandparent RB and
	parent RB repsectively. The input is:
		- the index of the joint that connects the grandparent RB to the parent RB, and the index of the joint between parent and child

		- the distance from the parent's joint with its parent to the location of the child's joint, expressed in parent coordinates

		- two rotation normals - one that specifies the plane of rotation for the parent's joint, expressed in grandparent coords, 
		  and the other specifies the plane of rotation between the parent and the child, expressed in parent's coordinates.

	    - The position of the end effector, expressed in child's coordinate frame

		- The desired position of the end effector, expressed in world coordinates

		- an estimate of the desired position of the end effector, in world coordinates, some dt later - used to compute desired angular velocities
*/
void IKVMCController::computeIKQandW(int parentJIndex, int childJIndex, const Vector3d& parentAxis, const Vector3d& parentNormal, const Vector3d& childNormal, const Vector3d& childEndEffector, const Point3d& wP, bool computeAngVelocities, const Point3d& futureWP, double dt){
	//this is the joint between the grandparent RB and the parent
	Joint* parentJoint = character->joints[parentJIndex];
	//this is the grandparent - most calculations will be done in its coordinate frame
	ArticulatedRigidBody* gParent = parentJoint->parent;
	//this is the reduced character space where we will be setting the desired orientations and ang vels.
	ReducedCharacterState rs(&desiredPose);

	//the desired relative orientation between parent and grandparent
	Quaternion qParent;
	//and the desired relative orientation between child and parent
	Quaternion qChild;


	TwoLinkIK::getIKOrientations(parentJoint->getParentJointPosition(), gParent->getLocalCoordinates(wP), parentNormal, parentAxis, childNormal, childEndEffector, &qParent, &qChild);

	controlParams[parentJIndex].relToFrame = false;
	controlParams[childJIndex].relToFrame = false;
	rs.setJointRelativeOrientation(qChild, childJIndex);
	rs.setJointRelativeOrientation(qParent, parentJIndex);
	

	Vector3d wParentD(0,0,0);
	Vector3d wChildD(0,0,0);

	if (computeAngVelocities){
		//the joint's origin will also move, so take that into account, by subbing the offset by which it moves to the
		//futureTarget (to get the same relative position to the hip)
		Vector3d velOffset = gParent->getAbsoluteVelocityForLocalPoint(parentJoint->getParentJointPosition());

		Quaternion qParentF;
		Quaternion qChildF;
		TwoLinkIK::getIKOrientations(parentJoint->getParentJointPosition(), gParent->getLocalCoordinates(futureWP + velOffset * -dt), parentNormal, parentAxis, childNormal, childEndEffector, &qParentF, &qChildF);

		Quaternion qDiff = qParentF * qParent.getComplexConjugate();
		wParentD = qDiff.v * 2/dt;
		//the above is the desired angular velocity, were the parent not rotating already - but it usually is, so we'll account for that
		wParentD -= gParent->getLocalCoordinates(gParent->getAngularVelocity());

		qDiff = qChildF * qChild.getComplexConjugate();
		wChildD = qDiff.v * 2/dt;

		//make sure we don't go overboard with the estimates, in case there are discontinuities in the trajectories...
		boundToRange(&wChildD.x, -5, 5);boundToRange(&wChildD.y, -5, 5);boundToRange(&wChildD.z, -5, 5);
		boundToRange(&wParentD.x, -5, 5);boundToRange(&wParentD.y, -5, 5);boundToRange(&wParentD.z, -5, 5);
	}

	rs.setJointRelativeAngVelocity(wChildD, childJIndex);
	rs.setJointRelativeAngVelocity(wParentD, parentJIndex);
}

/**
	This method is used to compute the target angles for the swing hip and swing knee that help 
	to ensure (approximately) precise foot-placement control.
*/
void IKVMCController::computeIKSwingLegTargets(double dt){
	Point3d pNow, pFuture;

	//note, V is already expressed in character frame coordinates.
	pNow = getSwingFootTargetLocation(phi, comPosition, characterFrame);
	pFuture = getSwingFootTargetLocation(MIN(phi+dt, 1), comPosition + comVelocity * dt, characterFrame);


	Vector3d parentAxis(character->joints[swingHipIndex]->cJPos, character->joints[swingKneeIndex]->pJPos);
	Vector3d childAxis(character->joints[swingKneeIndex]->cJPos, character->joints[swingAnkleIndex]->pJPos);

	computeIKQandW(swingHipIndex, swingKneeIndex, parentAxis, swingLegPlaneOfRotation, Vector3d(-1,0,0), childAxis, pNow, true, pFuture, dt);
//	computeIKQandW(swingHipIndex, swingKneeIndex, Vector3d(0, -0.355, 0), Vector3d(1,0,0), Vector3d(1,0,0), Vector3d(0, -0.36, 0), pNow, true, pFuture, dt);
}

/**
	returns the required stepping location, as predicted by the inverted pendulum model. The prediction is made
	on the assumption that the character will come to a stop by taking a step at that location. The step location
	is expressed in the character's frame coordinates.
*/
Vector3d IKVMCController::computeIPStepLocation(){
	Vector3d step;
	double h = fabs(comPosition.y - stanceFoot->getCMPosition().y);
	step.x = v.x * sqrt(h/9.8 + v.x * v.x / (4*9.8*9.8)) * 1.3;
	step.z = v.z * sqrt(h/9.8 + v.z * v.z / (4*9.8*9.8)) * 1.1;	
	step.y = 0;
	return step;
}

/**
	This method returns a target for the location of the swing foot, based on some state information. It is assumed that the velocity vel
	is expressed in character-relative coordinates (i.e. the sagittal component is the z-component), while the com position, and the
	initial swing foot position is expressed in world coordinates. The resulting desired foot position is also expressed in world coordinates.
*/
Point3d IKVMCController::getSwingFootTargetLocation(double t, const Point3d& com, const Quaternion& charFrameToWorld){
	Vector3d step;
	step.z = swingFootTrajectorySagittal.evaluate_catmull_rom(t);
	step.x = swingFootTrajectoryCoronal.evaluate_catmull_rom(t);
	//now transform this vector into world coordinates
	step = charFrameToWorld.rotate(step);
	//add it to the com location
	step = com + step;
	//finally, set the desired height of the foot
	step.y = swingFootHeightTrajectory.evaluate_catmull_rom(t) + panicHeight + unplannedForHeight;

	step += computeSwingFootDelta(t);

	return step;
}

/**
	This method is used to ensure that each RB sees the net torque that the PD controller computed for it.
	Without it, an RB sees also the sum of -t of every child.
*/
void IKVMCController::bubbleUpTorques(){
	for (int i=character->joints.size()-1;i>=0;i--){
		if (i != stanceHipIndex && i != stanceKneeIndex)
			if (character->joints[i]->getParent() != root)
				torques[character->joints[i]->getParent()->pJoint->id] +=  torques[i];
	}
}

/**
	This method computes the torques that cancel out the effects of gravity, 
	for better tracking purposes
*/
void IKVMCController::computeGravityCompensationTorques(){
	vmc->resetTorques();
	for (uint i=0;i<character->joints.size();i++){
		if (i != stanceHipIndex && i != stanceKneeIndex && i != stanceAnkleIndex)
			vmc->computeJointTorquesEquivalentToForce(character->joints[i], Point3d(), Vector3d(0, character->joints[i]->child->props.mass*9.8, 0), NULL);
	}

	for (uint i=0;i<character->joints.size();i++){
		torques[i] += vmc->torques[i];
	}
}

/**
	This method is used to compute the force that the COM of the character should be applying.
*/
Vector3d IKVMCController::computeVirtualForce(){
	//this is the desired acceleration of the center of mass
	Vector3d desA = Vector3d();
	desA.z = (velDSagittal - v.z) * 30;
	desA.x = (-d.x + comOffsetCoronal) * 20 + (velDCoronal - v.x) * 9;
	
	if (doubleStanceMode == true){
		Vector3d errV = characterFrame.inverseRotate(doubleStanceCOMError*-1);
		desA.x = (-errV.x + comOffsetCoronal) * 20 + (velDCoronal - v.x) * 9;
		desA.z = (-errV.z + comOffsetSagittal) * 10 + (velDSagittal - v.z) * 150;
	}

	//and this is the force that would achieve that - make sure it's not too large...
	Vector3d fA = (desA) * character->getMass();
	boundToRange(&fA.x, -100, 100);
	boundToRange(&fA.z, -60, 60);

	//now change this quantity to world coordinates...
	fA = characterFrame.rotate(fA);

	return fA;
}

/**
	This method returns performes some pre-processing on the virtual torque. The torque is assumed to be in world coordinates,
	and it will remain in world coordinates.
*/
void IKVMCController::preprocessAnkleVTorque(int ankleJointIndex, DynamicArray<ContactPoint> *cfs, Vector3d *ankleVTorque){
	bool heelInContact, toeInContact;
	ArticulatedRigidBody* foot = character->joints[ankleJointIndex]->child;
	getForceInfoOn(foot, cfs, &heelInContact, &toeInContact);
	*ankleVTorque = foot->getLocalCoordinates(*ankleVTorque);

	if (toeInContact == false || phi < 0.2 || phi > 0.8) ankleVTorque->x = 0;

	Vector3d footRelativeAngularVel = foot->getLocalCoordinates(foot->getAngularVelocity());
	if ((footRelativeAngularVel.z < -0.2 && ankleVTorque->z > 0) || (footRelativeAngularVel.z > 0.2 && ankleVTorque->z < 0))
		ankleVTorque->z = 0;

	if (fabs(footRelativeAngularVel.z) > 1.0) ankleVTorque->z = 0;
	if (fabs(footRelativeAngularVel.x) > 1.0) ankleVTorque->x = 0;
	
	boundToRange(&ankleVTorque->z, -20, 20);

	*ankleVTorque = foot->getWorldCoordinates(*ankleVTorque);
}

/**
	This method is used to compute torques for the stance leg that help achieve a desired speed in the sagittal and lateral planes
*/
void IKVMCController::computeLegTorques(int ankleIndex, int kneeIndex, int hipIndex, DynamicArray<ContactPoint> *cfs){
	Vector3d fA = computeVirtualForce();

	Vector3d r;

	Point3d p = comPosition;

	r.setToVectorBetween(character->joints[ankleIndex]->child->getWorldCoordinates(character->joints[ankleIndex]->cJPos), p);

	Vector3d ankleTorque = r.crossProductWith(fA);
	preprocessAnkleVTorque(ankleIndex, cfs, &ankleTorque);
	torques[ankleIndex] += ankleTorque;

	r.setToVectorBetween(character->joints[kneeIndex]->child->getWorldCoordinates(character->joints[kneeIndex]->cJPos), p);
	torques[kneeIndex] += r.crossProductWith(fA);

	r.setToVectorBetween(character->joints[hipIndex]->child->getWorldCoordinates(character->joints[hipIndex]->cJPos), p);
	torques[hipIndex] += r.crossProductWith(fA);

	//the torque on the stance hip is cancelled out, so pass it in as a torque that the root wants to see!
	ffRootTorque -= r.crossProductWith(fA);

	int lBackIndex = character->getJointIndex("pelvis_lowerback");
	r.setToVectorBetween(character->joints[lBackIndex]->child->getWorldCoordinates(character->joints[lBackIndex]->cJPos), p);
	torques[lBackIndex] += r.crossProductWith(fA) / 10;

	int mBackIndex = character->getJointIndex("lowerback_torso");
	r.setToVectorBetween(character->joints[mBackIndex]->child->getWorldCoordinates(character->joints[mBackIndex]->cJPos), p);
	torques[mBackIndex] += r.crossProductWith(fA) / 10;
}

void IKVMCController::COMJT(DynamicArray<ContactPoint> *cfs){
	//applying a force at the COM induces the force f. The equivalent torques are given by the J' * f, where J' is
	// dp/dq, where p is the COM.

	int lBackIndex = character->getJointIndex("pelvis_lowerback");
	int mBackIndex = character->getJointIndex("lowerback_torso");

	double m = 0;
	ArticulatedRigidBody* tibia = character->joints[stanceAnkleIndex]->parent;
	ArticulatedRigidBody* femur = character->joints[stanceKneeIndex]->parent;
	ArticulatedRigidBody* pelvis = character->joints[stanceHipIndex]->parent;
	ArticulatedRigidBody* lBack = character->joints[lBackIndex]->child;
	ArticulatedRigidBody* mBack = character->joints[mBackIndex]->child;

	Point3d anklePos = character->joints[stanceAnkleIndex]->child->getWorldCoordinates(character->joints[stanceAnkleIndex]->cJPos);
	Point3d kneePos = character->joints[stanceKneeIndex]->child->getWorldCoordinates(character->joints[stanceKneeIndex]->cJPos);
	Point3d hipPos = character->joints[stanceHipIndex]->child->getWorldCoordinates(character->joints[stanceHipIndex]->cJPos);
	Point3d lbackPos = character->joints[lBackIndex]->child->getWorldCoordinates(character->joints[lBackIndex]->cJPos);
	Point3d mbackPos = character->joints[mBackIndex]->child->getWorldCoordinates(character->joints[mBackIndex]->cJPos);

	//total mass...
	m = tibia->props.mass + femur->props.mass + pelvis->props.mass + lBack->props.mass + mBack->props.mass;

	Vector3d fA = computeVirtualForce();

	Vector3d f1 =	Vector3d(anklePos, tibia->state.position) * tibia->props.mass +
					Vector3d(anklePos, femur->state.position) * femur->props.mass + 
					Vector3d(anklePos, pelvis->state.position) * pelvis->props.mass + 
					Vector3d(anklePos, lBack->state.position) * lBack->props.mass + 
					Vector3d(anklePos, mBack->state.position) * mBack->props.mass;
	f1 /= m;
	
	Vector3d f2 =	Vector3d(kneePos, femur->state.position) * femur->props.mass + 
					Vector3d(kneePos, pelvis->state.position) * pelvis->props.mass + 
					Vector3d(kneePos, lBack->state.position) * lBack->props.mass + 
					Vector3d(kneePos, mBack->state.position) * mBack->props.mass;
	f2 /= m;

	Vector3d f3 =	Vector3d(hipPos, pelvis->state.position) * pelvis->props.mass + 
					Vector3d(hipPos, lBack->state.position) * lBack->props.mass + 
					Vector3d(hipPos, mBack->state.position) * mBack->props.mass;
	f3 /= m;

	Vector3d f4 =	Vector3d(lbackPos, lBack->state.position) * lBack->props.mass + 
					Vector3d(lbackPos, mBack->state.position) * mBack->props.mass;
	f4 /= m;

	Vector3d f5 =	Vector3d(mbackPos, mBack->state.position) * mBack->props.mass;
	f5 /= m;
	
	Vector3d ankleTorque = f1.crossProductWith(fA);
	preprocessAnkleVTorque(stanceAnkleIndex, cfs, &ankleTorque);

	torques[stanceAnkleIndex] += ankleTorque;
	torques[stanceKneeIndex] += f2.crossProductWith(fA);
	torques[stanceHipIndex] += f3.crossProductWith(fA);

	//the torque on the stance hip is cancelled out, so pass it in as a torque that the root wants to see!
	ffRootTorque -= f3.crossProductWith(fA);

	torques[lBackIndex] -= f4.crossProductWith(fA) * 0.5;
	torques[mBackIndex] -= f5.crossProductWith(fA) * 0.3;
}

/**
	updates the indexes of the swing and stance hip, knees and ankles
*/
void IKVMCController::updateSwingAndStanceReferences(){
	stanceHipIndex = ((stance == LEFT_STANCE) ? (lHipIndex) : (rHipIndex));
	swingHipIndex = ((stance == LEFT_STANCE) ? (rHipIndex) : (lHipIndex));
	swingKneeIndex = ((stance == LEFT_STANCE) ? (rKneeIndex) : (lKneeIndex));
	stanceKneeIndex = ((stance == LEFT_STANCE) ? (lKneeIndex) : (rKneeIndex));
	stanceAnkleIndex = ((stance == LEFT_STANCE) ? (lAnkleIndex) : (rAnkleIndex));
	swingAnkleIndex = ((stance == LEFT_STANCE) ? (rAnkleIndex) : (lAnkleIndex));

	swingHip = character->joints[swingHipIndex];
	swingKnee = character->joints[swingKneeIndex];
}

/**
	This method is used to compute the torques
*/
void IKVMCController::computeTorques(DynamicArray<ContactPoint> *cfs){
	//get the correct references for the swing knee and hip, as well as an estimate of the starting position for the swing foot


	//evaluate the target orientation for every joint, using the SIMBICON state information
	evaluateJointTargets();


	//now overwrite the target angles for the swing hip and the swing knee in order to ensure foot-placement control
	if (doubleStanceMode == false)
		computeIKSwingLegTargets(0.001);

	computePDTorques(cfs);

	//bubble-up the torques computed from the PD controllers
	bubbleUpTorques();

	//we'll also compute the torques that cancel out the effects of gravity, for better tracking purposes
	computeGravityCompensationTorques();

	ffRootTorque.setValues(0,0,0);

	if (cfs->size() > 0)
		COMJT(cfs);
//		computeLegTorques(stanceAnkleIndex, stanceKneeIndex, stanceHipIndex, cfs);

	if (doubleStanceMode && cfs->size() > 0)
		computeLegTorques(swingAnkleIndex, swingKneeIndex, swingHipIndex, cfs);
	

	//and now separetely compute the torques for the hips - together with the feedback term, this is what defines simbicon
	computeHipTorques(qRootD, getStanceFootWeightRatio(cfs), ffRootTorque);
//	blendOutTorques();
}

/**
	determines if there are any heel/toe forces on the given RB
*/
void IKVMCController::getForceInfoOn(RigidBody* rb, DynamicArray<ContactPoint> *cfs, bool* heelForce, bool* toeForce){
	//figure out if the toe/heel are in contact...
	*heelForce = false;
	*toeForce = false;
	Point3d tmpP;
	for (uint i=0;i<cfs->size();i++){
		if (haveRelationBetween((*cfs)[i].rb1, rb) || haveRelationBetween((*cfs)[i].rb2, rb)){
			tmpP = rb->getLocalCoordinates((*cfs)[i].cp);
			if (tmpP.z < 0) *heelForce = true;
			if (tmpP.z > 0) *toeForce = true;
		}
	}	
}

void IKVMCController::performPreTasks(double dt, DynamicArray<ContactPoint> *cfs) { 
	if( behaviour != NULL ) 
		behaviour->simStepPlan(SimGlobals::dt);

	SimBiController::performPreTasks(dt, cfs);
}

bool IKVMCController::performPostTasks(double dt, DynamicArray<ContactPoint> *cfs) {
	bool newState = SimBiController::performPostTasks(dt, cfs);

	if( newState ) {
		if( behaviour != NULL ) 
			behaviour->conTransitionPlan();
	}

	return newState;

}