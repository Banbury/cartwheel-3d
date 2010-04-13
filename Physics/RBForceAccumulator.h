#pragma once

#include <MathLib/Vector3d.h>

#include <Physics/PhysicsDll.h>

class PHYSICS_DECLSPEC RBForceAccumulator{
public:
	//this is the total net force acting on a body
	Vector3d netForce;
	//this is the total net torque (takes into account the torques due to forces as well)
	Vector3d netTorque;
public:
	RBForceAccumulator(void);
	~RBForceAccumulator(void);
};
