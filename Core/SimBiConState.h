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

#pragma once

#include <MathLib/Trajectory.h>
#include <Core/BalanceFeedback.h>
#include <Utils/Utils.h>
#include <MathLib/Vector3d.h>
#include <MathLib/Quaternion.h>
#include "ConUtils.h"
#include "SimGlobals.h"
#include <Utils/Utils.h>
#include <Utils/Observable.h>



/**
 *  This helper class is used to hold information regarding one component of a state trajectory. This includes (mainly): the base trajectory, 
 *	a data member that specifies the feedback law to be used, and the axis about which it represents a rotation, 
 */
class TrajectoryComponent : public Observable {

public:
	//this is the array of basis functions that specify the trajectories for the sagittal plane.
	Trajectory1d baseTraj;

	//this trajectory can be used to scale the base trajectory up or down as a function of the distance from the
	//expected d-value. Used, for instance, to limit the contribution of the toe-off if not at steady state.
	Trajectory1d dTrajScale;

	//this trajectory can be used to scale the base trajectory up or down as a function of the distance from the
	//expected d-value. Used, for instance, to limit the contribution of the toe-off if not at steady state.
	Trajectory1d vTrajScale;

	//if this variable is set to true, then when the stance of the character is the left side, the 
	//static target provided by this trajectory should be negated
	bool reverseAngleOnLeftStance;
	//if this variable is set to true, then when the stance of the character is the right side, the 
	//static target provided by this trajectory should be negated
	bool reverseAngleOnRightStance;
	//this is the rotation axis that the angles obtained from the trajectory represent rotations about
	Vector3d rotationAxis;

	//this is the balance feedback that is to be used with this trajectory
	BalanceFeedback* bFeedback;

	//this is the base value for the trajectory
	double offset;

	/**
		Some public constants
	*/
	static const int ROS_LEFT = LEFT_STANCE;
	static const int ROS_RIGHT = RIGHT_STANCE;
	static const int ROS_DONT_REVERSE = 100;

	/**
		default constructor
	*/
	TrajectoryComponent(){
		rotationAxis = Vector3d();
		reverseAngleOnLeftStance = false;
		reverseAngleOnRightStance = false;
		bFeedback = NULL;
		offset = 0;
	}

	/**
		default destructor.
	*/
	~TrajectoryComponent(){
		delete bFeedback;
	}

	inline void setRotationAxis( const Vector3d& axis ) {
		rotationAxis = axis;
		notifyObservers();
	}

	inline const Vector3d& getRotationAxis() {
		return rotationAxis;
	}

	/**
		This method sets the stance of this state
	*/
	inline void setReverseOnStance( int stance ) {
		if ( stance == ROS_LEFT ) {
			reverseAngleOnLeftStance = true;
			reverseAngleOnRightStance = false;
		}
		else if ( stance == ROS_RIGHT ) {
			reverseAngleOnLeftStance = false;
			reverseAngleOnRightStance = true;
		}
		else if ( stance == ROS_DONT_REVERSE ) {
			reverseAngleOnLeftStance = false;
			reverseAngleOnRightStance = false;
		}
		else
			throwError( "Invalid state stance!" );
		notifyObservers();
	}


	inline int getReverseOnStance() const {
		if( reverseAngleOnLeftStance && reverseAngleOnRightStance )
			throwError( "Invalid state stance!" );
		if( reverseAngleOnLeftStance )
			return ROS_LEFT;
		if( reverseAngleOnRightStance )
			return ROS_RIGHT;

		return ROS_DONT_REVERSE;
	}

	inline void setFeedback( BalanceFeedback* feedback_disown ) {
		bFeedback = feedback_disown;
		notifyObservers();
	}

	inline BalanceFeedback* getFeedback() const {
		return bFeedback;
	}

	inline void setBaseTrajectory( const Trajectory1d& traj ) {
		baseTraj.copy( traj );
		notifyObservers();
	}
	inline const Trajectory1d& getBaseTrajectory() const {
		return baseTraj;
	}

	inline void setVTrajScale( const Trajectory1d& traj ) {
		vTrajScale.copy( traj );
		notifyObservers();
	}
	inline const Trajectory1d& getVTrajScale() const {
		return vTrajScale;
	}

