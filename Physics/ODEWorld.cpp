#include "ODEWorld.h"
#include <Utils/utils.h>
#include <Physics/Joint.h>
#include <Physics/StiffJoint.h>
#include <Physics/HingeJoint.h>
#include <Physics/UniversalJoint.h>
#include <Physics/BallInSocketJoint.h>
#include <Physics/PhysicsGlobals.h>

/**
	Default constructor
*/
ODEWorld::ODEWorld() : World(){
	setupWorld();
}

/**
	destructor
*/
ODEWorld::~ODEWorld(void){
	//destroy the ODE physical world, simulation space and joint group
	
	dCloseODE();
}


void ODEWorld::destroyWorld() {
	delete[] cps;
	delete pcQuery;
	dJointGroupDestroy(contactGroupID);
	dSpaceDestroy(spaceID);
	dWorldDestroy(worldID);	
	odeToRbs.clear();
	World::destroyWorld();
}

void ODEWorld::setupWorld() {
	int maxCont = 4;

    worldID = dWorldCreate();

	//set a few of the constants that ODE needs to be aware of
	dWorldSetContactSurfaceLayer(worldID,0.001);							// the ammount of interpenetration allowed between objects
	dWorldSetContactMaxCorrectingVel(worldID, 1.0);							// maximum velocity that contacts are allowed to generate  

	//set the gravity...
	Vector3d gravity = PhysicsGlobals::up * PhysicsGlobals::gravity;
	dWorldSetGravity(worldID, gravity.x, gravity.y, gravity.z);	
	
	//Initialize the world, simulation space and joint groups
    spaceID = dHashSpaceCreate(0);
	contactGroupID = dJointGroupCreate(0);

	//make sure that when we destroy the space group, we destroy all the geoms inside it
	dSpaceSetCleanup(spaceID, 1);

	//allocate the space for the contacts;
	maxContactCount = maxCont;
	cps = new dContact[maxContactCount];

	pcQuery = NULL;

	pcQuery = new PreCollisionQuery();
}

void ODEWorld::destroyAllObjects() {
	destroyWorld();
	setupWorld();
}

/**
	this method is used to copy the state of the ith rigid body to its ode counterpart.
*/
void ODEWorld::setODEStateFromRB(int i){
	if (i<0 || (uint)i>=odeToRbs.size())
		return;

	//if it is a locked object, we update its CDPS
	if (odeToRbs[i].rb->isLocked() == true) {

		for (uint j=0;j<odeToRbs[i].collisionVolumes.size();j++){
			dGeomID t = odeToRbs[i].collisionVolumes[j];
			dGeomSetPosition(t, odeToRbs[i].rb->state.position.x, odeToRbs[i].rb->state.position.y, odeToRbs[i].rb->state.position.z);
			dQuaternion q;
			q[0] = odeToRbs[i].rb->state.orientation.s;
			q[1] = odeToRbs[i].rb->state.orientation.v.x;
			q[2] = odeToRbs[i].rb->state.orientation.v.y;
			q[3] = odeToRbs[i].rb->state.orientation.v.z;
			dGeomSetQuaternion(t, q);
		}
		return;
	}

	dQuaternion tempQ;
	tempQ[0] = odeToRbs[i].rb->state.orientation.s;
	tempQ[1] = odeToRbs[i].rb->state.orientation.v.x;
	tempQ[2] = odeToRbs[i].rb->state.orientation.v.y;
	tempQ[3] = odeToRbs[i].rb->state.orientation.v.z;
	
	dBodySetPosition(odeToRbs[i].id, odeToRbs[i].rb->state.position.x, odeToRbs[i].rb->state.position.y, odeToRbs[i].rb->state.position.z);
	dBodySetQuaternion(odeToRbs[i].id, tempQ);
	dBodySetLinearVel(odeToRbs[i].id, odeToRbs[i].rb->state.velocity.x, odeToRbs[i].rb->state.velocity.y, odeToRbs[i].rb->state.velocity.z);
	dBodySetAngularVel(odeToRbs[i].id, odeToRbs[i].rb->state.angularVelocity.x, odeToRbs[i].rb->state.angularVelocity.y, odeToRbs[i].rb->state.angularVelocity.z);
}

