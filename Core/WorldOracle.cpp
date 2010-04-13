#include "WorldOracle.h"

WorldOracle::WorldOracle(void){
}

WorldOracle::~WorldOracle(void){
}


double WorldOracle::getWorldHeightAt(Point3d worldLoc){
	double height = 0;
	worldLoc.y = 0;
	for (uint i=0;i<spheres.size();i++){
		//see if the given point falls within it...
		Vector3d v(spheres[i].pos, worldLoc);
		double r = spheres[i].radius;
		double dist = v.length();
		if (dist < r){
			double tmpHeight = sqrt(r*r - v.x*v.x - v.z*v.z) + spheres[i].pos.y;
			if (height < tmpHeight)
				height = tmpHeight;
		}
	}

	return height;
}

void createSpheresRBFile(DynamicArray<Sphere> spheres, char* fName){
	FILE* fp = fopen(fName, "w");
	//we'll create an RB with the correct CDP, mesh and position
	for (uint i=0;i<spheres.size();i++){
		fprintf(fp, "RigidBody\n");
		fprintf(fp, "\tstatic\n");
		fprintf(fp, "\tCDP_Sphere %2.3lf %2.3lf %2.3lf %2.3lf\n", spheres[i].pos.x,spheres[i].pos.y,spheres[i].pos.z, spheres[i].radius);
		fprintf(fp, "/End\n\n\n");
	}
	fclose(fp);
}


void WorldOracle::initializeWorld(World *physicalWorld){
//	spheres.push_back(Sphere(Point3d(0,-0.2,0), 0.3));
//	spheres.push_back(Sphere(Point3d(5, -1.0, 6), 1.1));

	if (spheres.size() > 0){
		createSpheresRBFile(spheres, "../data/objects/tmpSpheres.rbs");
		physicalWorld->loadRBsFromFile("../data/objects/tmpSpheres.rbs");
	}
}

void WorldOracle::draw(){
	for (uint i=0;i<spheres.size();i++)
		GLUtils::drawSphere(spheres[i].pos, spheres[i].radius, 9);
}

