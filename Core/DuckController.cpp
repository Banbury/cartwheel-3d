#include "DuckController.h"

DuckController::DuckController(Character* b, IKVMCController* llc, WorldOracle* w) : BehaviourController(b, llc, w){
	crouchLevel = 0;
	clearanceHeight = 1.2;
}

DuckController::~DuckController(void){

}

/**
	this method gets called at every simulation time step
*/
void DuckController::simStepPlan(double dt){
	BehaviourController::simStepPlan(dt);
	ArticulatedRigidBody* rb = bip->getARBByName("head");
	if (rb->getCMPosition().y > clearanceHeight)
		crouchLevel += dt/2;
//	else
//		crouchLevel -= dt/2;

	boundToRange(&crouchLevel, 0, 1);

	setUpperBodyPose(ubSagittalLean - crouchLevel * -1, ubCoronalLean - crouchLevel * -0.2, ubTwist);
	setKneeBend(kneeBend - crouchLevel * -2);
}

/**
	this method determines the degree to which the character should be panicking
*/
double DuckController::getPanicLevel(){
	//be extra careful...
	return 2 * BehaviourController::getPanicLevel();
}

