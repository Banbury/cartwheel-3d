#pragma once

#include <Physics/Joint.h>
#include <Core/Character.h>


/**
	This class implements the analytical solution for a simple linkage that is formed by a parent link and a child link connected by a joint - the child joint. It is assumed
	that the parent can rotate about some relative coordinate frame - the grandparent frame (that can be the parent's parent in the linkage, or the world if
	the parent is a root, since it can still rotate relative to the world frame) - about any axis - i.e. the parent is connected to the grandparent by a 
	ball-in-socket joint that doesn't have any joint limits. The child can only rotate relative to the parent around the axis n. The vector v, in general, 
	represents the axis that connects either the child joint to the child's end effector, or the parent's parent joint to the child joint. This class can be used
	to retrieve the relative orientation between the parent and the grandparent, as well as the relative orientation between the parent and the child, so that
	the child's end effector is as close as possible to a specified target. No joint limits are implemented by this solver.
*/
class AnalyticalIK{
public:
	/**
		This method determines the position of the joint between the child and parent links. 
		Input (all quantities need to be measured in the same coordinate frame):
			p1 - location of parent's point of revolution (for example the shoulder for an arm)
			p2 - target location of end effector of child link (for example, the wrist location
			n - normal to the plane in which the relative motion between parent and child will take place (for example, the plane defined by the arm and forearm)
			r1 - the length of the parent link
			r2 - the length of the child link
		Return:
			p - the position of the joint (the elbow, for instance), measured in the same coordinates as the values passed in here
	*/
	static inline Point3d solve(const Point3d& p1, const Point3d& p2, const Vector3d& n, double r1, double r2){
		//the solution for this comes from computation of the intersection of two circles of radii r1 and r2, located at
		//p1 and p2 respectively. There are, of course, two solutions to this problem. The calling application can differentiate between these
		//by passing in n or -n for the plane normal.
		
		//this is the distance between p1 and p2. If it is > r1+r2, then we have no solutions. To be nice about it,
		//we will set r to r1+r2 - the behaviour will be to reach as much as possible, even though you don't hit the target
		double r = Vector3d(p1, p2).length();
		if (r > r1+r2)
			r = r1 + r2;
		//this is the length of the vector starting at p1 and going to the midpoint between p1 and p2
		double a = (r1 * r1 - r2 * r2 + r * r) / (2 * r);
		//and this is the distance from the midpoint of p1-p2 to the intersection point
		double h = sqrt(r1*r1 - a*a);
		//now we need to get the two directions needed to reconstruct the intersection point
		Vector3d d1 = Vector3d(p1, p2).toUnit();
		Vector3d d2 = d1.crossProductWith(n).toUnit();

		//and now get the intersection point
		Point3d p = p1 + d1 * a + d2 * (-h);

		return p;
	}

	/**
		This method determines the relative orientation between the parent link and its grandparent- this boils down to solving for the orientation 
		that aligns two coordinate frames, where we know two perpendicular vectors from each that map to the other. 

		Input:
			vGlobal - parent's v expressed in grandparent coordinates
			nGlobal	- this is the rotation axis that is used for the relative rotation between the child and the parent joint, expressed in 
					  grandparent coordinates
			vLocal  - parent's v expressed in the parent's local coordinates
			nLocal  - this is the rotation axis that is used for the relative rotation between the child and the parent joint, expressed in 
					  parent's local coordinates
		Output:
			q		- the relative orientation between the parent and the grandparent (i.e. transforms vectors from parent coordinates to grandparent coordinates).
	*/
	static inline Quaternion getParentOrientation(const Vector3d& vGlobal, const Vector3d& nGlobal, Vector3d vLocal, const Vector3d& nLocal){
		Quaternion q;

		//first off, compute the quaternion that rotates nLocal into nGlobal
		Vector3d axis = nLocal.crossProductWith(nGlobal);
		axis.toUnit();
		double ang = nLocal.angleWith(nGlobal);

		q = Quaternion::getRotationQuaternion(ang, axis);

		//now q rotates nLocal into nGlobal. We now need an aditional orientation that aligns vLocal to vGlobal

		//one thing to note: nLocal is perpendicular to vLocal so q*nLocal is perpendicular to q*vLocal. Also, q*nLocal is aligned to nGlobal,
		//so nGlobal is perpendicular to both q*vLocal and vGlobal, which means that this rotation better be about vGlobal!!!
		vLocal = q.rotate(vLocal);
		axis = vLocal.crossProductWith(vGlobal);
		axis.toUnit();
		ang = vLocal.angleWith(vGlobal);

		q = Quaternion::getRotationQuaternion(ang, axis) * q;

		return q;
	}