/**
	this method is used to copy the state of the ith rigid body, from the ode object to its rigid body counterpart 
*/
void ODEWorld::setRBStateFromODE(int i){
	const dReal *tempData;

	//if it is a locked object, we won't do anything about it
	if (odeToRbs[i].rb->isLocked() == true)
		return;

	//if the objects is supposed to be planar, make sure we don't let drift accumulate
	if (odeToRbs[i].rb->props.isPlanar){
	   const dReal *rot = dBodyGetAngularVel(odeToRbs[i].id);
	   const dReal *quat_ptr;
	   dReal quat[4], quat_len;
	   quat_ptr = dBodyGetQuaternion( odeToRbs[i].id );
	   quat[0] = quat_ptr[0];
	   quat[1] = quat_ptr[1];
	   quat[2] = 0; 
	   quat[3] = 0; 
	   quat_len = sqrt( quat[0] * quat[0] + quat[1] * quat[1] );
	   quat[0] /= quat_len;
	   quat[1] /= quat_len;
	   dBodySetQuaternion( odeToRbs[i].id, quat );
	   dBodySetAngularVel( odeToRbs[i].id, rot[0], 0, 0);
	}


	tempData = dBodyGetPosition(odeToRbs[i].id);
	odeToRbs[i].rb->state.position.x = tempData[0];
	odeToRbs[i].rb->state.position.y = tempData[1];
	odeToRbs[i].rb->state.position.z = tempData[2];

	tempData = dBodyGetQuaternion(odeToRbs[i].id);
	odeToRbs[i].rb->state.orientation.s = tempData[0];
	odeToRbs[i].rb->state.orientation.v.x = tempData[1];
	odeToRbs[i].rb->state.orientation.v.y = tempData[2];
	odeToRbs[i].rb->state.orientation.v.z = tempData[3];

	tempData = dBodyGetLinearVel(odeToRbs[i].id);
	odeToRbs[i].rb->state.velocity.x = tempData[0];
	odeToRbs[i].rb->state.velocity.y = tempData[1];
	odeToRbs[i].rb->state.velocity.z = tempData[2];

	tempData = dBodyGetAngularVel(odeToRbs[i].id);
	odeToRbs[i].rb->state.angularVelocity.x = tempData[0];
	odeToRbs[i].rb->state.angularVelocity.y = tempData[1];
	odeToRbs[i].rb->state.angularVelocity.z = tempData[2];
}


/**
	this method is used to set up an ODE sphere geom. NOTE: ODE only allows planes to
	be specified in world coordinates, not attached to a body, so we need to fix it once and
	for all.
*/
dGeomID ODEWorld::getPlaneGeom(PlaneCDP* p, RigidBody* parent){
	//and create the ground plane
	Vector3d n = parent->getWorldCoordinates(p->getNormal());
	Vector3d o = Vector3d(parent->getWorldCoordinates(p->getOrigin()));
	dGeomID g = dCreatePlane(spaceID, n.x, n.y, n.z, o.dotProductWith(n));
	return g;
}

/**
	this method is used to set up an ODE sphere geom. It is properly placed in body coordinates.
*/
dGeomID ODEWorld::getSphereGeom(SphereCDP* s){
	dGeomID g = dCreateSphere(0, s->getRadius());
	Point3d c = s->getCenter();
	dGeomSetPosition(g, c.x, c.y, c.z);
	return g;
}


/**
	this method is used to set up an ODE box geom. It is properly placed in body coordinates.
*/
dGeomID ODEWorld::getBoxGeom(BoxCDP* b){
	dGeomID g = dCreateBox(0, b->getXLen(), b->getYLen(), b->getZLen());
	Point3d c = b->getCenter();
	dGeomSetPosition(g, c.x, c.y, c.z);
	return g;
}

