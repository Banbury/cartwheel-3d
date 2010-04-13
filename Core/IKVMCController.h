#pragma once

#include <MathLib/Trajectory.h>

#include <Core/SimBiController.h>
#include <Core/VirtualModelController.h>


/**
	Controller Inspired by Simbicon and Virtual Model Control ideas. Uses IK to control swing foot placement. It assumes
	a pretty specilized character architecture: Legs look like this: 
		root -> hips (3 dof) -> knees (1 dof, around x-axis, for now) -> ankles (doesn't matter how many dofs) -> (may or may not have toes, etc).

	An IKVMCController is a Simbicon Controller. The targets for the swing hip and swing knee are computed using IK, based on a target
	foot location (which is given by the inverted pendulum prediction), and the stance ~hip, knee and ankle torques are computed in part
	by the desired velocity of the CM. Gravity compensation can be added to any rb in the body (propagated to the root). Also, "torque bubbling"
	can be added for any chain that starts at the root.

	TODO:

		- try out some stairs, slopes
		- try some IK for the arm, while walking
		- try some static balance, arms moving...
		- get a less 'hacky' implementation of the desired foot placement position (at least in the sagittal plane).

		- test out 'external' foot placement accuracy - DONE!
		- make walk more natural - how much is simbicon, how much is T = J' * f for push-off? How much does the vertical trajectory of swing foot matter? - DONE!
		- eliminate/reduce the torso/head wobbling. - DONE!
		- stance leg/swing leg velocity/position control through T = J' * f - DONE!
		- get some decent toe-off - Pretty much DONE!
		- torque bubbling - DONE!
		- gravity compensation - DONE!
		- check to see why the crouched walk happens sometime - stance knee should be straight, shouldn't it? - DONE!
		- check to see why swing hip initially goes inwards! - DONE!
		- make sure character can walk in any direction - DONE!
		- limit the extent to which stance ankle 'rolls' on the heel - DONE!
		- how much should the strength of the root be? What works best?
			- doesn't actually matter too much. 1 seems to work best, so we'll go with that.



		- THINGS TO TRY STILL:
			- try to somehow bring the upper body into the motion - maybe simulate the force being applied
			to the CM - i.e. t = dpCM/dq * f, where CM now only takes into account the chain from the head/torso
			to the stance foot - i don't know...
			- upper body - can it be used to make it slow down/speed up/whatever faster?
			- maybe com J'?
*/

class BehaviourController;
class IKVMCController: public SimBiController{
friend class TestApp3;
friend class TestApp4;
friend class BehaviourController;
private:
	//keep track of the swing knee and swing hip
	int swingKneeIndex;
	Joint *swingKnee, *swingHip;
	int lKneeIndex, rKneeIndex, lAnkleIndex, rAnkleIndex;
	//keep track of the stance ankle, stance knee and stance hip
	int stanceAnkleIndex, stanceKneeIndex, swingAnkleIndex;
	//this is a controller that we will be using to compute gravity-cancelling torques
	VirtualModelController* vmc;
	//some feed-forward torque we want the root to see...
	Vector3d ffRootTorque;

	/**
		determines if there are any heel/toe forces on the given RB
	*/
	void getForceInfoOn(RigidBody* rb, DynamicArray<ContactPoint> *cfs, bool* heelForce, bool* toeForce);

	BehaviourController* behaviour;

public:
	//desired velocity in the sagittal plane
	double velDSagittal;
	//desired velocity in the coronal plane...
	double velDCoronal;
	//this is a desired foot trajectory that we may wish to follow, expressed separately, for the 3 components,
	//and relative to the current location of the CM
	Trajectory1d swingFootTrajectorySagittal;
	Trajectory1d swingFootTrajectoryCoronal;
	Trajectory1d swingFootHeightTrajectory;
	Trajectory1d swingFootTrajectoryDeltaSagittal;
	Trajectory1d swingFootTrajectoryDeltaCoronal;
	Trajectory1d swingFootTrajectoryDeltaHeight;
	//this is the vector that specifies the plane of rotation for the swing leg, relative to the root...
	Vector3d swingLegPlaneOfRotation;
	//if the controller is in double-stance mode, then the swing foot won't have an IK target
	bool doubleStanceMode;
	//desired offset of the CM relative to the stance foot/midpoint of the feet
	double comOffsetSagittal;
	double comOffsetCoronal;
	//this variable can be used to quickly alter the desired height, if panic ensues...
	double panicHeight;
	//and this should be used to add height for the leg (i.e. if it needs to step over an obstacle that wasn't planned for).
	double unplannedForHeight;

public:
	/**
		constructor
	*/
	IKVMCController(Character* b);

	/**
		destructor
	*/
	~IKVMCController(void);

	inline void setSwingFootTrajectoryDeltaSagittal( const Trajectory1d& traj ) {
		swingFootTrajectoryDeltaSagittal.copy( traj );
		notifyObservers();
	}
	inline const Trajectory1d& getSwingFootTrajectoryDeltaSagittal() const {
		return swingFootTrajectoryDeltaSagittal;
	}