	/**
		This method determines the rotation angle about the axis n for the child link, relative to the orientation of the parent
		Input (all quantities need to be measured in the same coordinate frame, ideally the parent's):
			vParent - the v qunatity for the parent
			vChild  - the v quntity for the child (vector between the child joint and the end effector).
			n		- the axis of rotation
		Output:
			q		- the relative orientation between the child and parent frames
	*/
	static inline double getChildRotationAngle(const Vector3d& vParent, const Vector3d& vChild, const Vector3d n){
		//compute the angle between the vectors (p1, p) and (p, p2), and that's our result
		double angle = vParent.angleWith(vChild);
		if (vParent.crossProductWith(vChild).dotProductWith(n)<0)
			angle = -angle;

		return angle;
	}

};




/**
	This is a generic class for a linkage composed of two links and one joint that is to be controlled by the analytical IK solver. In addition
	to the child joint, the parent's orientation, relative to either it's parent or the world, is also computed. Classes extending this one are
	responsible with the handeling of the parent's 'joint' information.
*/
class SimpleIKLinkage{
public:
	SimpleIKLinkage(Character* bip, Joint* joint, int jChildIndex, int jParentIndex){
		if (bip == NULL || joint == NULL || jChildIndex<0 || jParentIndex<0)
			throwError("The character needs to be passed in as a parameter, and the joint(s) needs to be valid!");
		this->bip = bip;
		this->joint = joint;
		this->parent = joint->getParent();
		this->child = joint->getChild();
		this->jChildIndex = jChildIndex;
		this->jParentIndex = jParentIndex;
	}

	/**
		this method is used to compute the IK solution (orientation of the parent relative to its parent, and orientation
		of the child relative to the parent), using the information stored in the data members.
	*/
	void getIKOrientations(Quaternion* pOrientation, Quaternion* cOrientation){
		//vector from the point of revolution of the parent to the child joint, and its length
		Vector3d vParentP = Vector3d(parentPointOfRevolutionP, childPointOfRevolutionP);
		double rp = vParentP.length();
		//vector from the child's point of revolution to the end effector
		Vector3d vChildC = Vector3d(childPointOfRevolutionC, endEffectorC);
		double rc = vChildC.length();

		//adjust the world-coordinates rotation axis (normal to the plane of rotation) so that the vector from the
		//parent's point of revolution to the desired end effector lies in the plane of rotation (perpendicular to
		//the rotation axis).

		Point3d parentPointOfRevolutionW = parent->getWorldCoordinates(parentPointOfRevolutionP);
		Vector3d line = Vector3d(parentPointOfRevolutionW, desiredEndEffectorW);
		Vector3d temp = rotAxisW.crossProductWith(line);
		rotAxisW = line.crossProductWith(temp);
		rotAxisW.toUnit();

		//now compute the location of the child joint, in world coordinates
		Vector3d solvedJointPosW = AnalyticalIK::solve(parentPointOfRevolutionW, desiredEndEffectorW, rotAxisW, rp, rc);
		Vector3d vParentW = Vector3d(parentPointOfRevolutionW, solvedJointPosW);
		Vector3d vChildW = Vector3d(solvedJointPosW, desiredEndEffectorW);

		//now we need to solve for the orientations of the parent and child
		//if the parent has 0 relative orientation to the grandparent (default), then the grandparent's orientation is also the parent's
		Quaternion worldToDefaultParent = grandParentOrientation.getComplexConjugate();
		*pOrientation = AnalyticalIK::getParentOrientation(worldToDefaultParent.rotate(vParentW), worldToDefaultParent.rotate(rotAxisW), vParentP, rotAxisP).toUnit();

		double childAngle = AnalyticalIK::getChildRotationAngle(vParentW, vChildW, rotAxisW);
		*cOrientation = Quaternion::getRotationQuaternion(childAngle, rotAxisP);
	}
protected:
	//first of all, we need to know which character this joint belongs to
	Character* bip;
	//keep track of which joint is between the parent and the child links
	Joint* joint;
	//we also need to know the index of the joint in the character's state, so that we
	//can set/read the relative orientation
	//and keep a reference to the child and the parent links, for easy access
	ArticulatedRigidBody* child;
	ArticulatedRigidBody* parent;
	//also know the index of the main joint in the reduced character state
	int jChildIndex;
	//and keep track of the parent's joint position, if applicable
	int jParentIndex;

