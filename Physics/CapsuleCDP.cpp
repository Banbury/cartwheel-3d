#include ".\capsulecdp.h"
#include <GLUtils/GLUtils.h>
#include <Physics/SphereCDP.h>
#include <Physics/PlaneCDP.h>
#include <MathLib/Segment.h>
#include <Physics/RigidBody.h>


CapsuleCDP::~CapsuleCDP(void){
}


/**
	draw an outline of the capsule
*/

void CapsuleCDP::draw(){
//	GLUtils::drawCylinder(this->c.radius, Vector3d(this->c.p1, this->c.p2), this->c.p1, 6);
//	GLUtils::drawSphere(this->c.p1, this->c.radius, 5);
//	GLUtils::drawSphere(this->c.p2, this->c.radius, 5);
	GLUtils::drawCapsule(this->c.radius,Vector3d(this->c.p1, this->c.p2), this->c.p1, 6);
}

void CapsuleCDP::updateToWorldPrimitive(){
	wC.p1 = bdy->getWorldCoordinates(c.p1);
	wC.p2 = bdy->getWorldCoordinates(c.p2);
	wC.radius = c.radius;
}

int CapsuleCDP::computeCollisionsWithSphereCDP(SphereCDP* sp,  DynamicArray<ContactPoint> *cps){
	return getContactPoints(&this->wC, &sp->wS, cps);
}

int CapsuleCDP::computeCollisionsWithPlaneCDP(PlaneCDP* p,  DynamicArray<ContactPoint> *cps){
	return getContactPoints(&this->wC, &p->wP, cps);
}

