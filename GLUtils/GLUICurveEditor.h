#pragma once

#include <GLUtils/GLUtilsDll.h>

#include <GLUtils/GLUIWindow.h>
#include <MathLib/Trajectory.h>

/**
  * This class is used to draw a simple curve editor in the OpenGL window.
  */
class GLUTILS_DECLSPEC GLUICurveEditor : public GLUIWindow {

protected:
	
	// Define the viewing boundaries of the curve
	double minPos, maxPos, minVal, maxVal;

	// The title of the window
	char title[100];

	// Index of the control point held by the mouse (-1 if holding the background)
	int pointHeldIdx;

	// Position where background is being help, in pixel
	int holdPosX, holdPosY;

	// The trajectory to display and edit
	Trajectory1d* trajectory;

	// The time to display
	double *currTime;

	// Some flags
	bool canEditPosition;
	bool canMoveHorizontally;
	bool canScaleHorizontally;

	/**
		Converts from a curve position to a pixel X position
	*/
	double posToPixel( double pos );

	/**
		Converts from a curve valueto a pixel Y position
	*/
	double valToPixel( double val );

	/**
		Converts from a pixel vector X position to a curve position vector
	*/
	double pixelVecToPos( int vecX );

	/**
		Converts from a pixel X position to a curve position
	*/
	double pixelToPos( int posX );

	/**
		Converts from a pixel vector Y position to a curve value vector
	*/
	double pixelVecToVal( int vecY );

	/**
		Converts from a pixel Y position to a curve value
	*/
	double pixelToVal( int posY );

	int findClosestControlPoint(int x, int y);
	void captureControlPoint( int index, int x, int y );
	void checkReleaseCapture( GLUIMouseEvent* mouseEvent );

public:

	/**
		Default constructor
	*/
	GLUICurveEditor(GLUIContainer* parent, int x=0, int y=0, int width=0, int height=0 );

	void setTitle( const char* title ) {
		strncpy( this->title, title, 99 );
	}

	/**
		Attaches a trajectory to be edited
		The CurveEditor doesn't own the trajectory and the caller is responsible for destroying
		the trajectory. If the trajectory is destroyed, the editor should be notified by passing "null"
	*/
	void setTrajectory( Trajectory1d* trajectory ) { this->trajectory = trajectory; }

	const Trajectory1d* getTrajectory() const { return trajectory; }

	void setCurrTime( double* currTime ) { this->currTime = currTime; }

	const double* getCurrTime() const { return currTime; }

	/** 
		Draw the curve editor in the top-left corner of the OpenGL window
	*/
	void draw();

	bool onLeftDown( GLUIMouseEvent* mouseEvent );
	bool onRightDown( GLUIMouseEvent* mouseEvent );
	bool onLeftUp( GLUIMouseEvent* mouseEvent );
	bool onRightUp( GLUIMouseEvent* mouseEvent );
	bool onMotion( GLUIMouseEvent* mouseEvent );

};