	inline void setDTrajScale( const Trajectory1d& traj ) {
		dTrajScale.copy( traj );
		notifyObservers();
	}
	inline const Trajectory1d& getDTrajScale() const {
		return dTrajScale;
	}


	/**
		this method is used to evaluate the trajectory at a given point phi, knowing the stance of the character, 
		and the d and v values used for feedback.
	*/
	inline Quaternion evaluateTrajectoryComponent(SimBiController* con, Joint* j, int stance, double phi, const Vector3d& d, const Vector3d& v, bool bareTrajectory = false){
		double baseAngle = offset;
	
		double scale = 1;
		//this d.z should really be d dotted with some axis - probably the same as the feedback one...
		if (dTrajScale.getKnotCount() > 0)
			scale *= dTrajScale.evaluate_linear(d.z);

		//this v.z should really be v dotted with some axis - probably the same as the feedback one...
		if (vTrajScale.getKnotCount() > 0)
			scale *= vTrajScale.evaluate_linear(v.z);

		if (bareTrajectory == true)
			scale = 1;

		if (baseTraj.getKnotCount() > 0)
			baseAngle += baseTraj.evaluate_catmull_rom(phi) * scale;

		if (stance == LEFT_STANCE && reverseAngleOnLeftStance)
			baseAngle = -baseAngle;
		if (stance == RIGHT_STANCE && reverseAngleOnRightStance)
			baseAngle = -baseAngle;

		double feedbackValue = computeFeedback(con, j, phi, d, v);

		return Quaternion::getRotationQuaternion(baseAngle + feedbackValue, rotationAxis);
	}

	/**
		this method is used to evaluate the feedback contribution, given the current phase, d and v.
	*/
	inline double computeFeedback(SimBiController* con, Joint* j, double phi, const Vector3d& d, const Vector3d& v){
		if (bFeedback == NULL)
			return 0;
		return bFeedback->getFeedbackContribution(con, j, phi, d, v);
	}

	/** 
		Update this component to recenter it around the new given D and V trajectories
	*/
	void updateComponent(SimBiController* con, Joint* j, Trajectory1d& newDTrajX, Trajectory1d& newDTrajZ, Trajectory1d& newVTrajX, Trajectory1d& newVTrajZ, 
					      Trajectory1d* oldDTrajX, Trajectory1d* oldDTrajZ, Trajectory1d* oldVTrajX, Trajectory1d* oldVTrajZ, int nbSamples );

	/**
		This method is used to read a trajectory from a file
	*/ 
	void readTrajectoryComponent(FILE* f);

	/**
		This method is used to read a trajectory from a file
	*/ 
	void writeBaseTrajectory(FILE* f);

	void writeScaleTraj(FILE* f, const Trajectory1d& scaleTraj, char type);

	/**
		This method is used to write a trajectory to a file
	*/
	void writeTrajectoryComponent(FILE* f);

};


/**
 *  This helper class is used to hold information regarding one external force.
 */
class ExternalForce : public Observable{

public:
	
	//if the biped that is controlled is in a left-sideed stance, then this is the pointer of the articulated rigid body that
	//the external force is applied to 
	ArticulatedRigidBody* leftStanceARB;
	//and this is the pointer of the articulated rigid body that the trajectory is associated to if the biped is in a
	//right-side stance
	ArticulatedRigidBody* rightStanceARB;
	//we'll keep the body name, for debugging purposes
	char bName[100];

	//these are the trajectory to define the force
	Trajectory1d forceX;
	Trajectory1d forceY;
	Trajectory1d forceZ;

	//these are the trajectory to define the force
	Trajectory1d torqueX;
	Trajectory1d torqueY;
	Trajectory1d torqueZ;

	/**
		default constructor
	*/
	ExternalForce(){
		leftStanceARB = rightStanceARB = NULL;
		strcpy(bName, "NoNameBody");
	}

	/**
		default destructor.
	*/
	~ExternalForce(){
	}

	inline void setForceX( const Trajectory1d& traj ) {
		forceX.copy( traj );
		notifyObservers();
	}

	inline void setForceY( const Trajectory1d& traj ) {
		forceY.copy( traj );
		notifyObservers();
	}

	inline void setForceZ( const Trajectory1d& traj ) {
		forceZ.copy( traj );
		notifyObservers();
	}

	inline void setTorqueX( const Trajectory1d& traj ) {
		torqueX.copy( traj );
		notifyObservers();
	}

