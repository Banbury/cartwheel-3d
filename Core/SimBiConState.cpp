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

#include "SimBiConState.h"
#include <Utils/Utils.h>
#include "SimGlobals.h"
#include "SimBiController.h"

/** 
	Update this component to recenter it around the new given D and V trajectories
*/
void TrajectoryComponent::updateComponent(SimBiController* con, Joint* j, Trajectory1d& newDTrajX, Trajectory1d& newDTrajZ, Trajectory1d& newVTrajX, Trajectory1d& newVTrajZ, 
										   Trajectory1d* oldDTrajX, Trajectory1d* oldDTrajZ, Trajectory1d* oldVTrajX, Trajectory1d* oldVTrajZ, 
										  int nbSamples ) {

	if( bFeedback == NULL )
		return;

	double startPhi = 0;
	double endPhi = 0;
	if( baseTraj.getKnotCount() > 0 ) {
		startPhi = min( startPhi, baseTraj.getMinPosition() );
		endPhi = min( startPhi, baseTraj.getMaxPosition() );
	}
	if( newDTrajX.getKnotCount() > 0 ) {
		startPhi = max( startPhi, newDTrajX.getMinPosition() );
		endPhi = max( startPhi, newDTrajX.getMaxPosition() );
	}

	Trajectory1d result;
	Vector3d d0, v0, newD0, newV0;
	for( int i = 0; i < nbSamples; ++i ) {
		double interp = (double) i / (nbSamples - 1.0);
		double phi = startPhi * (1.0 - interp) + endPhi * interp;

		double baseAngle = 0;
		if (baseTraj.getKnotCount() > 0)
			baseAngle = baseTraj.evaluate_catmull_rom(phi);
		SimBiController::computeDorV( phi, &newDTrajX, &newDTrajZ, LEFT_STANCE, &newD0 );
		SimBiController::computeDorV( phi, &newVTrajX, &newVTrajZ, LEFT_STANCE, &newV0 );
		SimBiController::computeDorV( phi, oldDTrajX, oldDTrajZ, LEFT_STANCE, &d0 );
		SimBiController::computeDorV( phi, oldVTrajX, oldVTrajZ, LEFT_STANCE, &v0 );

		double feedback = computeFeedback(con, j, phi, newD0 - d0, newV0 - v0);

		if (reverseAngleOnLeftStance)
			baseAngle -= feedback;
		else
			baseAngle += feedback;

		result.addKnot( phi, baseAngle );
	}
	result.simplify_catmull_rom( 0.005 );
	baseTraj.copy( result );
}


/**
	This method is used to read the knots of a base trajectory from the file, where they are specified one (knot) on a line
*/
void TrajectoryComponent::writeBaseTrajectory(FILE* f){
	if (f == NULL)
		return;

	fprintf( f, "\t\t\t%s\n", getConLineString(CON_BASE_TRAJECTORY_START) );

	for( int i=0; i < baseTraj.getKnotCount(); ++i ) {
		fprintf( f, "\t\t\t\t%lf %lf\n", baseTraj.getKnotPosition(i), baseTraj.getKnotValue(i) );
	}

	fprintf( f, "\t\t\t%s\n", getConLineString(CON_BASE_TRAJECTORY_END) );
}

/**
	This method is used to read the knots of a base trajectory from the file, where they are specified one (knot) on a line
*/
void TrajectoryComponent::writeScaleTraj(FILE* f, const Trajectory1d& scaleTraj, char type){
	if (f == NULL)
		return;

	fprintf( f, "\t\t\t%cScaleTraj\n", type);

	for( int i=0; i < scaleTraj.getKnotCount(); ++i ) {
		fprintf( f, "\t\t\t\t%lf %lf\n", scaleTraj.getKnotPosition(i), scaleTraj.getKnotValue(i) );
	}

	fprintf( f, "\t\t\t/%cScaleTraj\n", type );
}

