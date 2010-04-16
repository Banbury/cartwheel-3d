#include "GradientDescentOptimizer.h"
#include <time.h>
#include <stdio.h>


/**
	The constructor takes as parameter a function pointer that is used to evaluate the function that is to be optimized, 
	and also the data that needs to be passed in every time	the function is called.
*/
GradientDescentOptimizer::GradientDescentOptimizer(ObjFunction oFunc, void* d){
	alphaInitial = 1;
	nrBisections = 20;
	alphaDecay = 0.5;
	nrIterations = 15;
	minImprovement = 0.0001;
	h = 0.001;
	data = d;
	objFunc = oFunc;
}

GradientDescentOptimizer::~GradientDescentOptimizer(void){

}

//this method is used to compute the gradient of the function, evaluated at X. ObjValue holds the value of the
//objective function evaluated at X.
void GradientDescentOptimizer::computeGradient(DynamicArray<double>* X, DynamicArray<double>* gradient, double objValue){
	int nrVars = X->size();

	//compute the gradient, using a one or two sided finite difference
	for (int i=0;i<nrVars;i++){
		double initialValue = (*X)[i];
		(*X)[i] = initialValue + h;
		double eP = objFunc(X, data);
//#define USE_CENTRAL_DIFFERENCES
#ifdef USE_CENTRAL_DIFFERENCES
		(*X)[i] = initialValue - h;
		double eN = objFunc(X, data);
		//set this value back now
		(*gradient)[i] = ((eP - eN) / (2*h));
#else
		//we can use a one-sided finite difference scheme with the line below...
		(*gradient)[i] = ((eP - objValue) / (h));
#endif
		(*X)[i] = initialValue;
//		tprintf("gradient[%d] = %lf (ep = %lf, eN = %lf)\n", i, (*gradient)[i], eP, eN);
	}
}

//this method is used to form a new array of function parameters, in the form:  xN+1 = XN - alpha * G(F)
void GradientDescentOptimizer::createVector(DynamicArray<double> *target, DynamicArray<double> *X, DynamicArray<double> *G, double alpha){
	//the new guess is xN+1 = XN - alpha * G(F), where G(F) is the gradient of the objective function
	for (unsigned int i=0;i<X->size();i++)
		(*target)[i] = ((*X)[i] - alpha * (*G)[i]);
}


//this method is used to perform a line search, to improve the value of X, using the gradient information. The new
//objective value evaluated at the new X is stored in curObjValue. If the method returns 0, it means the objective
//function could not get improved, so X wasn't modified.
double GradientDescentOptimizer::lineSearch(DynamicArray<double>* X, DynamicArray<double>* gradient, double *curObjValue){
	//perform the line search on the parameter alpha
	double alpha = alphaInitial;

	for (int i=0;i<nrBisections;i++){
		//get the new guess...
		createVector(&testParams, X, gradient, alpha);
		//with this new guess, re-evaluate the objective function
		double newObjValue = objFunc(&testParams, data);
//		tprintf("trying alpha=%lf --> objFunc = %lf\n", alpha, newObjValue);
		//if we found a better value it means we are now on the right track...
		//the best value for alpha is somewhere between 0 and alpha
		if (newObjValue<*curObjValue){
			*curObjValue = newObjValue;
			double bestAlpha = alpha;
			//now we'll increase alpha a little, and discretize the remaining interval into equally spaced intervals
			alpha = 1/alphaDecay * alpha;
			double nrBins = nrBisections - i;
			for (int j=1;j<=nrBins;j++){
				createVector(&testParams, X, gradient, alpha * j * 1.0/nrBins);
				//with this new guess, re-evaluate the objective function
				double newObjValue = objFunc(&testParams, data);
//				tprintf("trying alpha=%lf --> objFunc = %lf\n",  alpha * j * 1.0/nrBins, newObjValue);
				if (newObjValue<*curObjValue){
					*curObjValue = newObjValue;
					bestAlpha = alpha * j * 1.0/nrBins;
				}
			}
			//done... report the value of alpha that we could find...
			return bestAlpha;
		}
		//otherwise we'll try again with a smaller value for alpha
		alpha = alphaDecay * alpha;
	}
	return 0;
}

/**
	This method starts the optimization process, starting from the given initial solution. The number of parameters passed in through X
	is the number of parameters n that the method optimizes over. This method returns the time, measured in seconds, that this method took
	to optimize. The values in X contain the initial soultion to the optimization, and at the end of the call, they will contain the result.
*/
int GradientDescentOptimizer::optimize(DynamicArray<double>* X){
	//this is the vector of partial derivatives of the obj function - the gradient
	DynamicArray<double> gradient;
	//this is the total number of variables that we need to optimize over...
	int nrVars = X->size();
	bool done = false;

	tprintf("Starting optimization process\n");


	//make sure that the testPrameters, gradient and result arrays have enough space allocated...
	testParams.clear();

	for (int i=0;i<nrVars;i++){
		testParams.push_back(0);
		gradient.push_back(0);
	}

	//record the starting time, so we can keep track of how long the process took...
	int startTime = (int)time(0);

	//compute the initial value of the objective function
	double curObjValue = objFunc(X, data);

	tprintf("Initial objective value: %lf\n", curObjValue);

	int iterNr = 0;

	while (!done){
		iterNr++;
		double previousBestObjValue = curObjValue;
		done = true;

		tprintf("Computing gradient...\n");
		//compute the gradient
		computeGradient(X, &gradient, curObjValue);
		tprintf("Starting line search...\n");
		//perform the line search
		double alpha = lineSearch(X, &gradient, &curObjValue);
		//if we can improve any further, then do it...
		if (alpha>0){
			tprintf("Better solution found at alpha=%lf! New obj value: %lf\n", alpha, curObjValue);

			//it is worth trying again...
			done = false;
			createVector(&testParams, X, &gradient, alpha);
			for (int j=0;j<nrVars;j++)
				(*X)[j] = testParams[j];

			for (uint j=0;j<X->size();j++)
				tprintf("%lf\n", (*X)[j]);
			tprintf("--------------------------------\n");

		}

		//make sure we stop after a set number of iterations, or if the obj value didn't improve much
		if (iterNr>=nrIterations || (previousBestObjValue - curObjValue)< minImprovement)
			done = true;
	}

	int timeEllapsed = (int)time(0) - startTime;

	tprintf("Done! Optimization took %ds\n", timeEllapsed);

	//and we're done... return the time this method took
	return timeEllapsed;
}



