#include "Controller.h"

/**
	Default constructor.
*/
Controller::Controller(Character* ch) : Observable() {
	this->character = ch;
	jointCount = ch->getJointCount();
	for (int i=0;i<jointCount;i++)
		torques.push_back(Vector3d());
	for (int i=0;i<jointCount;i++){
		character->joints[i]->id = i;
	}
}

/**
	Default destructor.
*/
Controller::~Controller(void) {
}


/**
	This method is used to apply the torques that are computed to the character that is controlled.
*/
void Controller::applyTorques(){

	bool noOldTorqueInfo = false;
	if (oldTorques.size() != torques.size()){
		oldTorques.clear();
		noOldTorqueInfo = true;
	}

	Vector3d tmpTorque, deltaT;
	for (int i=0;i<jointCount;i++){
		if (noOldTorqueInfo)
			oldTorques.push_back(torques[i]);

		deltaT.setToDifference(torques[i], oldTorques[i]);

		double maxTorqueRateOfChange = 2000;

		deltaT.x = (deltaT.x<-maxTorqueRateOfChange)?(-maxTorqueRateOfChange):(deltaT.x);
		deltaT.x = (deltaT.x>maxTorqueRateOfChange)?(maxTorqueRateOfChange):(deltaT.x);
		deltaT.y = (deltaT.y<-maxTorqueRateOfChange)?(-maxTorqueRateOfChange):(deltaT.y);
		deltaT.y = (deltaT.y>maxTorqueRateOfChange)?(maxTorqueRateOfChange):(deltaT.y);
		deltaT.z = (deltaT.z<-maxTorqueRateOfChange)?(-maxTorqueRateOfChange):(deltaT.z);
		deltaT.z = (deltaT.z>maxTorqueRateOfChange)?(maxTorqueRateOfChange):(deltaT.z);


		tmpTorque.setToSum(oldTorques[i], deltaT);

		character->getJoint(i)->setTorque(tmpTorque);
		oldTorques[i].setValues(tmpTorque);
	}
}

/**
	This method is used to reset the torques that are to be applied.
*/
void Controller::resetTorques(){
	oldTorques.clear();
	for (int i=0;i<jointCount;i++)
		torques[i] = Vector3d(0,0,0);
}


