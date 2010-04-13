#pragma once
#include "GLCamera.h"

/**
 * This class implements a simple open GL camera. This camera can be used to automatically follow a target, and rotate around it.
 */
class GLTrackingCamera : public GLCamera{
private:
	//keep track of the orientation of the camera.
	Quaternion orientation;
public:
	GLTrackingCamera(void);
	virtual ~GLTrackingCamera(void);

	//this method is used to apply the transofmations 
	virtual void applyCameraTransformations();
	//this method is used to change the camera parameters based on parameters obtained from the change in mouse position
	virtual void applyMouseInput(const Vector3d& input, int mode);

	//to simplify things, we will keep track of the rotation using this vector. This way, we won't need to mix rotations about the x, y and z axis
	Vector3d rotations;
	//and also how far it is along its z axis (assuming that it is looking down the -ve z axis of its local frame)
	double camDistance;
	//we will also keep track of the point we are looking at, in world coordinates
	Point3d target;
	//this is the world to camera transformation matrix, recomputed at every step
	TransformationMatrix worldToCam;
};
