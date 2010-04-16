#include <cstdlib>

#include "World.h"
#include <Physics/RBUtils.h>
#include <Utils/Utils.h>

#include <Physics/ODEWorld.h>   // Singleton will be an instance of ODEWorld

// Singleton stuff
World* World::_instance = NULL;
void World::create() {
	assert( _instance == NULL );
	_instance = new ODEWorld();
	std::atexit( World::destroy );
}

void World::destroy() {
	assert( _instance != NULL );
	delete _instance;
}



World::World(void){
	this->objects = DynamicArray<RigidBody*>(300);
	this->objects.clear();
}

World::~World(void){
	destroyWorld();
}

void World::destroyWorld() {

	//delete all the rigid bodies in this world
	for (uint i=0;i<objects.size();i++) {
		if( objects[i]->isArticulated() ) {
			if ( ((ArticulatedRigidBody*)objects[i])->getAFParent() != NULL )
				continue; // Articulated figure is responsible of deleting this object
		}
		delete objects[i];
	}
	objects.clear();

	ABs.clear();

	//delete the references to the articulated figures that we hold as well
	for (uint i=0;i<AFs.size();i++)
		delete AFs[i];
	AFs.clear();

	jts.clear();

	contactPoints.clear();
}

void World::destroyAllObjects() {
	destroyWorld();
}

/**
	This method is used to draw all the rigid bodies in the world
*/
void World::drawRBs(int flags){
	for (uint i=0;i<objects.size();i++)
		objects[i]->draw(flags);
}

/**
	This method renders all the rigid bodies as a set of vertices 
	and faces that will be appended to the passed OBJ file.

	vertexIdxOffset indicates the index of the first vertex for this object, this makes it possible to render
	multiple different meshes to the same OBJ file
	
	Returns the number of vertices written to the file
*/
uint World::renderRBsToObjFile(FILE* fp, uint vertexIdxOffset){
	
	uint nbVerts = 0;
	for (uint i=0;i<objects.size();i++)
		nbVerts += objects[i]->renderToObjFile(fp, vertexIdxOffset + nbVerts);

	return nbVerts;
}


/**
	This method returns the reference to the first articulated rigid body with 
	its name and its articulared figure name, or NULL if it is not found
*/
ArticulatedRigidBody* World::getARBByName(char* name, char* articulatedFigureName){
	if (name == NULL)
		return NULL;
	for (uint i=0;i<ABs.size();i++)
		if (strcmp(name, ABs[i]->name) == 0)
			if( articulatedFigureName == NULL ||
				strcmp( articulatedFigureName, ABs[i]->getAFParent()->getName() ) == 0 )
			return ABs[i];
	return NULL;
}

/**
	This method returns the reference to the first articulated rigid body with 
	its name and its articulared figure name, or NULL if it is not found
*/
ArticulatedRigidBody* World::getARBByName(char* name, const ArticulatedFigure* articulatedFigure){
	if (name == NULL)
		return NULL;
	for (uint i=0;i<ABs.size();i++)
		if (strcmp(name, ABs[i]->name) == 0 && articulatedFigure == ABs[i]->getAFParent() )
			return ABs[i];
	return NULL;
}

/**
	This method returns the reference to the rigid body with the given name, or NULL if it is not found
*/
RigidBody* World::getRBByName(char* name){
	if (name == NULL)
		return NULL;
	for (uint i=0;i<this->objects.size();i++)
		if (strcmp(name, objects[i]->name) == 0)
			return objects[i];
	return NULL;
}

