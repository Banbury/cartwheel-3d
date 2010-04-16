#include "SimpleStyleParameters.h"
#include "Core/SimGlobals.h"

SimpleStyleParameters::SimpleStyleParameters(void){
	torsoLeanSagittal = 0;
	kneeBend = 0;
	duckFootedness = 0;
	stepHeight = 0;
	elbowBend = 0;
	shoulderTwist = 0;
	shoulderCoronal = 0;
	shoulderSagittal = 0;
}

SimpleStyleParameters::~SimpleStyleParameters(void){

}

void SimpleStyleParameters::applyStyleParameters(BehaviourController* bc){
	bc->requestUpperBodyPose(torsoLeanSagittal, 0, 0 );
	bc->requestKneeBend(kneeBend);
	bc->requestDuckFootedness(duckFootedness);
	bc->requestStepHeight(stepHeight);

	bc->setElbowAngles(elbowBend, elbowBend);
	bc->setShoulderAngles(shoulderTwist, shoulderTwist, shoulderCoronal, shoulderCoronal, shoulderSagittal, shoulderSagittal);
}



/**
	if t=1, it's all this style. If it's 0, it's all the other...
*/
void SimpleStyleParameters::applyInterpolatedStyleParameters(BehaviourController* bc, double t, SimpleStyleParameters* other){
	boundToRange(&t, 0, 1);
	if (IS_ZERO(t-1) || other == NULL) applyStyleParameters(bc);
	if (IS_ZERO(t)) other->applyStyleParameters(bc);

	SimpleStyleParameters interp;
	interp.torsoLeanSagittal = t * torsoLeanSagittal + (1-t) * other->torsoLeanSagittal;
	interp.kneeBend = t * kneeBend + (1-t) * other->kneeBend;
	interp.duckFootedness = t * duckFootedness + (1-t) * other->duckFootedness;
	interp.stepHeight = t * stepHeight + (1-t) * other->stepHeight;

	interp.elbowBend = t * elbowBend + (1-t) * other->elbowBend;
	interp.shoulderTwist = t * shoulderTwist + (1-t) * other->shoulderTwist;
	interp.shoulderCoronal = t * shoulderCoronal + (1-t) * other->shoulderCoronal;
	interp.shoulderSagittal = t * shoulderSagittal + (1-t) * other->shoulderSagittal;

	interp.applyStyleParameters(bc);
}


