#pragma once

#include <Utils/Utils.h>
#include <Core/Controller.h>
#include "Character.h"



/**
	This class is used as a container for the properties needed by a PD controller
*/
class ControlParams : public Observable{
public:
	Joint* joint;
	//this variable is set to true if the current joint is being actively controlled, false otherwise
	bool controlled;
	//these two variables are the proporitonal and derivative gains for the PD controller used to compute the torque
	double kp, kd;
	//this is the maximum absolute value torque allowed at this joint
	double maxAbsTorque;
	//the torques about the about the x, y and z axis will be scaled differently to account for the potentially different principal moments of inertia
	//of the child
	Vector3d scale;
	//if this variable is set to true, it means that the PD targets are computed elsewhere, so we should skip them...
	bool qRelExternallyComputed;

	double strength;

	//this variable, if true, indicates that the desired orientation is specified in the custom coordinate frame
	bool relToFrame;

	//and this is the coordinate frame (and its angular velocity) that the desired orientation is specified in, if relToFrame is true
	Quaternion frame;
	Vector3d   frameAngularVelocityInWorld;

public:

	/**
		This constructor initializes the variables to some safe, default values
	*/
	ControlParams( Joint* joint = NULL ){
		this->joint = joint;
		controlled = false;
		kp = kd = 0;
		maxAbsTorque = 0;
		scale = Vector3d();
		strength = 1;
		relToFrame = false;
		qRelExternallyComputed = false;
	}

	/**
		This constructor initializes the variables to some safe, default values
	*/
	ControlParams( const ControlParams& other ){
		joint = other.joint;
		controlled = other.controlled;
		kp = other.kp;
		kd = other.kd;
		maxAbsTorque = other.maxAbsTorque;
		scale = other.scale;
		strength = other.strength;
		relToFrame = other.relToFrame;
		frame = other.frame;
		frameAngularVelocityInWorld = other.frameAngularVelocityInWorld;
		qRelExternallyComputed = other.qRelExternallyComputed;
	}

	const char* getJointName() const {
		if( joint != NULL )	return joint->getName();
		return "root";
	}

	Joint* getJoint() const { return joint; }

	bool setJoint(Joint* joint) {
		if( this->joint != NULL && this->joint != joint )
			return false;
		this->joint = joint;
		return true;
	}

	void setKp( double kp ) {
		this->kp = kp;
		notifyObservers();
	}

	double getKp() const { return kp; }

	void setKd( double kd ) {
		this->kd = kd;
		notifyObservers();
	}

	double getKd() const { return kd; }

	void setMaxAbsTorque( double maxAbsTorque ) {
		this->maxAbsTorque = maxAbsTorque;
		notifyObservers();
	}

	double getMaxAbsTorque() const { return maxAbsTorque; }

	void setScale( const Vector3d& scale ) {
		this->scale = scale;
		notifyObservers();
	}

	const Vector3d& getScale() const { return scale; }

	void setStrength( double strength ) {
		this->strength = strength;
		notifyObservers();
	}

	double getStrength() const { return strength; }

	void setRelToFrame( bool relToFrame ) {
		this->relToFrame = relToFrame;
		notifyObservers();
	}

	bool getRelToFrame() const { return relToFrame; }
};

/**
	A pose controller is used to have the character track a given pose.
	Each pose is given as a series of relative orientations, one for
	each parent-child pair (i.e. joint). Classes extending this one 
	have to worry about setting the desired relative orientation properly.
*/
class PoseController : public Controller{
	friend class TestApp;
protected:
	//this is the pose that the character is aiming at achieving
	ReducedCharacterStateArray desiredPose;
	//this is the array of joint properties used to specify the 
	DynamicArray<ControlParams> controlParams;

	/**
		This method is used to parse the information passed in the string. This class knows how to read lines
		that have the name of a joint, followed by a list of the pertinent parameters. If this assumption is not held,
		then classes extended this one are required to provide their own implementation of this simple parser
	*/
	virtual void parseGainLine(char* line);
public:
	/**
		Constructor - expects a character that it will work on
	*/
	PoseController(Character* ch);
	virtual ~PoseController(void);

	virtual void addControlParams( const ControlParams& params ) {
		int jIndex = character->getJointIndex(params.getJoint());
		if (jIndex < 0)
			throwError("Cannot find joint \'%s\' in character", params.getJointName());
		controlParams[jIndex] = params;
		notifyObservers();
	}

	ControlParams* getControlParams( int index ) {
		return &controlParams[index];
	}

	/**
		Scale all the PD gains of the controller by the specified factor
	*/
	virtual void scaleGains( double factor ) {
		for( unsigned int i=0; i < controlParams.size(); ++i ) {
			if( strcmp( "ToeJoint", character->getJoint(i)->name+1 ) == 0 )
				continue;
			double kp = controlParams[i].getKp() * factor;
			controlParams[i].setKp( kp );
			double kd = sqrt(kp)*2;
			controlParams[i].setKd( kd );

		}
	}

	int getControlParamsCount() const {
		return controlParams.size();
	}

	/**
		This method is used to compute the torques, based on the current and desired poses
	*/
	virtual void computeTorques(DynamicArray<ContactPoint> *cfs);

	/**
		This method is used to compute the PD torque, given the current relative orientation of two coordinate frames (child and parent),
		the relative angular velocity, the desired values for the relative orientation and ang. vel, as well as the virtual motor's
		PD gains. The torque returned is expressed in the coordinate frame of the 'parent'.
	*/
	static Vector3d computePDTorque(const Quaternion& qRel, const Quaternion& qRelD, const Vector3d& wRel, const Vector3d& wRelD, ControlParams* pdParams);

	/**
		This method is used to scale and apply joint limits to the torque that is passed in as a parameter. The orientation that transforms 
		the torque from the coordinate frame that it is currently stored in, to the coordinate frame of the 'child' to which the torque is 
		applied to (it wouldn't make sense to scale the torques in any other coordinate frame)  is also passed in as a parameter.
	*/
	static void scaleAndLimitTorque(Vector3d* torque, ControlParams* pdParams, const Quaternion& qToChild);

	/**
		This method is used to apply joint limits to the torque passed in as a parameter. It is assumed that
		the torque is already represented in the correct coordinate frame
	*/
	static void limitTorque(Vector3d* torque, ControlParams* cParams);

	/**
		This method is used to read the gain coefficients, as well as max torque allowed for each joint
		from the file that is passed in as a parameter.
	*/
	void readGains(char* fName);

	/**
		This method is used to read the gain coefficients, as well as max torque allowed for each joint
		from the file that is passed in as a parameter.
	*/
	void readGains(FILE* f);
	
	
	/**
		This method is used to write the gain coefficients, as well as max torque allowed for each joint
		from the file that is passed in as a parameter.
	*/
	void writeGains(FILE* f);

	/**
		sets the targets to match the current state of the character
	*/
	void setTargetsFromState();
};