/**
	this method is used to set up an ODE sphere geom. It is properly placed in body coordinates.
*/
dGeomID ODEWorld::getCapsuleGeom(CapsuleCDP* c){
	Point3d a = c->getPoint1();
	Point3d b = c->getPoint2();
	Vector3d ab(a, b);
	dGeomID g = dCreateCCylinder(0, c->getRadius(), ab.length());
	
	Point3d cen = a + ab/2.0;
	dGeomSetPosition(g, cen.x, cen.y, cen.z);


	//now, the default orientation for this is along the z-axis. We need to rotate this to make it match the direction
	//of ab, so we need an angle and an axis...
	Vector3d defA(0, 0, 1);

	Vector3d axis = defA.crossProductWith(ab);
	axis.toUnit();
	double rotAngle = defA.angleWith(ab);

	Quaternion relOrientation = Quaternion::getRotationQuaternion(rotAngle, axis);
	
	dQuaternion q;
	q[0] = relOrientation.s;
	q[1] = relOrientation.v.x;
	q[2] = relOrientation.v.y;
	q[3] = relOrientation.v.z;

	dGeomSetQuaternion(g, q);

	return g;
}

/**
	This method is used to set up an ode hinge joint, based on the information in the hinge joint passed in as a parameter
*/
void ODEWorld::setupODEFixedJoint(StiffJoint* hj){
	dJointID j = dJointCreateFixed(worldID, 0);
	dJointAttach(j, odeToRbs[(int)(hj->child->id)].id, odeToRbs[(int)(hj->parent->id)].id);
	dJointSetFixed(j);
}

/**
	This method is used to set up an ode hinge joint, based on the information in the hinge joint passed in as a parameter
*/
void ODEWorld::setupODEHingeJoint(HingeJoint* hj){
	dJointID j = dJointCreateHinge(worldID, 0);
	dJointAttach(j, odeToRbs[(int)(hj->child->id)].id, odeToRbs[(int)(hj->parent->id)].id);
	Point3d p = hj->child->getWorldCoordinates(hj->cJPos);
	dJointSetHingeAnchor(j, p.x, p.y, p.z);
	Vector3d a = hj->parent->getWorldCoordinates(hj->a);
	dJointSetHingeAxis(j, a.x, a.y, a.z);

	//now set the joint limits
	if (hj->useJointLimits == false)
		return;

	dJointSetHingeParam(j, dParamLoStop, hj->minAngle);
	dJointSetHingeParam(j, dParamHiStop, hj->maxAngle);
}

/**
	This method is used to set up an ode universal joint, based on the information in the universal joint passed in as a parameter
*/
void ODEWorld::setupODEUniversalJoint(UniversalJoint* uj){
	dJointID j = dJointCreateUniversal(worldID, 0);
	dJointAttach(j, odeToRbs[(int)(uj->child->id)].id, odeToRbs[(int)(uj->parent->id)].id);
	Point3d p = uj->child->getWorldCoordinates(uj->cJPos);
	dJointSetUniversalAnchor(j, p.x, p.y, p.z);

	Vector3d a = uj->parent->getWorldCoordinates(uj->a);
	Vector3d b = uj->child->getWorldCoordinates(uj->b);

	dJointSetUniversalAxis1(j, a.x, a.y, a.z);
	dJointSetUniversalAxis2(j, b.x, b.y, b.z);

	//now set the joint limits
	if (uj->useJointLimits == false)
		return;

	dJointSetUniversalParam(j, dParamLoStop, uj->minAngleA);
	dJointSetUniversalParam(j, dParamHiStop, uj->maxAngleA);
	dJointSetUniversalParam(j, dParamLoStop2, uj->minAngleB);
	dJointSetUniversalParam(j, dParamHiStop2, uj->maxAngleB);
}

