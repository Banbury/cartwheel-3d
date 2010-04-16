#pragma once

#include <Utils/Utils.h>

/**
	A perturbation/alteration class contains information regarding the variable(s) that it is used to alter. If multiple variables
	are altered by the same perturbation, then they are assumed to have the same initial value.
 */
class Perturbation {
public:
	//this is a list of pointers to the values that need to be perturbed
	DynamicArray<double*> variables;
	//this is a copy of the original value of the variables above
	double originalValue;
	//and this is the name associated with the variables (mostly for debugging purposes)
	char name[100];
	//with each perturbation we will associate a weight - potentially useful for optimizations...
	double weight;

public:
	//constructor...
	Perturbation(double* var, char *PName, double w);

	//a copy constructor
	Perturbation(const Perturbation& other);

	//and a copy operator
	Perturbation& operator = (const Perturbation& other);

	//adds a variable to be affected by the same perturbation
	void addVariable(double* newVar);

	//this method sets the value passed in to all the variables that need to be perturbed
	void setValue(double newValue);

	//this method returns the value of the variables that are being altered
	double getValue();

	//and this method returns the default value
	double getDefaultValue();

	//this method restores the default value of all the variables
	void resetVariables();

	//returns the name
	char* getName();
};

/**
	A controller is defined by a sequence of parameters. This class provides an easy way of perturbing (i.e. altering) the value of some of these
	parameters. This can be useful when testing different settings for a controller (for instance when computing gradients).
*/
class ControllerPerturbator{
public:
	DynamicArray<Perturbation*> perturbations;
public:
	//resets the perturbations
	void resetPerturbations();

	//applys a given perturbation to one of the variables being altered
	void perturb(int which, double value);

	//adds a new perturbation to the collection...
	void addPerturbation(DynamicArray<double*> *vars, char* PName, double w);

	//adds a new perturbation to the collection...
	void addPerturbation(double *var, char* PName, double w);

	//default constructor
	ControllerPerturbator();

	//destructor
	~ControllerPerturbator(void);

	//returns the list of perturbation values...
	void getDeltaP(DynamicArray<double> *result);

	//sets the perturbations...
	void setDeltaP(DynamicArray<double> *deltaP);
};
