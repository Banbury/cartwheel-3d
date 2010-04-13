#pragma once

#include <MathLib/MathLibDll.h>

#include <MathLib/Vector3d.h>
#include <MathLib/Point3d.h>


class MATHLIB_DECLSPEC Plane{
public:
	//a plane is defined by its normal, and a point on it
	Vector3d n;
	Point3d p;
public:
	Plane(void);
	Plane(const Point3d& p, const Vector3d& n);
	~Plane(void);
};