/**
	This method is used to set up an ode ball-and-socket joint, based on the information in the ball in socket joint passed in as a parameter
*/
void ODEWorld::setupODEBallAndSocketJoint(BallInSocketJoint* basj){
	dJointID j = dJointCreateBall(worldID, 0);
	dJointAttach(j, odeToRbs[(int)(basj->child->id)].id, odeToRbs[(int)(basj->parent->id)].id);
	Point3d p = basj->child->getWorldCoordinates(basj->cJPos);
	//now we'll set the world position of the ball-and-socket joint. It is important that the bodies are placed in the world
	//properly at this point
	dJointSetBallAnchor(j, p.x, p.y, p.z);

	//now deal with the joint limits
	if (basj->useJointLimits == false)
		return;

	Vector3d a = basj->parent->getWorldCoordinates(basj->swingAxis1);
	Vector3d b =  basj->child->getWorldCoordinates(basj->twistAxis);

	//we'll assume that:
	//b is the twisting axis of the joint, and the joint limits will be (in magnitude) less than 90 degrees, otherwise
	//the simulation will go unstable!!!


	dJointID aMotor = dJointCreateAMotor(worldID, 0);
	dJointAttach(aMotor, odeToRbs[(int)(basj->parent->id)].id, odeToRbs[(int)(basj->child->id)].id);
	dJointSetAMotorMode(aMotor, dAMotorEuler);

	dJointSetAMotorParam(aMotor, dParamStopCFM, 0.1);
	dJointSetAMotorParam(aMotor, dParamStopCFM2, 0.1);
	dJointSetAMotorParam(aMotor, dParamStopCFM3, 0.1);


	dJointSetAMotorAxis (aMotor, 0, 1, a.x, a.y, a.z);
	dJointSetAMotorAxis (aMotor, 2, 2, b.x, b.y, b.z);

	dJointSetAMotorParam(aMotor, dParamLoStop, basj->minSwingAngle1);
	dJointSetAMotorParam(aMotor, dParamHiStop, basj->maxSwingAngle1);
	
	dJointSetAMotorParam(aMotor, dParamLoStop2, basj->minSwingAngle2);
	dJointSetAMotorParam(aMotor, dParamHiStop2, basj->maxSwingAngle1);

	dJointSetAMotorParam(aMotor, dParamLoStop3, basj->minTwistAngle);
	dJointSetAMotorParam(aMotor, dParamHiStop3, basj->maxTwistAngle);
}

/**
	this method is used to create ODE geoms for all the collision primitives of the rigid body that is passed in as a paramter
*/
void ODEWorld::createODECollisionPrimitives(RigidBody* body, int index){
	//now we'll set up the body's collision detection primitives
	for (uint j=0;j<body->cdps.size();j++){
		int cdpType = body->cdps[j]->getType();

		//depending on the type of collision primitive, we'll now create g.
		dGeomID g;

		switch (cdpType){
			case SPHERE_CDP:
				g = getSphereGeom((SphereCDP*)body->cdps[j]);
				break;
			case CAPSULE_CDP:
				g = getCapsuleGeom((CapsuleCDP*)body->cdps[j]);
				break;
			case BOX_CDP:
				g = getBoxGeom((BoxCDP*)body->cdps[j]);
				break;
			case PLANE_CDP:
				//NOTE: only static objects can have planes as their collision primitives - if this isn't static, force it!!
				g = getPlaneGeom((PlaneCDP*)body->cdps[j], body);
				break;
			default:
				throwError("Ooppps... No collision detection primitive was created rb: %s, cdp: %d", body->name, j);
		}

		//now associate the geom to the rigid body that it belongs to, so that we can look up the properties we need later...
		dGeomSetData(g, body);

		//if it's a plane, it means it must be static, so we can't attach a transform to it...
		if (cdpType == PLANE_CDP)
			continue;

		//now we've created a geom for the current body. Note: g will be rotated relative to t, so that it is positioned
		//well in body coordinates, and then t will be attached to the body.
		dGeomID t = dCreateGeomTransform(spaceID);
		//make sure that when we destroy the transfromation, we destroy the encapsulated objects as well.
		dGeomTransformSetCleanup(t, 1);

		//associate the transform geom with the body as well
		dGeomSetData(t, body);

		//if the object is fixed, then we want the geometry to take into account the initial position and orientation of the rigid body
		if (body->isLocked() == true){
			dGeomSetPosition(t, body->state.position.x, body->state.position.y, body->state.position.z);
			dQuaternion q;
			q[0] = body->state.orientation.s;
			q[1] = body->state.orientation.v.x;
			q[2] = body->state.orientation.v.y;
			q[3] = body->state.orientation.v.z;
			dGeomSetQuaternion(t, q);
			// Save collision volumes for eventual updates
			odeToRbs[index].collisionVolumes.push_back( t );
		}

		dGeomTransformSetGeom(t, g);
		//now add t (which contains the correctly positioned geom) to the body, if we do really have an ODE body for it
		if (body->isLocked() == false)
			dGeomSetBody(t, odeToRbs[body->id].id);
	}
}


