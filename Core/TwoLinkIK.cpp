#include "TwoLinkIK.h"

TwoLinkIK::TwoLinkIK(void){
}

TwoLinkIK::~TwoLinkIK(void){
}

/**
	All quantities that are passed in as parameters here need to be expressed in the same "global" coordinate frame, unless otherwise noted:
		- p1: this is the location of the origin of the parent link
		- p2: this is the target location of the end effector on the child link
		- n: this is the default normal to the rotation plane (it will get modified as little as possible to account for the target location)

		- vParent: vector from parent origin to child origin, expressed in parent coordinates
		- nParent: this is the rotation axis, expressed in parent coordinates. The relative orientation between child and parent will be about this axis
		- vChild: vector from child origin, to position of the end effector, expressed in child coordinates

		Output:
			- qP: relative orientation of parent, relative to "global" coordinate frame
			- qC: relative orientation of child, relative to the parent coordinate frame


	NOTE: for now, the vector vChild is pretty much ignored. Only its length is taken into account. The axis that the child is lined up around is the same as the direction
	of the vector from the parent's origin to the child's origin (when there is zero relative orientation between the two, the axis has the same coordinates in both
	child and parent frame).
*/
void TwoLinkIK::getIKOrientations(const Point3d& p1, const Point3d& p2, const Vector3d& n, const Vector3d& vParent, const Vector3d& nParent, const Vector3d& vChild, Quaternion* qP, Quaternion* qC){
	//modify n so that it's perpendicular to the vector (p1, p2), and still as close as possible to n
	Vector3d line = Vector3d(p1, p2);
	Vector3d temp = n.crossProductWith(line);
	Vector3d nG = line.crossProductWith(temp);
	nG.toUnit();

	//now compute the location of the child origin, in "global" coordinates
	Vector3d solvedJointPosW = TwoLinkIK::solve(p1, p2, nG, vParent.length(), vChild.length());
	Vector3d vParentG = Vector3d(p1, solvedJointPosW);
	Vector3d vChildG = Vector3d(solvedJointPosW, p2);
	

	//now we need to solve for the orientations of the parent and child
	//if the parent has 0 relative orientation to the grandparent (default), then the grandparent's orientation is also the parent's
	*qP = TwoLinkIK::getParentOrientation(vParentG, nG, vParent, nParent);

	double childAngle = TwoLinkIK::getChildRotationAngle(vParentG, vChildG, nG);
	*qC = Quaternion::getRotationQuaternion(childAngle, nParent);
}
