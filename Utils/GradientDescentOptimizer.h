#pragma once

#include <Utils/UtilsDll.h>

#include <Utils/Utils.h>


/**
	This generic class can be used to apply gradient descent optimization to minimize a function f from R^n into R. It is assumed that
	f is differentiable. The gradient information is computed using finite differences. The optimization routine needs to have access
	to a method that computes the function f, applied at a point x that belongs to R^n.
*/

//this defines the function structure for methods that are supposed to be used as objective functions
typedef double (*ObjFunction)(DynamicArray<double>*, void*);

class UTILS_DECLSPEC GradientDescentOptimizer{
public:
	//this is the maximum number of iterations that we will be performing
	int nrIterations;
	//this is the starting alpha value that is used for the line search
	double alphaInitial;
	//the number of segments that we divide alpha in during the line search
	double nrBisections;
	//this is the ammount by which alpha decays with every iteration in the line search
	double alphaDecay;
	//if the new function value didn't improve by more than this relative to the previous objective value, we will not continue on with another iterations
	double minImprovement;
	//this is the step size that is used when computing the gradient
	double h;

private:
	ObjFunction objFunc;
	void* data;
	//this will be a vector of values used for testing, etc...
	DynamicArray<double> testParams;


	//this method is used to compute the gradient of the function, evaluated at X. ObjValue holds the value of the
	//objective function evaluated at X.
	void computeGradient(DynamicArray<double>* X, DynamicArray<double>* gradient, double objValue);

	//this method is used to perform a line search, to improve the value of X, using the gradient information. 
	//The method returns the best value of alpha that could be found, that improves the objective function (the
	//new value of which is stored in curObjValue), or 0 if the function could not be improved. The values stored
	//in x do not get modified
	double lineSearch(DynamicArray<double>* X, DynamicArray<double>* gradient, double *curObjValue);

	//this method is used to form a new array of function parameters, in the form:  xN+1 = XN - alpha * G(F)
	void createVector(DynamicArray<double> *target, DynamicArray<double> *X, DynamicArray<double> *G, double alpha);

public:
	/**
		The constructor takes as parameter a function pointer that is used to evaluate the function that is to be optimized, 
		and also the data that needs to be passed in every time	the function is called.
	*/
	GradientDescentOptimizer(ObjFunction oFunc, void* d);

	/**
		This method starts the optimization process, starting from the given initial solution. The number of parameters passed in here
		is the number of parameters n that the method optimizes over. This method returns the time, measured in seconds, that this method took
		to optimize. The values in X contain the initial soultion to the optimization, and at the end of the call, they will contain the result.
	*/
	int optimize(DynamicArray<double>* X);

	~GradientDescentOptimizer(void);
};
