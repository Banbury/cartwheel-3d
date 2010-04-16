#pragma once

#include <MathLib/MathLibDll.h>

#include <MathLib/Point3d.h>


class MATHLIB_DECLSPEC Sphere{
public:
	//a sphere is defined by its location in space (a point), and its radius
	Point3d pos;
	double radius;
public:
	Sphere(void);
	Sphere(const Point3d& p, double r);
	~Sphere(void);
};