	/*-----------------------------------------------------------------------------------------------------------------------------*
     *	these are the data members that need to be properly initialized before solving for the parent and child orientation        *
	 *-----------------------------------------------------------------------------------------------------------------------------*/

	//this is the axis of rotation between the parent and child links, expressed in world coordinates
	Vector3d rotAxisW;
	//and the rotation axis expressed in parent coordinates
	Vector3d rotAxisP;
	//keep track of the point of revolution of the parent (e.g. location of the shoulder or hip), stored in the parent's coordinates
	Point3d parentPointOfRevolutionP;
	//keep track of the point of revolution of the child (e.g. location of the elbow or knee), stored in the parent's coordinates
	Point3d childPointOfRevolutionP;
	//and store it in child coordinates as well
	Point3d childPointOfRevolutionC;
	//and keep track of the child's end effector location - store it in child coordinates
	Point3d endEffectorC;
	//and this is the desired end effector position, stored in world coordinates
	Point3d desiredEndEffectorW;
	//and this is the orientation of the grandparent - be it a link or the world
	Quaternion grandParentOrientation;
};


/**
	This class is used to store the relevant information regarding a character's spine
*/
class HumanoidIKSpine{
private:
	//The spine is assumed (for now) to be made up of only the lower body (i.e. root or pelvis) and the upper body (chest and upper back)
	Joint* spineJoint;
	//store in here the min and max angle for the elbow joint, so that we don't have to access the joint all the time
	double minAngle;
	double maxAngle;

};

/**
	This class is used to store the relevant information regarding a character's arm
*/
class HumanoidIKArm : public SimpleIKLinkage{
public:
	//this is the constructor - initialize the desired quantities
	HumanoidIKArm(Character* bip, char* jMainName, char* jParentName) : SimpleIKLinkage(bip, bip->getJointByName(jMainName), bip->getJointIndex(jMainName), bip->getJointIndex(jParentName)){
		//compute the length of the upper arm
		parentPointOfRevolutionP = parent->getParentJoint()->getChildJointPosition();
		childPointOfRevolutionP = joint->getParentJointPosition();
		childPointOfRevolutionC = joint->getChildJointPosition();
		//these bipeds don't have actual wrists, so we'll estimate their position
		endEffectorC = -Vector3d(joint->getChildJointPosition());
		if (joint->getJointType() != HINGE_JOINT)
			throwError("The elbow joint is expected to be a universal joint!!");

		rotAxisP = ((HingeJoint*)joint)->getRotAxisA();
//		minAngle = ((HingeJoint*)joint)->minAngleA;
//		maxAngle = ((HingeJoint*)joint)->maxAngleA;
	}

