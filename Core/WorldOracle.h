#pragma once

#include <MathLib/Sphere.h>
#include <Physics/World.h>

class WorldOracle{
private:
	DynamicArray<Sphere> spheres;

public:
	WorldOracle(void);
	virtual ~WorldOracle(void);

	virtual double getWorldHeightAt(Point3d worldLoc);
	virtual void initializeWorld(World *physicalWorld);

	virtual void draw();
};



