#pragma once

#include <Core/BehaviourController.h>

/**
	container for parameter settings for simple walking styles
*/
class SimpleStyleParameters{
public:
	double torsoLeanSagittal;
	double kneeBend;
	double duckFootedness;
	double elbowBend;
	double stepHeight;
	double shoulderTwist;
	double shoulderCoronal;
	double shoulderSagittal;
	
public:
	SimpleStyleParameters(void);
	~SimpleStyleParameters(void);

	void applyStyleParameters(BehaviourController* bc);

	/**
		if t=1, it's all this style. If it's 0, it's all the other...
	*/
	void applyInterpolatedStyleParameters(BehaviourController* bc, double t, SimpleStyleParameters* other);

};


