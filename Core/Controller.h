#pragma once

#include <Utils/Utils.h>
#include "Character.h"
#include <Physics/World.h>
#include <Utils/Observable.h>

/**
	This class is used to provide a generic interface to a controller. A controller acts on a character - it computes torques that are
	applied to the joints of the character. The details of how the torques are computed are left up to the classes that extend this one.
*/
class Controller : public Observable {
	friend class DoubleStanceFeedback;
	friend class IKVMCController;
	friend class TestApp;
	friend class TestApp2;
protected:
	//this is the character that the controller is acting on
	Character* character;
	//and this is the array of torques that will be computed in order for the character to match the desired pose - stored in world coordinates
	DynamicArray<Vector3d> torques;
	//these are the last torques that were applied - used to place limits on how fast the torques are allowed to change
	DynamicArray<Vector3d> oldTorques;
	//this is the number of joints of the character - stored here for easy access
	int jointCount;
	//This is a name for the controller
	char name[100];
public:
	Controller(Character* ch);
	virtual ~Controller(void);

	void setName( const char* name ) {
		strncpy(this->name,name,99);
		notifyObservers();
	}

	const char* getName() {
		return name;
	}

	Character* getCharacter() const {
		return character;
	}

	virtual void performPreTasks(double dt, DynamicArray<ContactPoint> *cfs) {
		computeTorques(cfs);
		applyTorques();
	}

	// Returns true if it transitioned to a new state, false otherwise
	virtual bool performPostTasks(double dt, DynamicArray<ContactPoint> *cfs) { return false; }

	/**
		This method is used to compute the torques, based on the current and desired poses
	*/
	virtual void computeTorques(DynamicArray<ContactPoint> *cfs) = 0;

	/**
		This method is used to apply the torques that are computed to the character that is controlled.
	*/
	void applyTorques();

	/**
		This method is used to reset the torques that are to be applied.
	*/
	void resetTorques();


};
