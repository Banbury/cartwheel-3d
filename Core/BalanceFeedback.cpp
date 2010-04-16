#include "BalanceFeedback.h"
#include <Core/ConUtils.h>
#include <Physics/Joint.h>
#include <Core/SimBiController.h>


BalanceFeedback::BalanceFeedback(void){
}

BalanceFeedback::~BalanceFeedback(void){
}


void LinearBalanceFeedback::loadFromFile(FILE* f){
	if (f == NULL)
		throwError("File pointer is NULL - cannot read gain coefficients!!");

	//have a temporary buffer used to read the file line by line...
	char buffer[200];

	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		switch (lineType) {
			case CON_FEEDBACK_END:
				//we're done...
				return;
				break;
			case CON_COMMENT:
				break;
			case CON_CV:
				if (sscanf(line, "%lf", &this->cv)!=1)
					throwError("A cv value must be specified!");
				break;
			case CON_CD:
				if (sscanf(line, "%lf", &this->cd)!=1)
					throwError("A cd value must be specified!");
				break;
			case CON_D_MIN:
				if (sscanf(line, "%lf", &this->dMin)!=1)
					throwError("A dMin value must be specified!");
				break;
			case CON_D_MAX:
				if (sscanf(line, "%lf", &this->dMax)!=1)
					throwError("A dMax value must be specified!");
				break;
			case CON_V_MIN:
				if (sscanf(line, "%lf", &this->vMin)!=1)
					throwError("A vMin value must be specified!");
				break;
			case CON_V_MAX:
				if (sscanf(line, "%lf", &this->vMax)!=1)
					throwError("A vMax value must be specified!");
				break;
			case CON_FEEDBACK_PROJECTION_AXIS:
				if (sscanf(line, "%lf %lf %lf", &this->feedbackProjectionAxis.x, &this->feedbackProjectionAxis.y, &this->feedbackProjectionAxis.z)!=3)
					throwError("The axis for a trajectory is specified by three parameters!");
				this->feedbackProjectionAxis.toUnit();
				break;
			case CON_NOT_IMPORTANT:
				tprintf("Ignoring input line: \'%s\'\n", line);
				break;
			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
	throwError("Incorrect SIMBICON input file: No \'/jointTrajectory\' found ", buffer);
}


/**
	This method is used to write the state parameters to a file
*/
void LinearBalanceFeedback::writeToFile(FILE* f){
	if (f == NULL)
		return;

	fprintf( f, "\t\t\t%s linear\n", getConLineString(CON_FEEDBACK_START) );

	fprintf( f, "\t\t\t\t%s %lf %lf %lf\n", getConLineString(CON_FEEDBACK_PROJECTION_AXIS),
		feedbackProjectionAxis.x,
		feedbackProjectionAxis.y,
		feedbackProjectionAxis.z );
	fprintf( f, "\t\t\t\t%s %lf\n", getConLineString(CON_CD), cd );
	fprintf( f, "\t\t\t\t%s %lf\n", getConLineString(CON_CV), cv );
	if (dMin > -1000) fprintf( f, "\t\t\t\t%s %lf\n", getConLineString(CON_D_MIN), dMin);
	if (dMax < 1000) fprintf( f, "\t\t\t\t%s %lf\n", getConLineString(CON_D_MAX), dMax);
	if (vMin > -1000) fprintf( f, "\t\t\t\t%s %lf\n", getConLineString(CON_V_MIN), vMin);
	if (vMax < 1000) fprintf( f, "\t\t\t\t%s %lf\n", getConLineString(CON_V_MAX), vMax);


	fprintf( f, "\t\t\t%s\n", getConLineString(CON_FEEDBACK_END) );
}


DoubleStanceFeedback::DoubleStanceFeedback(){
	feedbackProjectionAxis = Vector3d();
	totalMultiplier = 15;
	vMin = -0.3;
	vMax = 0.3;
	dMin = -10;
	dMax = 10;

	minFeedbackValue = 0;
	maxFeedbackValue = 0;
}

/**
	This method returns a scalar that is the ammount of feedback that needs to be added to a trajectory. It is a function of the
	phase in the controller's state (between 0 and 1), the vector between the stance foot and the center of mass, and the velocity of
	the center of mass.
*/
double DoubleStanceFeedback::getFeedbackContribution(SimBiController* con, Joint* j, double phi, Vector3d d, Vector3d v){
	RigidBody* theBody;
	Vector3d tmpP;
	double offset = 0;

	if (j != NULL)
		theBody = j->getChild();
	else
		theBody = con->character->getRoot();

	//we might want to initialize it (or add to it) some default value in case we are trying to control its projection
	offset = 0; 
	double dToUse = theBody->getLocalCoordinates(con->doubleStanceCOMError).dotProductWith(feedbackProjectionAxis);
	if (dToUse < dMin) dToUse = dMin;
	if (dToUse > dMax) dToUse = dMax;


	double vToUse = theBody->getLocalCoordinates(con->comVelocity).dotProductWith(feedbackProjectionAxis);
	
	if (vToUse < vMin) vToUse = vMin;
	if (vToUse > vMax) vToUse = vMax;

	double err = (dToUse * cd - vToUse * cv + offset);

	double result = err * totalMultiplier;
/*
	if (j)
		tprintf("%s gets: %lf\n", j->getName(), result);
	else
		tprintf("root gets: %lf\n", result);
*/
	if (result < minFeedbackValue) result = minFeedbackValue;
	if (result > maxFeedbackValue) result = maxFeedbackValue;

//	if (j == NULL && cv > 0.4)
//		logPrint("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", theBody->getLocalCoordinates(con->COMOffset).dotProductWith(feedbackProjectionAxis), dToUse, theBody->getLocalCoordinates(con->comVelocity).dotProductWith(feedbackProjectionAxis), vToUse, result, forceRatioWeight);

	return result;
}


void DoubleStanceFeedback::writeToFile(FILE* f){
	if (f == NULL)
		return;

	fprintf( f, "\t\t\t%s COM_SupportCenterFeedback\n", getConLineString(CON_FEEDBACK_START) );

	fprintf(f, "Not yet implemented...\n");

	fprintf( f, "\t\t\t%s\n", getConLineString(CON_FEEDBACK_END) );
}


void DoubleStanceFeedback::loadFromFile(FILE* f){
	if (f == NULL)
		throwError("File pointer is NULL - cannot read gain coefficients!!");

	//have a temporary buffer used to read the file line by line...
	char buffer[200];

	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		switch (lineType) {
			case CON_FEEDBACK_END:
				//we're done...
				return;
				break;
			case CON_COMMENT:
				break;
			case CON_D_MIN:
				if (sscanf(line, "%lf", &this->dMin)!=1)
					throwError("A dMin value must be specified!");
				break;
			case CON_D_MAX:
				if (sscanf(line, "%lf", &this->dMax)!=1)
					throwError("A dMax value must be specified!");
				break;
			case CON_V_MIN:
				if (sscanf(line, "%lf", &this->vMin)!=1)
					throwError("A vMin value must be specified!");
				break;
			case CON_V_MAX:
				if (sscanf(line, "%lf", &this->vMax)!=1)
					throwError("A vMax value must be specified!");
				break;
			case CON_CV:
				if (sscanf(line, "%lf", &this->cv)!=1)
					throwError("A cv value must be specified!");
				break;
			case CON_CD:
				if (sscanf(line, "%lf", &this->cd)!=1)
					throwError("A cd value must be specified!");
				break;
			case CON_MIN_FEEDBACK:
				if (sscanf(line, "%lf", &this->minFeedbackValue)!=1)
					throwError("A minFeedbackValue value must be specified!");
				break;
			case CON_MAX_FEEDBACK:
				if (sscanf(line, "%lf", &this->maxFeedbackValue)!=1)
					throwError("A maxFeedbackValue value must be specified!");
				break;
			case CON_FEEDBACK_PROJECTION_AXIS:
				if (sscanf(line, "%lf %lf %lf", &this->feedbackProjectionAxis.x, &this->feedbackProjectionAxis.y, &this->feedbackProjectionAxis.z)!=3)
					throwError("The axis for a trajectory is specified by three parameters!");
				this->feedbackProjectionAxis.toUnit();
				break;
			case CON_NOT_IMPORTANT:
				tprintf("Ignoring input line: \'%s\'\n", line);
				break;
			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
	throwError("Incorrect SIMBICON input file: No \'/jointTrajectory\' found ", buffer);
}