	//this method is used to compute the desired relative orientations of the upperarm and of the forearm (i.e. the shoulder and elbow joints)
	//so that the wrist is located at the desired, world-coordinates position specified
	void setIKOrientations(Point3d desiredWristWorldPosition){
		//set up the needed variables
		desiredEndEffectorW = desiredWristWorldPosition;
		grandParentOrientation = parent->getParentJoint()->getParent()->getOrientation();
//		rotAxisW = joint->getParent()->getWorldCoordinates(rotAxisP);
		rotAxisW = grandParentOrientation.rotate(rotAxisP);


		//get the orientations
		Quaternion pOrientation, cOrientation;
		getIKOrientations(&pOrientation, &cOrientation);
		
		//and apply them
		ReducedCharacterStateArray state;
		bip->getState(&state);
		ReducedCharacterState rs(&state);
		rs.setJointRelativeOrientation(pOrientation, jParentIndex);
		rs.setJointRelativeOrientation(cOrientation, jChildIndex);
		bip->setState(&state);
	}


/*
	//this method is used to compute the desired relative orientations of the upperarm and of the forearm (i.e. the shoulder and elbow joints)
	//so that the wrist is located at the desired, world-coordinates position specified
	void setIKOrientations(Point3d desiredWristWorldPosition){
		//first of all, we need to compute, in world coordinates, the current position of the shoulder
		Point3d shoulderWorldPos = joint->getParent()->getWorldCoordinates(shoulderPos);
		Vector3d line = Vector3d(shoulderWorldPos, desiredWristWorldPosition);
		//now compute the rotation axis, in world coordinates, so that it is perpendicular to the desiredWristWorldPosition
		Vector3d n = joint->getParent()->getWorldCoordinates(rotAxis);
		Vector3d temp = n.crossProductWith(line);
		n = line.crossProductWith(temp);
		n.toUnit();

		//now compute the location of the elbow, in world coordinates
		Vector3d solvedElbowPos = AnalyticalIK::solve(shoulderWorldPos, desiredWristWorldPosition, n, rp, rc);

		Vector3d test = Vector3d(shoulderWorldPos, solvedElbowPos);
		Vector3d test2 = Vector3d(solvedElbowPos, desiredWristWorldPosition);
		double testLength = test.length();
		double testLength2 = test2.length();
		double testProduct = test.dotProductWith(n);
		double testProduct2 = test2.dotProductWith(n);

		//we need to try out and see what the elbow position would be, if the joint relative orientation (between the upper arm and its parent) is the identity
		Quaternion tempOrientation = parent->getOrientation();
		Point3d tempPos = parent->getLocalCoordinates(solvedElbowPos);
		parent->setOrientation(parent->getParentJoint()->getParent()->getOrientation());
		tempPos = joint->getParent()->getWorldCoordinates(tempPos);
		parent->setOrientation(tempOrientation);

		Quaternion parentDefaultOrientation = parent->getParentJoint()->getParent()->getOrientation();

		//now we need to solve for the orientations of the elbow and shoulder
		Quaternion pOrientation = AnalyticalIK::getParentOrientation(parentDefaultOrientation.getComplexConjugate().rotate(Vector3d(shoulderWorldPos, solvedElbowPos)), parentDefaultOrientation.getComplexConjugate().rotate(n), Vector3d(shoulderPos, elbowPos), rotAxis);
//		pOrientation = parentDefaultOrientation.getComplexConjugate() * pOrientation;

		double aC = AnalyticalIK::getChildRotationAngle(Vector3d(shoulderWorldPos,solvedElbowPos), Vector3d(solvedElbowPos, desiredWristWorldPosition), n);

		//this is the rotation axis, expressed in local coordinates
		Vector3d rotAxisLocal = parent->getParentJoint()->getParent()->getLocalCoordinates(n);

		//now apply the relative orientations...
		DynamicArray<double> state;
		bip->getState(&state);
		ReducedCharacterState rs(&state);
		rs.setJointRelativeOrientation(pOrientation.toUnit(), jParentIndex);
		//rotAxis is already in parents coordinates
		rs.setJointRelativeOrientation(Quaternion::getRotationQuaternion(aC, rotAxis), jMainIndex);
		bip->setState(&state);


		//test out the new elbow position - see if it ended up where it should have been...
		Point3d newElbowPos = parent->getWorldCoordinates(elbowPos);
	}
*/
private:
	//store in here the min and max angle for the elbow joint, so that we don't have to access the joint all the time
	double minAngle;
	double maxAngle;
};

/**
	This class is used to store the relevant information regarding a character's leg
*/
class HumanoidIKLeg{
private:
	//keep track of the elbow joint, and with this we can get to the lower and upper arm
	Joint* knee;
	//also keep track of the ankle joint - we may need to make sure that the word relative orientation for this stays constant while we do the IK stuff
	Joint* ankle;
	//store in here the min and max angle for the elbow joint, so that we don't have to access the joint all the time
	double minAngle;
	double maxAngle;
	//keep track of the location of the shoulder - store it in the upper arm's local coordinates
	Point3d hipPos;
	//and keep track of the wrist location - store it in the lower arm's local coordinates
	Point3d anklePos;
};


/**
	This class is used to implement method that are specific for the problem of inverse kinematics. The character that these methods work on
	is strictly limited to have a certain structure. This is because fast analytical methods can be used if certain assumption can be made about
	the character (for instance that the character has elbow and knee joints with one degree of freedom that are the only joints in the arms and legs).
*/
class HumanoidIKCharacter{
private:
	//this is the character that the IK module works on
	Character* biped;
	//we'll make sure that we keep track of the left and right arm and leg, the torso, etc. Store them here for easy access (and also enforcing the assumptions).
	HumanoidIKArm *leftArm;
	HumanoidIKArm *rightArm;
	HumanoidIKLeg *leftLeg;
	HumanoidIKLeg *rightLeg;


public:
	HumanoidIKCharacter(Character* bip);
	~HumanoidIKCharacter(void);

	void setRightArmTarget(Point3d p);
};