	inline void setTorqueY( const Trajectory1d& traj ) {
		torqueY.copy( traj );
		notifyObservers();
	}

	inline void setTorqueZ( const Trajectory1d& traj ) {
		torqueZ.copy( traj );
		notifyObservers();
	}

	inline const Trajectory1d& getForceX() const {
		return forceX;
	}

	inline const Trajectory1d& getForceY() const {
		return forceY;
	}
	inline const Trajectory1d& getForceZ() const {
		return forceZ;
	}


	inline const Trajectory1d& getTorqueX() const {
		return torqueX;
	}

	inline const Trajectory1d& getTorqueY() const {
		return torqueY;
	}
	inline const Trajectory1d& getTorqueZ() const {
		return torqueZ;
	}

	/**
		this method returns the joint index that this trajectory applies to, unless this applies to the root, in which case it returns -1.
	*/
	inline ArticulatedRigidBody* getARB(int stance){
		return (stance == LEFT_STANCE)?(leftStanceARB):(rightStanceARB);
	}

	/**
		Sets the body name.
		"STANCE_" is replaced by "l" when the stance is left and "r" when the stance is right
		"SWING_" is replaced by "r" when the stance is left and "l" when the stance is right
	*/
	inline void setBodyName(const char* name) {
		strncpy( bName, name, 99 );
		notifyObservers();
	}

	inline const char* getBodyName() const {
		return bName;
	}

};


/**
 *  This helper class is used to hold information regarding one state trajectory. This includes: a sequence of components, 
 *	the index of the joint that this trajectory applies to, the coordinate frame in which the final orientation is expressed, etc.
 */
class Trajectory : public Observable{

public:
	//these are the components that define the current trajectory
	DynamicArray<TrajectoryComponent*> components;
	
	//if the biped that is controlled is in a left-sideed stance, then this is the index of the joint that
	//the trajectory is used to control - it is assumed that if this is -1, then the trajectory applies
	//to the torso, and not to a joint
	int leftStanceIndex;
	//and this is the index of the joint that the trajectory is associated to if the biped is in a
	//right-side stance
	int rightStanceIndex;
	//we'll keep the joint name, for debugging purposes
	char jName[100];

	//if this variable is set to true, then the desired orientation here is expressed in character coordinates, otherwise it is relative
	//to the parent
	bool relToCharFrame;

	//this is the trajectory for the strength of the joing.
	Trajectory1d strengthTraj;

	/**
		default constructor
	*/
	Trajectory(){
		leftStanceIndex = rightStanceIndex = -1;
		strcpy(jName, "NoNameJoint");
		relToCharFrame = false;
	}

	/**
		default destructor.
	*/
	~Trajectory(){
		for (uint i=0;i<components.size();i++)
			delete components[i];
	}

	inline void setStrengthTrajectory( const Trajectory1d& traj ) {
		strengthTraj.copy( traj );
		notifyObservers();
	}

	inline const Trajectory1d& getStrengthTrajectory() const {
		return strengthTraj;
	}

	inline void setRelativeToCharacterFrame( bool rel = true ) {
		relToCharFrame = rel;
		notifyObservers();
	}

	inline bool isRelativeToCharacterFrame() const {
		return relToCharFrame;
	}

	void addTrajectoryComponent( TrajectoryComponent* trajComp_disown ) {
		components.push_back( trajComp_disown );
		notifyObservers();
	}

	void clearTrajectoryComponents() {
		components.clear();
		notifyObservers();
	}

	TrajectoryComponent* getTrajectoryComponent( uint index ) {
		return components[index];
	}

	uint getTrajectoryComponentCount() const {
		return components.size();
	}

	/**
		this method is used to evaluate the trajectory at a given point phi, knowing the stance of the character, 
		and the d and v values used for feedback.
	*/
	inline Quaternion evaluateTrajectory(SimBiController* con, Joint* j, int stance, double phi, const Vector3d& d, const Vector3d& v, bool bareTrajectory = false){
//
//		if (j && strcmp(j->getName(), "lAnkle") == 0)
//			tprintf("%2.4lf, %2.4lf\n", d.x, d.z);

		Quaternion q = Quaternion(1, 0, 0, 0);

		for (uint i=0;i<components.size();i++)
			q = components[i]->evaluateTrajectoryComponent(con, j, stance, phi, d, v, bareTrajectory) * q;

		return q;
	}