	inline void setSwingFootTrajectoryDeltaCoronal( const Trajectory1d& traj ) {
		swingFootTrajectoryDeltaCoronal.copy( traj );
		notifyObservers();
	}
	inline const Trajectory1d& getSwingFootTrajectoryDeltaCoronal() const {
		return swingFootTrajectoryDeltaCoronal;
	}

	inline void setSwingFootTrajectoryDeltaHeight( const Trajectory1d& traj ) {
		swingFootTrajectoryDeltaHeight.copy( traj );
		notifyObservers();
	}
	inline const Trajectory1d& getSwingFootTrajectoryDeltaHeight() const {
		return swingFootTrajectoryDeltaHeight;
	}

	inline Vector3d computeSwingFootDelta( double phiToUse = -1, int stanceToUse = -1 ) {

		if( phiToUse < 0 )
			phiToUse = phi;
		if( phiToUse > 1 )
			phiToUse = 1;
		if( stanceToUse < 0 )
			stanceToUse = stance;
		if( stanceToUse > 1 )
			stanceToUse = 1;

		double sign = (stanceToUse==LEFT_STANCE)?1.0:-1.0;
		return Vector3d(
			swingFootTrajectoryDeltaCoronal.evaluate_catmull_rom(phiToUse) * sign,
			swingFootTrajectoryDeltaHeight.evaluate_catmull_rom(phiToUse),
			swingFootTrajectoryDeltaSagittal.evaluate_catmull_rom(phiToUse) );
	}

	void setBehaviour( BehaviourController* behaviour_disown ) {
		if( this->behaviour != NULL )
			throwError( "Behaviour already set!" );
		this->behaviour = behaviour_disown;
	}

	BehaviourController* getBehaviour() { return behaviour; }

	/**
		returns the required stepping location, as predicted by the inverted pendulum model. The prediction is made
		on the assumption that the character will come to a stop by taking a step at that location. The step location
		is expressed in the character's frame coordinates.
	*/
	Vector3d computeIPStepLocation();

	/**
		This method is used to compute the target angles for the swing hip and swing knee that help 
		to ensure (approximately) precise foot-placement control.
	*/
	void computeIKSwingLegTargets(double dt);

	/**
		This method computes the desired target location for the swing ankle. It also returns an estimate of the desired
		foot location some time dt later. This method returns the points expressed in world coordinates.
	*/
//	void getDesiredSwingAngleLocation(Point3d* target, Point3d* futureTarget, double dt);

	/**
		This method is used to compute the torques
	*/
	virtual void computeTorques(DynamicArray<ContactPoint> *cfs);

	/**
		This method returns a target for the location of the swing foot, based on some state information. It is assumed that the velocity v
		is expressed in character-relative coordinates (i.e. the sagittal component is the z-component), while the com position, and the
		initial swing foot position is expressed in world coordinates. The resulting desired foot position is also expressed in world coordinates.
	*/
	Point3d getSwingFootTargetLocation(double t, const Point3d& com, const Quaternion& charFrameToWorld);

	/**
		This method computes the torques that cancel out the effects of gravity, 
		for better tracking purposes
	*/
	void computeGravityCompensationTorques();

	/**
		updates the indexes of the swing and stance hip, knees and ankles
	*/
	void updateSwingAndStanceReferences();

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
	void computeIKQandW(int parentJIndex, int childJIndex, const Vector3d& parentAxis, const Vector3d& parentNormal, const Vector3d& childNormal, const Vector3d& childEndEffector, const Point3d& wP, bool computeAngVelocities, const Point3d& futureWP, double dt);

	/**
		This method is used to ensure that each RB sees the net torque that the PD controller computed for it.
		Without it, an RB sees also the sum of -t of every child.
	*/
	void bubbleUpTorques();

	/**
		This method is used to compute torques for the stance leg that help achieve a desired speed in the sagittal and lateral planes
	*/
	void computeLegTorques(int ankleIndex, int kneeIndex, int hipIndex, DynamicArray<ContactPoint> *cfs);

	void COMJT(DynamicArray<ContactPoint> *cfs);

	/**
		This method is used to compute the force that the COM of the character should be applying.
	*/
	Vector3d computeVirtualForce();

	/**
		This method returns performes some pre-processing on the virtual torque. The torque is assumed to be in world coordinates,
		and it will remain in world coordinates.
	*/
	void preprocessAnkleVTorque(int ankleJointIndex, DynamicArray<ContactPoint> *cfs, Vector3d *ankleVTorque);

	virtual void performPreTasks(double dt, DynamicArray<ContactPoint> *cfs);

	// Returns true if it transitioned to a new state, false otherwise
	virtual bool performPostTasks(double dt, DynamicArray<ContactPoint> *cfs);

};



