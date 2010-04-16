
#include "GLCamera.h"
#include <include/GLheaders.h>

#define ORBIT_RAD_PER_SEC 1.0


GLCamera::GLCamera(void){
	//initialize to the identity quaternion
	orientation = Quaternion(1, 0, 0, 0);
	camDistance = -4;
	target = Point3d(0,0,0);
	rotations = Vector3d(0,0,0);
	autoOrbit = false;
}

GLCamera::~GLCamera(void){
}

//this method is used to apply the transofmations 
void GLCamera::applyCameraTransformations(){

	if(autoOrbit) {
		rotations.y += 0.025;
	}

	//build the orientation. We will construct it by rotating about the z axis, y axis first, and the x
	orientation = Quaternion::getRotationQuaternion(rotations.z, Vector3d(0, 0, 1))
				* Quaternion::getRotationQuaternion(rotations.y, Vector3d(0, 1, 0))
				* Quaternion::getRotationQuaternion(rotations.x, Vector3d(1, 0, 0));
	//we know we are looking down the z-axis of the camera coordinate frame. So we need to compute what that axis is in world coordinates.
	Vector3d zW = orientation.rotate(Vector3d(0,0,-1));
	//this is the camera's up vector, expressed in world coordinates 
	Vector3d yW = orientation.rotate(Vector3d(0,1,0));
	//we know where we are looking, so we can compute the position of the camera in world coordinates.
	Point3d camPos = target + zW * camDistance;
	//now we have all the information we need: the camera coordinate frame expressed in world coordinates. Need only invert it...
	gluLookAt(camPos.x, camPos.y, camPos.z, target.x, target.y, target.z, yW.x, yW.y, yW.z);
	double vals[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, vals);
	worldToCam.setOGLValues(vals);
}