	/**
		this method is used to evaluate the strength of the joint at a given value of phi.
	*/
	inline double evaluateStrength(double phiToUse) {
		if( strengthTraj.getKnotCount() == 0 ) return 1.0;
		return strengthTraj.evaluate_catmull_rom( phiToUse );
	}

	/**
		this method returns the joint index that this trajectory applies to, unless this applies to the root, in which case it returns -1.
	*/
	inline int getJointIndex(int stance){
		return (stance == LEFT_STANCE)?(leftStanceIndex):(rightStanceIndex);
	}

	/**
		Sets the joint name.
		"STANCE_" is replaced by "l" when the stance is left and "r" when the stance is right
		"SWING_" is replaced by "r" when the stance is left and "l" when the stance is right
	*/
	inline void setJointName(const char* name) {
		strncpy( jName, name, 99 );
		notifyObservers();
	}

	inline const char* getJointName() const {
		return jName;
	}

	/** 
		Update all the components to recenter them around the new given D and V trajectories
	*/
	void updateComponents(SimBiController* con, Joint* j, Trajectory1d& newDTrajX, Trajectory1d& newDTrajZ, Trajectory1d& newVTrajX, Trajectory1d& newVTrajZ,
						   Trajectory1d* oldDTrajX, Trajectory1d* oldDTrajZ, Trajectory1d* oldVTrajX, Trajectory1d* oldVTrajZ, int nbSamples ) {
		int nbComponents = components.size();
		for (int i=0;i<nbComponents;i++)
			components[i]->updateComponent(con, j, newDTrajX, newDTrajZ, newVTrajX, newVTrajZ,
										   oldDTrajX, oldDTrajZ, oldVTrajX, oldVTrajZ, nbSamples );
	}

	/**
		This method is used to read a trajectory from a file
	*/ 
	void readTrajectory(FILE* f);

	/**
		This method is used to write the knots of a strength trajectory to the file, where they are specified one (knot) on a line
	*/
	void writeStrengthTrajectory(FILE* f);

	/**
		This method is used to write a trajectory to a file
	*/
	void writeTrajectory(FILE* f);

};

/**
 *	A simbicon controller is made up of a set of a number of states. Transition between states happen on foot contact, time out, user interaction, etc.
 *  Each controller state holds the trajectories for all the joints that are controlled. 
 */
class SimBiConState : public Observable {
friend class ControllerEditor;
friend class SimBiController;
friend class BehaviourController;
friend class TurnController;
friend class TestApp3;
friend class TestApp4;

friend class ConCompositionFramework;
friend class CompositeController;
friend class ControllerOptimizer;
friend class BalanceControlOptimizer;
friend class SimbiconPlayer_v2;
private:
	//this is the array of external forces, one for each body that has an external force
	DynamicArray<ExternalForce*> sExternalForces;
	//this is the array of trajectories, one for each joint that is controlled
	DynamicArray<Trajectory*> sTraj;
	//this is a name of this state
	char name[100];
	//this is the number of the state that we should transition to in the controller's finite state machine
	int nextStateIndex;
	//this is the ammount of time that it is expected the biped will spend in this state
	double stateTime;

	//upon a transition to a new FSM state, it is assumed that the stance of the character either will be given stance, it will be reverseed , or keept the same.
	//if a state is designed for a certain stance, it is given by this variable
	//for generic states, this variable is used to determine if the stance should be reversed (as opposed to set to left or right), or stay the same.
	bool reverseStance;
	//and if this is the same, then upon entering this FSM state, the stance will remain the same
	bool keepStance;
	//if both keepStance and reverseStance are set to false, then this is the state that the character is asumed to take
	int stateStance;

	//if this variable is set to true, it indicates that the transition to the new state should occur when the swing foot contacts the ground
	//if this variable is false, then it will happen when the time of the controller goes up
	bool transitionOnFootContact;
	//if we are to allow a transition on foot contact, we need to take care of the possibility that it
	//will occur early. In this case, we may still want to switch. If phi is at least this, then it is assumed
	//that we can transition;
	double minPhiBeforeTransitionOnFootContact;
	//also, in order to make sure that we don't transition tooooo early, we expect a minimum force applied on the swing foot before
	//it should register as a contact
	double minSwingFootForceForContact;