/**
	This method is used to write a trajectory to a file
*/
void TrajectoryComponent::writeTrajectoryComponent(FILE* f){
	if (f == NULL)
		return;

	fprintf( f, "\t\t%s\n", getConLineString(CON_TRAJ_COMPONENT) );

	fprintf( f, "\t\t\t%s %lf %lf %lf\n", getConLineString(CON_ROTATION_AXIS), 
				rotationAxis.x, rotationAxis.y, rotationAxis.z );

	if( reverseAngleOnLeftStance )
		fprintf( f, "\t\t\t%s left\n", getConLineString(CON_REVERSE_ANGLE_ON_STANCE) );
	else if( reverseAngleOnRightStance )
		fprintf( f, "\t\t\t%s right\n", getConLineString(CON_REVERSE_ANGLE_ON_STANCE) );

	if( bFeedback )
		bFeedback->writeToFile( f );

	writeBaseTrajectory(f);
	if (dTrajScale.getKnotCount() > 0)
		writeScaleTraj(f, dTrajScale, 'd');

	if (vTrajScale.getKnotCount() > 0)
		writeScaleTraj(f, vTrajScale, 'v');

	fprintf( f, "\t\t%s\n", getConLineString(CON_TRAJ_COMPONENT_END) );
}

/**
	This method is used to read a trajectory from a file
*/
void TrajectoryComponent::readTrajectoryComponent(FILE* f){
	if (f == NULL)
		throwError("File pointer is NULL - cannot read gain coefficients!!");

	//have a temporary buffer used to read the file line by line...
	char buffer[200];
	char tmpString[200];

	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		switch (lineType) {
			case CON_TRAJ_COMPONENT_END:
				//we're done...
				return;
				break;
			case CON_COMMENT:
				break;
			case CON_ROTATION_AXIS:
				if (sscanf(line, "%lf %lf %lf", &this->rotationAxis.x, &this->rotationAxis.y, &this->rotationAxis.z)!=3)
					throwError("The axis for a trajectory is specified by three parameters!");
				this->rotationAxis.toUnit();
				break;
			case CON_FEEDBACK_START:
				//read the kind of feedback that is applicable to this state
				if (sscanf(line, "%s", tmpString) != 1)
					throwError("The kind of feedback to be used for a trajectory must be specified (e.g. linear)");
				delete bFeedback;
				bFeedback = NULL;
				if (strncmp(tmpString, "linear", 6) == 0){
					bFeedback = new LinearBalanceFeedback();
					bFeedback->loadFromFile(f);
				}else if (strncmp(tmpString, "doubleStance", 12) == 0){
					bFeedback = new DoubleStanceFeedback();
					bFeedback->loadFromFile(f);
				}else
					throwError("Unrecognized type of feedback: \'%s\'", line);
				break;
			case CON_BASE_TRAJECTORY_START:
				//read in the base trajectory
				SimBiConState::readTrajectory1d(f, baseTraj, CON_BASE_TRAJECTORY_END);
				break;
			case CON_D_SCALE_TRAJECTORY_START:
				//read in the base trajectory
				SimBiConState::readTrajectory1d(f, dTrajScale, CON_D_SCALE_TRAJECTORY_END);
				break;
			case CON_V_SCALE_TRAJECTORY_START:
				//read in the base trajectory
				SimBiConState::readTrajectory1d(f, vTrajScale, CON_V_SCALE_TRAJECTORY_END);
				break;
			case CON_REVERSE_ANGLE_ON_STANCE:
				if (strncmp(trim(line), "left", 4) == 0)
					reverseAngleOnLeftStance = true;
				else if (strncmp(trim(line), "right", 5) == 0)
					reverseAngleOnRightStance = true;
				else 
					throwError("When using the \'startingStance\' keyword, \'left\' or \'right\' must be specified!");
				break;
			case CON_NOT_IMPORTANT:
				tprintf("Ignoring input line-2: \'%s\'\n", line); 
				break;
			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
	throwError("Incorrect SIMBICON input file: No \'/trajectory\' found ", buffer);
}

/**
	This method is used to write the knots of a strength trajectory to the file, where they are specified one (knot) on a line
*/
void Trajectory::writeStrengthTrajectory(FILE* f){
	if (f == NULL)
		return;

	fprintf( f, "\t\t\t%s\n", getConLineString(CON_STRENGTH_TRAJECTORY_START) );

	for( int i=0; i < strengthTraj.getKnotCount(); ++i ) {
		fprintf( f, "\t\t\t\t%lf %lf\n", strengthTraj.getKnotPosition(i), strengthTraj.getKnotValue(i) );
	}

	fprintf( f, "\t\t\t%s\n", getConLineString(CON_STRENGTH_TRAJECTORY_END) );
}

/**
	This method is used to write a trajectory to a file
*/
void Trajectory::writeTrajectory(FILE* f){
	if (f == NULL)
		return;

	fprintf( f, "\t%s %s\n", getConLineString(CON_TRAJECTORY_START), jName );

	if (relToCharFrame)
		fprintf(f, "\t%s\n", getConLineString(CON_CHAR_FRAME_RELATIVE));

	writeStrengthTrajectory( f );

	for( uint i=0; i < components.size(); ++i ) {
		fprintf( f, "\n" );
		components[i]->writeTrajectoryComponent( f );
	}

	fprintf( f, "\t%s\n", getConLineString(CON_TRAJECTORY_END) );
}

/**
	This method is used to read a trajectory from a file
*/
void Trajectory::readTrajectory(FILE* f){
	if (f == NULL)
		throwError("File pointer is NULL - cannot read gain coefficients!!");

	//have a temporary buffer used to read the file line by line...
	char buffer[200];

	TrajectoryComponent* newComponent;

	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		switch (lineType) {
			case CON_STRENGTH_TRAJECTORY_START:
				//read in the base trajectory
				SimBiConState::readTrajectory1d(f, strengthTraj, CON_STRENGTH_TRAJECTORY_END );
				break;
			case CON_TRAJECTORY_END:
				//we're done...
				return;
				break;
			case CON_CHAR_FRAME_RELATIVE:
				relToCharFrame = true;
				break;
			case CON_COMMENT:
				break;
			case CON_TRAJ_COMPONENT:
				//read in the base trajectory
				newComponent = new TrajectoryComponent();
				newComponent->readTrajectoryComponent(f);
				components.push_back(newComponent);
				break;
			case CON_NOT_IMPORTANT:
				tprintf("Ignoring input line: \'%s\'\n", line); 
				break;
			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
	throwError("Incorrect SIMBICON input file: No \'/trajectory\' found ", buffer);
}

/**
	This method is used to read the state parameters from a file
*/
void SimBiConState::readState(FILE* f, int offset){
	if (f == NULL)
		throwError("File pointer is NULL - cannot read gain coefficients!!");

	//have a temporary buffer used to read the file line by line...
	char buffer[200];
	Trajectory* tempTraj;


	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		switch (lineType) {
			case CON_STATE_END:
				//we're done...
				return;
				break;
			case CON_NEXT_STATE:
				if (sscanf(line, "%d", &this->nextStateIndex) != 1)
					throwError("An index must be specified when using the \'nextState\' keyword");
				this->nextStateIndex += offset;
				break;
			case CON_STATE_DESCRIPTION:
				strcpy(this->name, trim(line));
				break;
			case CON_STATE_TIME:
				if (sscanf(line, "%lf", &stateTime)!=1)
					throwError("The time that is expected to be spent in this state needs to be provided.");
				break;
			case CON_STATE_STANCE:
				reverseStance = false;
				keepStance = false;
				if (strncmp(trim(line), "left",4) == 0)
					stateStance = LEFT_STANCE;
				else
					if (strncmp(trim(line), "right", 5) == 0)
						stateStance = RIGHT_STANCE;
					else
						if (strncmp(trim(line), "reverse", 7) == 0)
							reverseStance = true;
						else if (strncmp(trim(line), "same", 4) == 0)
								keepStance = true;
							else
								throwError("When using the \'stateStance\' keyword, \'left\', \'right\' or \'reverse\' must be specified.");
				break;
			case CON_TRANSITION_ON:
				transitionOnFootContact = false;
				if (strncmp(trim(line), "footDown", 8) == 0)
					transitionOnFootContact = true;
				else
					if (strncmp(trim(line), "timeUp", 6) == 0)
						//nothn' to do, since this is the default
						;
					else
						throwError("When using the \'transitionOn\' keyword, \'footDown\' or \'timeUp\' must be specified.");
				break;
			case CON_TRAJECTORY_START:
				//create a new trajectory, and read its information from the file
				tempTraj = new Trajectory();
				strcpy(tempTraj->jName, trim(line));
				tempTraj->readTrajectory(f);
				this->sTraj.push_back(tempTraj);
				break;

			case CON_D_TRAJX_START:
				if( dTrajX != NULL )
					throwError( "Two dTrajX trajectory, this is illegal!" );
				dTrajX = new Trajectory1d();
				readTrajectory1d( f, *dTrajX, CON_D_TRAJX_END );
				break;

			case CON_D_TRAJZ_START:
				if( dTrajZ != NULL )
					throwError( "Two dTrajZ trajectory, this is illegal!" );
				dTrajZ = new Trajectory1d();
				readTrajectory1d( f, *dTrajZ, CON_D_TRAJZ_END );
				break;

			case CON_V_TRAJX_START:
				if( vTrajX != NULL )
					throwError( "Two vTrajX trajectory, this is illegal!" );
				vTrajX = new Trajectory1d();
				readTrajectory1d( f, *vTrajX, CON_V_TRAJX_END );
				break;

			case CON_V_TRAJZ_START:
				if( vTrajZ != NULL )
					throwError( "Two vTrajZ trajectory, this is illegal!" );
				vTrajZ = new Trajectory1d();
				readTrajectory1d( f, *vTrajZ, CON_V_TRAJZ_END );
				break;

			case CON_COMMENT:
				break;


			case CON_NOT_IMPORTANT:
				tprintf("Ignoring input line: \'%s\'\n", line);
				break;


			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
	throwError("Incorrect SIMBICON input file: No \'/State\' found", buffer);
}

/**
	This method is used to write the state parameters to a file
*/
void SimBiConState::writeState(FILE* f, int index){
	if (f == NULL)
		return;

	fprintf( f, "%s %d\n", getConLineString(CON_STATE_START), index );

	fprintf( f, "\t%s %s\n", getConLineString(CON_STATE_DESCRIPTION), name );
	fprintf( f, "\t%s %d\n", getConLineString(CON_NEXT_STATE), nextStateIndex );
	fprintf( f, "\t%s %s\n", getConLineString(CON_TRANSITION_ON), 
		transitionOnFootContact?"footDown":"timeUp" );
	
	if( reverseStance )
		fprintf( f, "\t%s reverse\n", getConLineString(CON_STATE_STANCE) );
	else if( keepStance )
		fprintf( f, "\t%s same\n", getConLineString(CON_STATE_STANCE) );
	else if( stateStance == LEFT_STANCE )
		fprintf( f, "\t%s left\n", getConLineString(CON_STATE_STANCE) );
	else if( stateStance == RIGHT_STANCE )
		fprintf( f, "\t%s right\n", getConLineString(CON_STATE_STANCE) );

	fprintf( f, "\t%s %lf\n", getConLineString(CON_STATE_TIME), stateTime );
	
	fprintf( f, "\n" );

	if( dTrajX != NULL )
		writeTrajectory1d( f, *dTrajX, CON_D_TRAJX_START, CON_D_TRAJX_END );
	if( dTrajZ != NULL )
		writeTrajectory1d( f, *dTrajZ, CON_D_TRAJZ_START, CON_D_TRAJZ_END );
	if( vTrajX != NULL )
		writeTrajectory1d( f, *vTrajX, CON_V_TRAJX_START, CON_V_TRAJX_END );
	if( vTrajZ != NULL )
		writeTrajectory1d( f, *vTrajZ, CON_V_TRAJZ_START, CON_V_TRAJZ_END );

	fprintf( f, "\n" );

	for( uint i=0; i<sTraj.size(); ++i ) {
		fprintf( f, "\n" );
		sTraj[i]->writeTrajectory( f );	
	}
	
	fprintf( f, "%s\n", getConLineString(CON_STATE_END) );


}


/**
	This method is used to read the knots of a strength trajectory from the file, where they are specified one (knot) on a line
*/
void SimBiConState::readTrajectory1d(FILE* f, Trajectory1d& result, int endingLineType ){
	if (f == NULL)
		throwError("File pointer is NULL - cannot read gain coefficients!!");

	//have a temporary buffer used to read the file line by line...
	char buffer[200];
	double temp1, temp2;

	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		if (strlen(buffer)>195)
			throwError("The input file contains a line that is longer than ~200 characters - not allowed");
		char *line = lTrim(buffer);
		int lineType = getConLineType(line);
		if( lineType == endingLineType )
			//we're done...
			return;

		switch (lineType) {
			case CON_COMMENT:
				break;
			case CON_NOT_IMPORTANT:
				//we expect pairs of numbers, one pair on each row, so see if we have a valid pair
				if (sscanf(line, "%lf %lf", &temp1, &temp2) == 2){
					result.addKnot(temp1, temp2);
				}else
					tprintf("Ignoring input line: \'%s\'\n", line); 
				break;
			default:
				throwError("Incorrect SIMBICON input file: \'%s\' - unexpected line.", buffer);
		}
	}
	throwError("Incorrect SIMBICON input file: Trajectory not closed ", buffer);
}




/**
	This method is used to write a trajectory to the file
*/
void SimBiConState::writeTrajectory1d(FILE* f, Trajectory1d& result, int startingLineType, int endingLineType ){
	if (f == NULL)
		return;

	fprintf( f, "\t%s\n", getConLineString(startingLineType) );

	for( int i=0; i < result.getKnotCount(); ++i ) {
		fprintf( f, "\t\t%lf %lf\n", result.getKnotPosition(i), result.getKnotValue(i) );
	}

	fprintf( f, "\t%s\n", getConLineString(endingLineType) );
}


/** 
	Update all the trajectories to recenter them around the new given D and V trajectories
	Also save these new D and V trajectories.
*/
void SimBiConState::updateDVTrajectories(SimBiController* con, Joint* j, Trajectory1d& newDTrajX, Trajectory1d& newDTrajZ, Trajectory1d& newVTrajX, Trajectory1d& newVTrajZ, int nbSamples ) {

	int nbTraj = sTraj.size();
	for( int i = 0; i < nbTraj; ++i ) {		
		sTraj[i]->updateComponents( con, j, newDTrajX, newDTrajZ, newVTrajX, newVTrajZ, dTrajX, dTrajZ, vTrajX, vTrajZ, nbSamples );
	}
	
	if( dTrajX != NULL )
		delete dTrajX;
	if( dTrajZ != NULL )
		delete dTrajZ;
	if( vTrajX != NULL )
		delete vTrajX;
	if( vTrajZ != NULL )
		delete vTrajZ;
	dTrajX = new Trajectory1d( newDTrajX );
	dTrajZ = new Trajectory1d( newDTrajZ );
	vTrajX = new Trajectory1d( newVTrajX );
	vTrajZ = new Trajectory1d( newVTrajZ );
}