/**
	This method reads a list of rigid bodies from the specified file.
*/
void ODEWorld::loadRBsFromFile(char* fName){
	//make sure we don't go over the old articulated figures in case this method gets called multiple times.
	int index = objects.size();
	int index_afs = AFs.size();

	World::loadRBsFromFile(fName);

	// Add all non-articulated rigid bodies in ODE
	for (uint i=index;i<objects.size();i++){
		RigidBody* rigidBody = objects[i];
		// Add a placeholder in the odeToRbs mapping
		odeToRbs.push_back(ODE_RB_Map(0, rigidBody));
		if( !rigidBody->isArticulated() )
			linkRigidBodyToODE(objects.size()-1);
	}

	DynamicArray<Joint*> joints;

	// Check every newly added articulated figures
	for (uint i=index_afs;i<AFs.size();i++){

		// For each, add the articulated bodies they contain to ODE		
		for (uint j=0;j<objects.size();j++){
			if( !objects[j]->isArticulated() )
				continue;
			ArticulatedRigidBody* arb = (ArticulatedRigidBody*)objects[j];
			if( arb->getAFParent() == AFs[i] )
				linkRigidBodyToODE(j);
		}

		//now we will go through all the new joints, and create and link their ode equivalents
		joints.clear();
		AFs[i]->addJointsToList(&joints);
		for (uint j=0;j<joints.size();j++){
			//connect the joint to the two bodies
			int jointType = joints[j]->getJointType();
			switch (jointType){
				case STIFF_JOINT:
					setupODEFixedJoint((StiffJoint*)joints[j]);
					break;
				case BALL_IN_SOCKET_JOINT:
					setupODEBallAndSocketJoint((BallInSocketJoint*)joints[j]);
					break;
				case HINGE_JOINT:
					setupODEHingeJoint((HingeJoint*)joints[j]);
					break;
				case UNIVERSAL_JOINT:
					setupODEUniversalJoint((UniversalJoint*)joints[j]);
					break;
				default:
					throwError("Ooops.... Only BallAndSocket, Hinge, Universal and Stiff joints are currently supported.\n");
			}
		}
	}
}

/**
	This method adds one rigid body (not articulated).
*/
void ODEWorld::addRigidBody( RigidBody* rigidBody ) {

	World::addRigidBody(rigidBody);

	// Add a placeholder in the odeToRbs mapping
	odeToRbs.push_back(ODE_RB_Map(0, rigidBody));

	// Non-articulated rigid body are already well-positioned, link them to ODE
	if( !rigidBody->isArticulated() )
		linkRigidBodyToODE(objects.size()-1);
	// For articulated rigid bodies, we will only add them when (and if) they appear in an ArticulatedFigure


}

/**
	This method adds one rigid body (not articulated).
*/
void ODEWorld::addArticulatedFigure(ArticulatedFigure* articulatedFigure){
	World::addArticulatedFigure( articulatedFigure );

	// Add the articulated bodies contained into that figure to ODE		
	for (uint j=0;j<objects.size();j++){
		if( !objects[j]->isArticulated() )
			continue;
		ArticulatedRigidBody* arb = (ArticulatedRigidBody*)objects[j];
		if( arb->getAFParent() == articulatedFigure )
			linkRigidBodyToODE(j);
	}

	DynamicArray<Joint*> joints;

	//now we will go through all the new joints, and create and link their ode equivalents
	articulatedFigure->addJointsToList(&joints);
	for (uint j=0;j<joints.size();j++){
		//connect the joint to the two bodies
		int jointType = joints[j]->getJointType();
		switch (jointType){
			case STIFF_JOINT:
				setupODEFixedJoint((StiffJoint*)joints[j]);
				break;
			case BALL_IN_SOCKET_JOINT:
				setupODEBallAndSocketJoint((BallInSocketJoint*)joints[j]);
				break;
			case HINGE_JOINT:
				setupODEHingeJoint((HingeJoint*)joints[j]);
				break;
			case UNIVERSAL_JOINT:
				setupODEUniversalJoint((UniversalJoint*)joints[j]);
				break;
			default:
				throwError("Ooops.... Only BallAndSocket, Hinge, Universal and Stiff joints are currently supported.\n");
		}
	}
}

