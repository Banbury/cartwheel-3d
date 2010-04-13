#pragma once

#include <mathLib/Vector3d.h>
#include <mathLib/Quaternion.h>
#include <mathLib/Point3d.h>
#include <mathLib/TransformationMatrix.h>

#include <GLUtils/GLUtilsDll.h>

/**
 * This class implements a simple open GL camera. This camera can be used to automatically follow a target, and rotate around it.
 */
class GLUTILS_DECLSPEC GLCamera{
private:
	//keep track of the orientation of the camera.
	Quaternion orientation;


	//to simplify things, we will keep track of the rotation using this vector. This way, we won't need to mix rotations about the x, y and z axis
	Vector3d rotations;
	//and also how far it is along its z axis (assuming that it is looking down the -ve z axis of its local frame)
	double camDistance;
	//we will also keep track of the point we are looking at, in world coordinates
	Point3d target;
	//this is the world to camera transformation matrix, recomputed at every step
	TransformationMatrix worldToCam;

	bool autoOrbit;

public:
	GLCamera(void);
	~GLCamera(void);

	//this method is used to apply the transofmations 
	void applyCameraTransformations();

	void translateTarget( const Vector3d& vec ) { target += vec; }
	void setTarget( const Point3d& target ) { this->target = target; }
	void modifyRotations( const Vector3d& delta ) { rotations += delta; }
	void translateCamDistance( double delta ) { camDistance += delta; }

	const Vector3d& getRotations() const { return rotations; }
	double getCamDistance() const { return camDistance; }
	const Point3d& getTarget() const { return target; }
	const TransformationMatrix& getWorldToCam() const { return worldToCam; }

	inline void setAutoOrbit(bool autoOrbit) { this->autoOrbit = autoOrbit; }
	inline bool doesAutoOrbit() const { return autoOrbit; }
};
