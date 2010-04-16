/*
	Simbicon 1.5 Controller Editor Framework, 
	Copyright 2009 Stelian Coros, Philippe Beaudoin and Michiel van de Panne.
	All rights reserved. Web: www.cs.ubc.ca/~van/simbicon_cef

	This file is part of the Simbicon 1.5 Controller Editor Framework.

	Simbicon 1.5 Controller Editor Framework is free software: you can 
	redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Simbicon 1.5 Controller Editor Framework is distributed in the hope 
	that it will be useful, but WITHOUT ANY WARRANTY; without even the 
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Simbicon 1.5 Controller Editor Framework. 
	If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <Utils/Observable.h>
#include <MathLib/Vector3d.h>


class SimBiController;
class Joint;

/**
	This generic class provides an interface for classes that provide balance feedback for controllers for physically simulated characters.
*/

class BalanceFeedback : public Observable {
public:
	BalanceFeedback(void);
	virtual ~BalanceFeedback(void);
	/**
		This method returns a scalar that is the ammount of feedback that needs to be added to a trajectory. It is a function of the
		phase in the controller's state (between 0 and 1), the vector between the stance foot and the center of mass, and the velocity of
		the center of mass.
	*/
	virtual double getFeedbackContribution(SimBiController* con, Joint* j, double phi, Vector3d d, Vector3d v) = 0;
	virtual void writeToFile(FILE* fp) = 0;
	virtual void loadFromFile(FILE* fp) = 0;

};

/**
	This class applies feedback that is linear in d and v - i.e. the original simbicon feedback formulation
*/
class LinearBalanceFeedback : public BalanceFeedback{
public:
	//This vector, dotted with d or v, gives the quantities that should be used in the feedback formula
	Vector3d feedbackProjectionAxis;
	//these are the two feedback gains
	double cd;
	double cv;

	double vMin, vMax, dMin, dMax;
public:
	LinearBalanceFeedback(){
		feedbackProjectionAxis = Vector3d();
		cd = cv = 0;
		vMin = dMin = -1000;
		vMax = dMax = 1000;
	}

	virtual ~LinearBalanceFeedback(){

	}

	void setProjectionAxis( const Vector3d& axis ) {
		feedbackProjectionAxis = axis;
	}
	
	const Vector3d& getProjectionAxis() const {
		return feedbackProjectionAxis;
	}

	void setCd( double cd ) {
		this->cd = cd;
	}

	double getCd() const { return cd; }

	void setCv( double cv ) {
		this->cv = cv;
	}

	double getCv() const { return cv; }

	void setDLimits( double dMin, double dMax ) {
		this->dMin = dMin;
		this->dMax = dMax;
	}

	double getDMin() const { return dMin; }
	double getDMax() const { return dMax; }

	void setVLimits( double vMin, double vMax ) {
		this->vMin = vMin;
		this->vMax = vMax;
	}

	double getVMin() const { return vMin; }
	double getVMax() const { return vMax; }



	/**
		This method returns a scalar that is the ammount of feedback that needs to be added to a trajectory. It is a function of the
		phase in the controller's state (between 0 and 1), the vector between the stance foot and the center of mass, and the velocity of
		the center of mass.
	*/
	virtual double getFeedbackContribution(SimBiController* con, Joint* j, double phi, Vector3d d, Vector3d v){
		double dToUse = d.dotProductWith(feedbackProjectionAxis);
		double vToUse = v.dotProductWith(feedbackProjectionAxis);
		if (dToUse < dMin) dToUse = dMin;
		if (vToUse < vMin) vToUse = vMin;
		if (dToUse > dMax) dToUse = dMax;
		if (vToUse > vMax) vToUse = vMax;

		return dToUse * cd + vToUse * cv;
	}

	virtual void writeToFile(FILE* fp);
	virtual void loadFromFile(FILE* fp);


};


class DoubleStanceFeedback : public BalanceFeedback{
public:
	//This vector, dotted with d or v, gives the quantities that should be used in the feedback formula
	Vector3d feedbackProjectionAxis;

	//this is the max value of the feedback
	double maxFeedbackValue;
	//and this is the lowest value of the feedback
	double minFeedbackValue;

	double cd;
	double cv;
	double totalMultiplier;
	double vMin, vMax, dMin, dMax;

	DoubleStanceFeedback();

	virtual ~DoubleStanceFeedback(){

	}

	/**
		This method returns a scalar that is the ammount of feedback that needs to be added to a trajectory. It is a function of the
		phase in the controller's state (between 0 and 1), the vector between the stance foot and the center of mass, and the velocity of
		the center of mass.
	*/
	virtual double getFeedbackContribution(SimBiController* con, Joint* j, double phi, Vector3d d, Vector3d v);
	virtual void writeToFile(FILE* fp);
	virtual void loadFromFile(FILE* fp);

};