/**
	This methods creates an ODE object and links it to the passed RigidBody
*/
void ODEWorld::linkRigidBodyToODE( int index ) {
	
	RigidBody* rigidBody = odeToRbs[index].rb; 

	//CREATE AND LINK THE ODE BODY WITH OUR RIGID BODY
	//if the body is fixed, we'll only create the colission detection primitives
	if (!rigidBody->isLocked()){
		odeToRbs[index].id = dBodyCreate(worldID);
		//the ID of this rigid body will be its index in the 
		rigidBody->setBodyID( index );
		//we will use the user data of the object to store the index in this mapping as well, for easy retrieval
		dBodySetData(odeToRbs[index].id, (void*)index);
	}

	//if this is a planar object, make sure we constrain it to always stay planar
	if (rigidBody->props.isPlanar){
		dJointID j = dJointCreatePlane2D(worldID, 0);
		dJointAttach(j, odeToRbs[index].id, 0);
	}

	//PROCESS THE COLLISION PRIMITIVES OF THE BODY
	createODECollisionPrimitives(rigidBody, index);

	//SET THE INERTIAL PARAMETERS

	if (rigidBody->isLocked() == false){
		dMass m;

		//set the mass and principal moments of inertia for this object
		m.setZero();
		Vector3d principalMoments = odeToRbs[index].rb->getPMI();
		m.setParameters(odeToRbs[index].rb->getMass(), 0, 0, 0, 
			principalMoments.x, 
			principalMoments.y, 
			principalMoments.z, 
			0, 0, 0);

		dBodySetMass(odeToRbs[index].id, &m);

		setODEStateFromRB(index);
	}

}

/**
	this method is used to process the collision between the two objects passed in as parameters. More generally,
	it is used to determine if the collision should take place, and if so, it calls the method that generates the
	contact points.
*/
void ODEWorld::processCollisions(dGeomID o1, dGeomID o2){
    dBodyID b1, b2;
	RigidBody *rb1, *rb2;
    b1 = dGeomGetBody(o1);
    b2 = dGeomGetBody(o2);
    rb1 = (RigidBody*) dGeomGetData(o1);
    rb2 = (RigidBody*) dGeomGetData(o2);

	bool joined = b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact);

	if (pcQuery)
		if (pcQuery->shouldCheckForCollisions(rb1, rb2, joined) == false)
			return;

	//we'll use the minimum of the two coefficients of friction of the two bodies.
	double mu1 = rb1->getFrictionCoefficient();
	double mu2 = rb2->getFrictionCoefficient();
	double mu_to_use = min(mu1, mu2);
	double eps1 = rb1->getRestitutionCoefficient();
	double eps2 = rb2->getRestitutionCoefficient();
	double eps_to_use = min(eps1, eps2);

	int num_contacts = dCollide(o1,o2,maxContactCount,&(cps[0].geom), sizeof(dContact));

	double groundSoftness = 0, groundPenalty = 0;
	if (rb1){
		groundSoftness = rb1->props.groundSoftness;
		groundPenalty = rb1->props.groundPenalty;
	}else{
		groundSoftness = rb2->props.groundSoftness;
		groundPenalty = rb2->props.groundPenalty;
	}

	//fill in the missing properties for the contact points
	for (int i=0;i<num_contacts;i++){
		cps[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactBounce;
		cps[i].surface.mu = mu_to_use;
		cps[i].surface.bounce = eps_to_use;
		cps[i].surface.bounce_vel = 0.00001;

		cps[i].surface.soft_cfm = groundSoftness;
		cps[i].surface.soft_erp = groundPenalty;
	}

	// and now add them contact points to the simulation
	for (int i=0;i<num_contacts;i++){
		//create a joint, and link the two geometries.
		dJointID c = dJointCreateContact(worldID, contactGroupID, &cps[i]);
		dJointAttach(c, b1, b2);

		if (jointFeedbackCount >= MAX_CONTACT_FEEDBACK)
			tprintf("Warning: too many contacts are established. Some of them will not be reported.\n");
		else{
			if (contactPoints.size() != jointFeedbackCount){
				tprintf("Warning: Contact forces need to be cleared after each simulation, otherwise the results are not predictable.\n");
			}
			contactPoints.push_back(ContactPoint());
			//now we'll set up the feedback for this contact joint
			contactPoints[jointFeedbackCount].rb1 = rb1;
			contactPoints[jointFeedbackCount].rb2 = rb2;
			contactPoints[jointFeedbackCount].cp = Point3d(cps[i].geom.pos[0], cps[i].geom.pos[1], cps[i].geom.pos[2]);
			dJointSetFeedback(c,&(jointFeedback[jointFeedbackCount]));
			jointFeedbackCount++;
		}
	}
}