	//this is the trajectory for the zero value of the feedback d
	Trajectory1d* dTrajX;
	Trajectory1d* dTrajZ;

	//this is the trajectory for the zero value of the feedback v
	Trajectory1d* vTrajX;
	Trajectory1d* vTrajZ;

public:
	/**
		Some public constants
	*/
	static const int STATE_LEFT_STANCE = LEFT_STANCE;
	static const int STATE_RIGHT_STANCE = RIGHT_STANCE;
	static const int STATE_REVERSE_STANCE = 100;
	static const int STATE_KEEP_STANCE = 101;


	/**
		default constructor
	*/
	SimBiConState(void){
		strcpy(name, "Uninitialized state");
		nextStateIndex = -1;
		this->stateTime = 0;
		transitionOnFootContact = true;
		minPhiBeforeTransitionOnFootContact = 0.5;
		minSwingFootForceForContact = 20.0;
		reverseStance = false;
		keepStance = false;

		dTrajX = NULL;
		dTrajZ = NULL;
		vTrajX = NULL;
		vTrajZ = NULL;
	}

	/**
		destructor
	*/
	~SimBiConState(void){
		for (uint i=0;i<sExternalForces.size();i++)
			delete sExternalForces[i];
		for (uint i=0;i<sTraj.size();i++)
			delete sTraj[i];
		if( dTrajX != NULL ) delete dTrajX;
		if( dTrajZ != NULL ) delete dTrajZ;
		if( vTrajX != NULL ) delete vTrajX;
		if( vTrajZ != NULL ) delete vTrajZ;
	}

	/**
		this method is used to determine the new stance, based on the information in this state and the old stance
	*/
	inline int getStateStance(int oldStance){
		if (keepStance == true)
			return oldStance;
		if (reverseStance == false)
			return stateStance;
		if (oldStance == LEFT_STANCE)
			return RIGHT_STANCE;
		return LEFT_STANCE;
	}

	/**
		Returns the time we're expecting to spend in this state
	*/
	inline double getStateTime(){
		return stateTime;
	}

	/**
		this method is used to retrieve the index of the next state 
	*/
	inline int getNextStateIndex(){
		return nextStateIndex;
	}

	/**
		this method is used to return the number of trajectories for this state
	*/
	inline int getExternalForceCount(){
		return sExternalForces.size();
	}

	/**
		Access a given trajectory
	*/
	inline ExternalForce* getExternalForce( uint idx ) {
		if( idx >= sExternalForces.size() ) return NULL;
		return sExternalForces[idx];
	}

	/**
		Access a given trajectory by name
	*/
	inline ExternalForce* getExternalForce( const char* name) {
		for (uint i=0;i<sExternalForces.size();i++)
			if (strcmp(sExternalForces[i]->bName, name) == 0)
				return sExternalForces[i];
		return NULL;
	}

	/**
		this method is used to return the number of trajectories for this state
	*/
	inline int getTrajectoryCount(){
		return sTraj.size();
	}

	/**
		Access a given trajectory
	*/
	inline Trajectory* getTrajectory( uint idx ) {
		if( idx >= sTraj.size() ) return NULL;
		return sTraj[idx];
	}

	/**
		Access a given trajectory by name
	*/
	inline Trajectory* getTrajectory( const char* name) {
		for (uint i=0;i<sTraj.size();i++)
			if (strcmp(sTraj[i]->jName, name) == 0)
				return sTraj[i];
		return NULL;
	}

	inline void setDTrajX( Trajectory1d* traj1d_disown ) {
		if( dTrajX != NULL ) delete dTrajX;
		dTrajX = traj1d_disown;
		notifyObservers();
	}

	inline void setDTrajZ( Trajectory1d* traj1d_disown ) {
		if( dTrajZ != NULL ) delete dTrajZ;
		dTrajZ = traj1d_disown;
		notifyObservers();
	}

	inline void setVTrajX( Trajectory1d* traj1d_disown ) {
		if( vTrajX != NULL ) delete vTrajX;
		vTrajX = traj1d_disown;
		notifyObservers();
	}

	inline void setVTrajZ( Trajectory1d* traj1d_disown ) {
		if( vTrajZ != NULL ) delete vTrajZ;
		vTrajZ = traj1d_disown;
		notifyObservers();
	}

	inline const Trajectory1d* getDTrajX() const {
		return dTrajX;
	}

