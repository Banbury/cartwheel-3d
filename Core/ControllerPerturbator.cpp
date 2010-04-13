#include "ControllerPerturbator.h"
#include <string.h>
#include <Utils/Utils.h>


//constructor...
Perturbation::Perturbation(double* var, char *PName, double w){
	if (var == NULL)
		throwError("Exception - Don't pass in a NULL value...\n");
	variables.push_back(var);
	originalValue = *var;
	strcpy(name, PName);
	this->weight = w;
}

//a copy constructor
Perturbation::Perturbation(const Perturbation& other){
	this->variables = other.variables;
	this->originalValue = other.originalValue;
	strcpy(this->name, other.name);
	this->weight = other.weight;
}

//and a copy operator
Perturbation& Perturbation::operator = (const Perturbation& other){
	this->variables = other.variables;
	this->originalValue = other.originalValue;
	strcpy(this->name, other.name);
	return *this;
}

void Perturbation::addVariable(double* newVar){
	if (newVar == NULL)
		throwError("Exception - Don't pass in a NULL value...\n");
	if (*newVar!=originalValue)
		throwError("Exception - The variable passed in doesn't match the existing variables.");
	variables.push_back(newVar);
}

//this method sets the value passed in to all the variables that need to be perturbed
void Perturbation::setValue(double newValue){
	for (uint i=0;i<variables.size();i++)
		*(variables[i]) = newValue;
}

//this method returns the value of the variables that are being altered
double Perturbation::getValue(){
	return *(variables[0]);
}

//and this method returns the default value
double Perturbation::getDefaultValue(){
	return originalValue;
}

//this method restores the default value of all the variables
void Perturbation::resetVariables(){
	setValue(this->originalValue);
}

char* Perturbation::getName(){
	return name;
}

//constructor..
ControllerPerturbator::ControllerPerturbator(void){
}

//destructor..
ControllerPerturbator::~ControllerPerturbator(void){
	for (uint i=0;i<perturbations.size();i++)
		delete perturbations[i];
}

//resets the perturbations
void ControllerPerturbator::resetPerturbations(){
	for (uint i=0;i<perturbations.size();i++)
		perturbations[i]->resetVariables();
}

//applys a given perturbation to one of the variables being altered
void ControllerPerturbator::perturb(int which, double value){
	if (which>=0 && (uint)which<perturbations.size())
		perturbations[which]->setValue(value);
}

//adds a new perturbation to the collection...
void ControllerPerturbator::addPerturbation(DynamicArray<double*> *vars, char* PName, double w){
	if (vars->size()<=0)
		return;
	Perturbation* p = new Perturbation((*vars)[0], PName, w);
	for (uint i=1;i<vars->size();i++)
		p->addVariable((*vars)[i]);

	perturbations.push_back(p);
}

//adds a new perturbation to the collection...
void ControllerPerturbator::addPerturbation(double *var, char* PName, double w){
	Perturbation* p = new Perturbation(var, PName, w);

	perturbations.push_back(p);
}


//returns the list of perturbation values...
void ControllerPerturbator::getDeltaP(DynamicArray<double> *result){
	for (uint i=0;i<perturbations.size();i++)
		result->push_back(perturbations[i]->getValue() - perturbations[i]->getDefaultValue());
}

//sets the perturbations...
void ControllerPerturbator::setDeltaP(DynamicArray<double> *deltaP){
	for (uint i=0;i<deltaP->size();i++)
		perturbations[i]->setValue(perturbations[i]->getDefaultValue() + (*deltaP)[i]);
}