/**
	This method is used to set the state of all the rigid body in this collection.
*/
void ODEWorld::setState(DynamicArray<double>* state, int start){
	World::setState(state, start);
}

/**
	This method is a simple call back function that passes the message to the world whose objects are being acted upon. 
*/
void collisionCallBack(void* odeWorld, dGeomID o1, dGeomID o2){
	((ODEWorld*)odeWorld)->processCollisions(o1, o2);
}

//void runTestStep(dWorldID w, dReal stepsize);


/**
	run a testing method...
*/
void ODEWorld::runTest(){
/*
	//make sure that the state of the RB's is synchronized with the engine...
	setEngineStateFromRB();

	//restart the counter for the joint feedback terms
	jointFeedbackCount = 0;

	Timer t;
	t.restart();
	for (int i=0;i<10000000;i++){
		//and run the step...
		runTestStep(worldID, 1/2000.0);
	}
	tprintf("ODE: This took %lf s\n", t.timeEllapsed());
*/
}

/**
	This method is used to integrate the forward simulation in time.
*/
void ODEWorld::advanceInTime(double deltaT){
	if( deltaT <= 0 )
		return;

	//make sure that the state of the RB's is synchronized with the engine...
	setEngineStateFromRB();

	//restart the counter for the joint feedback terms
	jointFeedbackCount = 0;

	//go through all the rigid bodies in the world, and apply their external force
	for (uint j=0;j<objects.size();j++){
		if( objects[j]->isLocked() ) 
			continue;
		const Vector3d& f = objects[j]->externalForce;
		if( !f.isZeroVector() )
			dBodyAddForce(odeToRbs[objects[j]->id].id, f.x, f.y, f.z);
		const Vector3d& t = objects[j]->externalTorque;
		if( !t.isZeroVector() )
			dBodyAddTorque(odeToRbs[objects[j]->id].id, t.x, t.y, t.z);
	}

	//go through all the joints in the world, and apply their torques to the parent and child rb's
	for (uint j=0;j<jts.size();j++){
		Vector3d t = jts[j]->torque;
		//we will apply to the parent a positive torque, and to the child a negative torque
		dBodyAddTorque(odeToRbs[jts[j]->parent->id].id, t.x, t.y, t.z);
		dBodyAddTorque(odeToRbs[jts[j]->child->id].id, -t.x, -t.y, -t.z);
	}


	//clear the previous list of contact forces
	contactPoints.clear();

	//we need to determine the contact points first - delete the previous contacts
	dJointGroupEmpty(contactGroupID);
	//initiate the collision detection
	dSpaceCollide(spaceID, this, &collisionCallBack);

	//advance the simulation
	dWorldStep(worldID, deltaT);
//	dWorldQuickStep(worldQID, deltaT);
//	runTestStep(worldID, deltaT);

	//copy over the state of the ODE bodies to the rigid bodies...
	setRBStateFromEngine();

	//copy over the force information for the contact forces
	for (int i=0;i<jointFeedbackCount;i++){
		contactPoints[i].f = Vector3d(jointFeedback[i].f1[0], jointFeedback[i].f1[1], jointFeedback[i].f1[2]);
		//make sure that the force always points away from the static objects
		if (contactPoints[i].rb1->isLocked() && !contactPoints[i].rb2->isLocked()){
			contactPoints[i].f = contactPoints[i].f * (-1);
			RigidBody* tmpBdy = contactPoints[i].rb1;
			contactPoints[i].rb1 = contactPoints[i].rb2;
			contactPoints[i].rb2 = tmpBdy;
		}
	}
}

