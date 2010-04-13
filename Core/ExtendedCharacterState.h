#pragma once

#include <Utils/Utils.h>
#include <MathLib/Vector3d.h>
#include <MathLib/Quaternion.h>
#include <Core/Character.h>


/**
	This class is used as a container for the full state of a character. It is used to be able to do things like the Value iteration algorithm, 
	where we need to continually restart simulations from specific character states. It holds the reduced-coordinates version of the state (that
	will introduce some error because of numerical issues), the stance of the character when this state was recorded/stored, and also d and v as 
	vectors expressed in the relative coordinate frame of the character. Even tough d and v should be fully specified with the state and stance 
	information, we will store it anyway for easy access. We will also hold the quaternion that holds the transformation from the relative world 
	frame to the world frame (aka heading - this is just the twist around the world-axis vertical axis). This orientation can be used to go from 
	quantities represented in world coordinates to quantities represented in the relative world frame of the character.
*/
class ExtendedCharacterState{
public:
	DynamicArray<double> state;
	int stance;
	Vector3d d;
	Vector3d v;
	Quaternion heading;
	bool lostControl;
	//also keep track of the step, as represented by the vector between the right foot and the left foot (or the negative of that depending on the situation)
	Vector3d step;

public:
	/**
		Constructor - sets all the data members to values passed in as parameters
	*/
	ExtendedCharacterState(DynamicArray<double> state, int stance, Vector3d d, Vector3d v, Quaternion heading, Vector3d step){
		this->state = state;
		this->stance = stance;
		this->d = d;
		this->v = v;
		this->heading = heading;
		lostControl = false;
		this->step = step;
	}

	/**
		Constructor - sets all the data members to values passed in as parameters
	*/
	ExtendedCharacterState(DynamicArray<double> state){
		this->state = state;
		this->stance = 0;
		this->d = Vector3d();
		this->v = Vector3d();
		this->heading = Quaternion();
		lostControl = false;
		this->step = Vector3d();
	}

	/**
		make a copy of all the data members of the state passed in as a parameter
	*/
	ExtendedCharacterState(ExtendedCharacterState* other){
		copyState(other);
	}

	inline void copyState(ExtendedCharacterState* other){
		this->state = other->state;
		this->stance = other->stance;
		this->d = other->d;
		this->v = other->v;
		this->heading = other->heading;
		this->lostControl = other->lostControl;
		this->step = other->step;
	}

	/**
		Default constructor - someone should set the data member values
	*/
	ExtendedCharacterState(){
		lostControl = false;
		this->stance = 0;
		this->d = Vector3d();
		this->v = Vector3d();
		this->heading = Quaternion();
		lostControl = false;
		this->step = Vector3d();
	}

	/**
		this contstructor initializes all the data members from a file
	*/
	ExtendedCharacterState(FILE* fp){
		readFromFile(fp);
	}

	/**
		write the contents of the state to a file
	*/
	void writeToFile(FILE* fp){
		fprintf(fp, "%d\n", state.size());
		for (uint i=0;i<state.size();i++)
			fprintf(fp, "%lf ", state[i]);
		fprintf(fp,"\n%d %lf %lf %lf %lf %lf %lf\n", stance, d.x, d.y, d.z, v.x, v.y, v.z);
		fprintf(fp, "%lf %lf %lf %lf\n", heading.s, heading.v.x, heading.v.y, heading.v.z);
		fprintf(fp,"%lf %lf %lf\n", step.x, step.y, step.z);
		if (lostControl)
			fprintf(fp, "0");
		else
			fprintf(fp, "1");
		fprintf(fp,"\n");
	}

	/**
		read the contents of the state from a file
	*/
	void readFromFile(FILE *fp){
		int temp;
		double temp2;
		state.clear();
		fscanf(fp, "%d", &temp);
		for (int i=0;i<temp;i++){
			fscanf(fp, "%lf ", &temp2);
			state.push_back(temp2);
		}

		fscanf(fp,"%d %lf %lf %lf %lf %lf %lf", &stance, &d.x, &d.y, &d.z, &v.x, &v.y, &v.z);
		fscanf(fp, "%lf %lf %lf %lf", &heading.s, &heading.v.x, &heading.v.y, &heading.v.z);
		fscanf(fp,"%lf %lf %lf", &step.x, &step.y, &step.z);
		fscanf(fp, "%d", &temp);

		ReducedCharacterState rs(&this->state);
		heading = computeHeading(rs.getOrientation());

		lostControl = (temp == 0);
	}

	virtual ~ExtendedCharacterState(void){
	}

	/**
		a bunch of getters
	*/
	inline Vector3d getD() const {return d;}
	inline Vector3d getV() const {return v;}
	inline int getStance() const {return stance;}
	inline DynamicArray<double>* getStateReference() {return &state;}
	inline Quaternion getHeading() const {return heading;}
	inline bool hasLostControl() const {return lostControl;}
	inline Vector3d getStep() const {return step;}
};