/**
	This method reads a list of rigid bodies from the specified file.
*/
void World::loadRBsFromFile(char* fName){
	if (fName == NULL)
		throwError("NULL file name provided.");
	FILE *f = fopen(fName, "r");
	if (f == NULL)
		throwError("Could not open file: %s", fName);

	//have a temporary buffer used to read the file line by line...
	char buffer[200];
	RigidBody* newBody = NULL;
	ArticulatedFigure* newFigure = NULL;
	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getRBLineType(line);
		switch (lineType) {
			case RB_RB:
				//create a new rigid body and have it load its own info...
				newBody = new RigidBody();
				newBody->loadFromFile(f);
				objects.push_back(newBody);
				break;
			case RB_ARB:
				//create a new articulated rigid body and have it load its own info...
				newBody = new ArticulatedRigidBody();
				newBody->loadFromFile(f);
				objects.push_back(newBody);
				//remember it as an articulated rigid body to be able to link it with other ABs later on
				ABs.push_back((ArticulatedRigidBody*)newBody);
				break;
			case RB_ARTICULATED_FIGURE:
				//we have an articulated figure to worry about...
                newFigure = new ArticulatedFigure();
				AFs.push_back(newFigure);
				newFigure->loadFromFile(f, this);
				newFigure->addJointsToList(&jts);
				break;
			case RB_NOT_IMPORTANT:
				if (strlen(line)!=0 && line[0] != '#')
					tprintf("Ignoring input line: \'%s\'\n", line);
				break;
			default:
				throwError("Incorrect rigid body input file: \'%s\' - unexpected line.", buffer);
		}
	}

	//now we'll make sure that the joint constraints are satisfied
	for (uint i=0;i<AFs.size();i++)
		AFs[i]->fixJointConstraints();

	//and now make sure that each rigid body's toWorld transformation is updated
//	for (uint i=0;i<objects.size();i++){
//		objects[i]->updateToWorldTransformation();
//	}
}

/**
	This method adds one rigid body (not articulated).
*/
void World::addRigidBody(RigidBody* rigidBody){
	objects.push_back(rigidBody);
	if( rigidBody->isArticulated() )
		ABs.push_back((ArticulatedRigidBody*)rigidBody);
}

/**
	This method adds one rigid body (not articulated).
*/
void World::addArticulatedFigure(ArticulatedFigure* articulatedFigure){
	articulatedFigure->loadIntoWorld();
	AFs.push_back(articulatedFigure);
	articulatedFigure->addJointsToList(&jts);
	articulatedFigure->fixJointConstraints();
}

/**
	This method is used to get the state of all the rigid body in this collection.
*/
void World::getState(DynamicArray<double>* state){
	for (uint i=0;i<this->objects.size();i++){
		state->push_back(objects[i]->state.position.x);
		state->push_back(objects[i]->state.position.y);
		state->push_back(objects[i]->state.position.z);

		state->push_back(objects[i]->state.orientation.s);
		state->push_back(objects[i]->state.orientation.v.x);
		state->push_back(objects[i]->state.orientation.v.y);
		state->push_back(objects[i]->state.orientation.v.z);

		state->push_back(objects[i]->state.velocity.x);
		state->push_back(objects[i]->state.velocity.y);
		state->push_back(objects[i]->state.velocity.z);

		state->push_back(objects[i]->state.angularVelocity.x);
		state->push_back(objects[i]->state.angularVelocity.y);
		state->push_back(objects[i]->state.angularVelocity.z);
	}
}

/**
	This method is used to set the state of all the rigid body in this collection.
*/
void World::setState(DynamicArray<double>* state, int start){
	int i = start;
	for (uint j=0;j<this->objects.size();j++){
		objects[j]->state.position = Point3d((*state)[i+0], (*state)[i+1], (*state)[i+2]);
		i+=3;
		objects[j]->state.orientation = Quaternion((*state)[i+0], (*state)[i+1], (*state)[i+2], (*state)[i+3]);
		i+=4;
		objects[j]->state.velocity = Vector3d((*state)[i+0], (*state)[i+1], (*state)[i+2]);
		i+=3;
		objects[j]->state.angularVelocity = Vector3d((*state)[i+0], (*state)[i+1], (*state)[i+2]);
		i+=3;
//		objects[j]->updateToWorldTransformation();
	}
}