/**
	This method is used to integrate the forward simulation in time.
*/
void ODEWorld::testAdvanceInTime(double deltaT){
	//make sure that the state of the RB's is synchronized with the engine...
	setEngineStateFromRB();

	//restart the counter for the joint feedback terms
	jointFeedbackCount = 0;

	//go through all the joints in the world, and apply their torques to the parent and child rb's
	for (uint j=0;j<jts.size();j++){
		Vector3d t = jts[j]->torque;
		//we will apply to the parent a positive torque, and to the child a negative torque
		dBodyAddTorque(odeToRbs[jts[j]->parent->id].id, t.x, t.y, t.z);
		dBodyAddTorque(odeToRbs[jts[j]->child->id].id, -t.x, -t.y, -t.z);
	}

	//clear the previous list of contact forces
	contactPoints.clear();

	//we need to determine the contact points first - delete the previous contacts
	dJointGroupEmpty(contactGroupID);
	//initiate the collision detection
	dSpaceCollide(spaceID, this, &collisionCallBack);

	//advance the simulation
//	dWorldStep(worldID, deltaT);
	runTestStep(worldID, deltaT);


	//copy over the state of the ODE bodies to the rigid bodies...
	setRBStateFromEngine();

	//copy over the force information for the contact forces
	for (int i=0;i<jointFeedbackCount;i++){
		contactPoints[i].f = Vector3d(jointFeedback[i].f1[0], jointFeedback[i].f1[1], jointFeedback[i].f1[2]);
		//make sure that the force always points away from the static objects
		if (contactPoints[i].rb1->isLocked() && !contactPoints[i].rb2->isLocked()){
			contactPoints[i].f = contactPoints[i].f * (-1);
			RigidBody* tmpBdy = contactPoints[i].rb1;
			contactPoints[i].rb1 = contactPoints[i].rb2;
			contactPoints[i].rb2 = tmpBdy;
		}
	}
}


/**
	this method is used to transfer the state of the rigid bodies, from ODE to the rigid body wrapper
*/
void ODEWorld::setRBStateFromEngine(){
	//now update all the rigid bodies...
	for (uint i=0;i<objects.size();i++){
		setRBStateFromODE(i);
//		objects[i]->updateToWorldTransformation();
	}
}

/**
	this method is used to transfer the state of the rigid bodies, from the rigid body wrapper to ODE's rigid bodies
*/
void ODEWorld::setEngineStateFromRB(){
	//now update all the rigid bodies...
	for (uint i=0;i<objects.size();i++){
		setODEStateFromRB(i);
	}
}

/**
	this method applies a force to a rigid body, at the specified point. The point is specified in local coordinates,
	and the force is also specified in local coordinates.
*/
void ODEWorld::applyRelForceTo(RigidBody* b, const Vector3d& f, const Point3d& p){
	if (!b)
		return;
	dBodyAddRelForceAtRelPos(odeToRbs[b->id].id, f.x, f.y, f.z, p.x, p.y, p.z);
}

/**
	this method applies a force to a rigid body, at the specified point. The point is specified in local coordinates,
	and the force is specified in world coordinates.
*/
void ODEWorld::applyForceTo(RigidBody* b, const Vector3d& f, const Point3d& p){
	if (!b)
		return;
	dBodyAddForceAtRelPos(odeToRbs[b->id].id, f.x, f.y, f.z, p.x, p.y, p.z);
}


/**
	this method applies a torque to a rigid body. The torque is specified in world coordinates.
*/
void ODEWorld::applyTorqueTo(RigidBody* b, const Vector3d& t){
	if (!b)
		return;
	dBodyAddTorque(odeToRbs[b->id].id, t.x, t.y, t.z);
}