	inline const Trajectory1d* getDTrajZ() const {
		return dTrajZ;
	}

	inline const Trajectory1d* getVTrajX() const {
		return vTrajX;
	}

	inline const Trajectory1d* getVTrajZ() const {
		return vTrajZ;
	}

	/**
		This method is used to determine if, based on the parameters passed in and the type of state this is,
		the current state in the controller FSM needs to be transitioned from.
	*/
	inline bool needTransition(double phi, double swingFootVerticalForce, double stanceFootVerticalForce){
		//if it is a foot contact based transition
		if (transitionOnFootContact == true){
			//transition if we have a meaningful foot contact, and if it does not happen too early on...
			if ((phi > minPhiBeforeTransitionOnFootContact && swingFootVerticalForce > minSwingFootForceForContact) || phi >= 1)
				return true;
			return false;
		}

		//otherwise it must be a time-based transition
		if (phi >= 1)
			return true;

		return false;
	}

	/**
		This method makes it possible to set the state name
	*/
	inline void setName(const char* name) {
		strncpy( this->name, name, 99 );
		notifyObservers();
	}

	inline void setNextStateIndex( int nextStateIndex ) {
		this->nextStateIndex = nextStateIndex;
		notifyObservers();
	}

	inline void setTransitionOnFootContact( bool transition = true ) {
		this->transitionOnFootContact = transition;
		notifyObservers();
	}

	inline bool getTransitionOnFootContact() const {
		return this->transitionOnFootContact;
	}

	/**
		This method sets the stance of this state
	*/
	inline void setStance( int stanceType ) {
		if ( stanceType == STATE_LEFT_STANCE ) {
			reverseStance = false;
			keepStance = false;
			stateStance = LEFT_STANCE;
		}
		else if ( stanceType == STATE_RIGHT_STANCE ) {
			reverseStance = false;
			keepStance = false;
			stateStance = RIGHT_STANCE;
		}
		else if ( stanceType == STATE_REVERSE_STANCE ) {
			reverseStance = true;
			keepStance = false;
			stateStance = -1;
		}
		else if ( stanceType == STATE_KEEP_STANCE ) {
			reverseStance = false;
			keepStance = true;
			stateStance = -1;
		}
		else
			throwError( "Invalid state stance!" );
		notifyObservers();	
	}

	int getStance() const {
		if( reverseStance && keepStance )
				throwError( "Invalid state stance!" );
		if( reverseStance )
				return STATE_REVERSE_STANCE;
		if( keepStance )
				return STATE_KEEP_STANCE;

		if( stateStance == LEFT_STANCE )
				return STATE_LEFT_STANCE;

		return STATE_RIGHT_STANCE;
	}

	void setDuration( double duration ) {
		stateTime = duration;
		notifyObservers();
	}

	double getDuration() {
		return stateTime;
	}

	void addExternalForce( ExternalForce* extForce_disown ) {
		sExternalForces.push_back( extForce_disown );
		notifyObservers();
	}

	void clearExternalForce() {
		sExternalForces.clear();
		notifyObservers();
	}

	void addTrajectory( Trajectory* traj_disown ) {
		sTraj.push_back( traj_disown );
		notifyObservers();
	}

	void clearTrajectories() {
		sTraj.clear();
		notifyObservers();
	}


	/**
		This method makes it possible to access the state name
	*/
	inline const char* getName() {
		return name;
	}

	/**
		This method is used to read the state parameters from a file
	*/
	void readState(FILE* f, int offset);

	/**
		This method is used to write the state parameters to a file
	*/
	void writeState(FILE* f, int index);


	/** 
		Update all the trajectories to recenter them around the new given D and V trajectories
		Also save these new D and V trajectories.
	*/
	void updateDVTrajectories(SimBiController* con, Joint* j, Trajectory1d& newDTrajX, Trajectory1d& newDTrajZ, Trajectory1d& newVTrajX, Trajectory1d& newVTrajZ, int nbSamples = 100 );


	/**
		This method is used to read the knots of a 1D trajectory from the file, where they are specified one (knot) on a line
		The trajectory is considered complete when a line starting with endingLineType is encountered
	*/
	static void readTrajectory1d(FILE* f, Trajectory1d& result, int endingLineType );

	static void writeTrajectory1d(FILE* f, Trajectory1d& result, int startingLineType, int endingLineType );

};


