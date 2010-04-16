#pragma once

#include <Core/RLEnvState.h>
#include <Core/RLCharacterState.h>

class RLState{
	//for these states we need to record two pieces of information - the character state and the environment state.
	RLCharacterState* chState;

	//we will also store a copy of the environment/task variable that, together with the character state makes up the RL state.
	RLEnvState es;
public:
	RLState(){chState = NULL;}
	~RLState(){};

	//copy constructor
	RLState(const RLState& other){
		this->chState = other.chState;
		this->es = other.es;
	}

	//and a copy operator
	RLState& operator = (const RLState& other){
		this->chState = other.chState;
		this->es = other.es;
		return *this;
	}

	void set(RLCharacterState* chs, RLEnvState* e){
		this->chState = chs;
		es = *e;
	}
	
	RLCharacterState* getCharacterStateReference(){
		return chState;
	}

	RLEnvState* getEnvStateReference(){
		return &es;
	}

	bool isFailedState(){
		return chState->lostControl;
	}
};


