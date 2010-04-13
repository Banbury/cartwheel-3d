#pragma once

#include <Core/Controller.h>

class VirtualModelController : public Controller{
public:
	VirtualModelController(Character* ch);
	~VirtualModelController(void);

	/**
		This method is used to compute the torques, based on the current and desired poses
	*/
	virtual void computeTorques(DynamicArray<ContactPoint> *cfs);


	/**
		This method is used to compute the torques that mimick the effect of applying a force on
		a rigid body, at some point. It works best if the end joint is connected to something that
		is grounded, otherwise (I think) this is just an approximation.

		This function works by making use of the formula:

		t = J' * f, where J' is dp/dq, where p is the position where the force is applied, q is
		'sorta' the relative orientation between links. It makes the connection between the velocity
		of the point p and the relative angular velocities at each joint. Here's an example of how to compute it.

		Assume: p = pBase + R1 * v1 + R2 * v2, where R1 is the matrix from link 1 to whatever pBase is specified in,
			and R2 is the rotation matrix from link 2 to whatever pBase is specified in, v1 is the point from link 1's
			origin to link 2's origin (in link 1 coordinates), and v2 is the vector from origin of link 2 to p 
			(in link 2 coordinates).

			dp/dt = d(R1 * v1)/dt + d(R2 * v2)/dt = d R1/dt * v1 + d R2/dt * v2, and dR/dt = wx * R, where wx is
			the cross product matrix associated with the angular velocity w
			so dp/dt = w1x * R1 * v1 + w2x * R2 * v2, and w2 = w1 + wRel
			
			= [-(R1*v1 + R2*v2)x   -(R2*v2)x ] [w1   wRel]', so the first matrix is the Jacobian.

			for a larger chain, we get:
				dp/dt = [-(R1*v1 + R2*v2 + ... + Rn*vn)x; -(R2*v2 + R3*v3 + ... + Rn*vn)x; ...; -(Rn*vn)x ] [w1; w2; ...; wn]'
				or
				dp/dt = [-p1x; -p2x; ...; -pnx ] [w1; w2; ...; wn]'
				where pi is the vector from the location of the ith joint to p;

				and all omegas are relative to the parent. The ith cross product vector in the jacobian is a vector from the
				location of the ith joint to the world location of the point where the force is applied.

			The first entry is the cross product matrix of the vector (in pBase coordinates) from the
			origin of link 1 to p, and the second entry is the vector (in pBase coordinates) from
			the origin of link 2 to p (and therein lies the general way of writing this).
	*/
	void computeJointTorquesEquivalentToForce(Joint* start, const Point3d& pLocal, const Vector3d& fGlobal, Joint* end);

};
