#include "articulatedrigidbody.h"
#include <Physics/Joint.h>


ArticulatedRigidBody::ArticulatedRigidBody(void){
	this->pJoint = NULL;
	this->AFParent = NULL;
}

ArticulatedRigidBody::~ArticulatedRigidBody(void){
}

/**
	This method draws the current rigid body.
*/
void ArticulatedRigidBody::draw(int flags){

	if (flags & SHOW_ABSTRACT_VIEW_SKELETON){
		GLboolean lighting = glIsEnabled(GL_LIGHTING);
		if (lighting){
			glDisable(GL_LIGHTING);
			
			Point3d start = state.position;

			if (this->pJoint)
				//GLUtils::drawCylinder(0.005, Vector3d(state.position, getWorldCoordinates(this->pJoint->cJPos)), state.position, 6);
				start = getWorldCoordinates(this->pJoint->cJPos);

			glColor3d(0,0,0);
			GLUtils::drawSphere(start, 0.01, 6);

			glColor3d(0.6,0,0);
			for (uint i=0;i<cJoints.size();i++){
				GLUtils::drawCylinder(0.005, Vector3d(start, getWorldCoordinates(this->cJoints[i]->pJPos)), start, 6);
			}
			if (cJoints.size() == 0)
				GLUtils::drawCylinder(0.005, Vector3d(start, state.position), start, 6);
			if (lighting)
				glEnable(GL_LIGHTING);
		}
	}

	RigidBody::draw(flags);
	
	if (!pJoint)
		return;

	if (flags & SHOW_JOINTS){
		//we will draw a little sphere at the location of the joint (we'll do it twice - once for the parent and one for the child. They should coincide
		//if the joint constraint is properly satisfied
		GLUtils::drawSphere(this->getWorldCoordinates(pJoint->cJPos), 0.02, 4);
		GLUtils::drawSphere(pJoint->parent->getWorldCoordinates(pJoint->pJPos), 0.02, 4);
	}
}

