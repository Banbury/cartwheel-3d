#pragma once

#include <Core/BehaviourController.h>

class DuckController : public BehaviourController {
protected:
	double crouchLevel;
	double clearanceHeight;
public:
	DuckController(Character* b, IKVMCController* llc, WorldOracle* w = NULL);
	virtual ~DuckController(void);

	/**
		this method gets called at every simulation time step
	*/
	virtual void simStepPlan(double dt);

	/**
		this method determines the degree to which the character should be panicking
	*/
	virtual double getPanicLevel();

};
