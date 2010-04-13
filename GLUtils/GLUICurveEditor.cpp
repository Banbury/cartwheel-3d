#include "GLUICurveEditor.h"

#include <GLUtils/GLUtils.h>
#include <Include/glut.h>

#define MIN_WIDTH 150
#define MIN_HEIGHT 100

/**
	Default constructor
*/
GLUICurveEditor::GLUICurveEditor(GLUIContainer* parent, int x, int y, int width, int height ) :
	GLUIWindow(parent, x,y,width,height)
{
	title[0] = 0;

	minPos = 0;
	maxPos = 1;
	minVal = -5;
	maxVal = 5;
	trajectory = NULL;
	currTime = NULL;
	pointHeldIdx = -1;

	canEditPosition = true;
	canMoveHorizontally = true;
	canScaleHorizontally = true;

	// Set a reasonable min size
	if( width == 0 ) width = MIN_WIDTH;
	if( height == 0 ) height = MIN_HEIGHT;

	setMinSize( min(width,MIN_WIDTH), min(height,MIN_HEIGHT) );
	
}

/**
	Converts from a curve position to a viewport pixel X position
*/
double GLUICurveEditor::posToPixel( double pos ) {
	return (pos - minPos) * (rect.width-1) / (maxPos-minPos);
}


/**
	Converts from a curve value to a viewport pixel Y position
*/
double GLUICurveEditor::valToPixel( double val ) {
	return (maxVal-val) * (rect.height-1) / (maxVal-minVal);
}


/**
	Converts from a viewport pixel vector X position to a curve position vector
*/
double GLUICurveEditor::pixelVecToPos( int vecX ) {
	return vecX*(maxPos-minPos) / (double)(rect.width-1);
}

/**
	Converts from a viewport pixel X position to a curve position
*/
double GLUICurveEditor::pixelToPos( int posX ) {
	return minPos + pixelVecToPos(posX);
}

/**
	Converts from a viewport pixel vector Y position to a curve value vector
*/
double GLUICurveEditor::pixelVecToVal( int vecY ) {
	return -vecY*(maxVal-minVal) / (double)(rect.height-1);
}


/**
	Converts from a viewport pixel Y position to a curve value
*/
double GLUICurveEditor::pixelToVal( int posY ) {
	return maxVal + pixelVecToVal( posY );
}


/** 
	Draw the curve editor in the top-left corner of the OpenGL window
*/
void GLUICurveEditor::draw() {

	// Save projection matrix and sets it to a simple orthogonal projection
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(minPos, maxPos, minVal, maxVal);
	

	// Drow vertical ticks
	double vTickSize = pow( 10, floor(log10(maxPos-minPos)+0.5) - 1.0 );
	double vTickStart = ceil( minPos/vTickSize ) * vTickSize;
	int nbVTicks = 0;

	glColor3d(0.45,0.45,0.45);
	glLineWidth( 1.0f );
	glBegin( GL_LINES );
	for( double pos = vTickStart; pos <= maxPos; pos += vTickSize ) {
		glVertex2d( pos, minVal );
		glVertex2d( pos, maxVal );
		nbVTicks++;
	}
	glEnd();

	// Draw horizontal ticks
	double hTickSize = pow( 10, floor(log10(maxVal-minVal)+0.5) - 1.0 );
	double hTickStart = ceil( minVal/hTickSize ) * hTickSize;
	int nbHTicks = 0;

	glColor3d(0.45,0.45,0.45);
	glLineWidth( 1.0f );
	glBegin( GL_LINES );
	for( double val = hTickStart; val <= maxVal; val += hTickSize ) {
		glVertex2d( minPos, val );
		glVertex2d( maxPos, val );
		nbHTicks++;
	}
	glEnd();

	// Draw zeros
	glColor3d(0.55,0.55,0.55);
	glLineWidth( 2.0f );
	glBegin( GL_LINES );
	glVertex2d( 0, minVal );
	glVertex2d( 0, maxVal );
	glVertex2d( minPos, 0 );
	glVertex2d( maxPos, 0 );
	glEnd();

	// Print tick values
	double vTickDensity = (double)rect.width / nbVTicks;
	int vTickModulo = max(1, (int)(44.0/vTickDensity + 0.5));
	glColor3d(1,1,1);
	double deltaPos = (maxPos - minPos) * 0.025;
	double deltaVal = (maxVal - minVal) * 0.025;
	int nbDigit = 0;
	if( vTickSize < 1 ) nbDigit = (int)((-log10(vTickSize))+0.5);
	for( double pos = vTickStart; pos <= maxPos; pos += vTickSize ) {
		if( (int)(pos/vTickSize+0.5*SGN(pos)) % vTickModulo != 0 ) continue;
		glRasterPos2d( pos - deltaPos, minVal + deltaVal );
		if( -0.00000001 < pos && pos < 0.00000001 ) pos = 0;
		GLUtils::gprintf( "%.*f", nbDigit, pos);
	}


	double hTickDensity = (double)rect.height / nbHTicks;
	int hTickModulo = max(1, (int)(16.0/hTickDensity + 0.5));
	nbDigit = 0;
	if( hTickSize < 1 ) nbDigit = (int)((-log10(hTickSize))+0.5);
	for( double val = hTickStart; val <= maxVal; val += hTickSize ) {
		if( (int)(val/hTickSize+0.5*SGN(val)) % hTickModulo != 0 ) continue;
		glRasterPos2d( minPos + deltaPos/2, val - deltaVal );
		if( -0.00000001 < val && val < 0.00000001 ) val = 0;
		GLUtils::gprintf( "%.*f", nbDigit, val );
	}


	if( trajectory && trajectory->getKnotCount() > 0 ) {
	
		// Draw the trajectory itself
		glColor3d(0.8,0.8,0.8);
		glLineWidth( 2.0f );
		glBegin( GL_LINE_STRIP );
		for( int i=0; i<rect.width; ++i ) {
			double pos = pixelToPos(i);
			double val = trajectory->evaluate_catmull_rom(pos);
			glVertex2d( pos, val );
		}
		glEnd();


		// Draw control points along the trajectory
		glPointSize( 8.0f );
		glBegin( GL_POINTS );
		for( int i=0; i<trajectory->getKnotCount(); ++i ) {
			if( pointHeldIdx == i ) glColor3d(1,1,0); else glColor3d(1,1,1);
			double pos = trajectory->getKnotPosition(i);
			double val = trajectory->getKnotValue(i);
			glVertex2d( pos, val );
		}
		glEnd();
	
	}

	// Draw the current time
	if( currTime != NULL ) {
		glColor3d(0.7,0.7,0);
		glLineWidth( 1.0f );
		glBegin( GL_LINES );
		glVertex2d( *currTime, minVal );
		glVertex2d( *currTime, maxVal );
		glEnd();
	}

	// Draw a frame
	deltaPos = (maxPos - minPos) * 0.0001;
	deltaVal = (maxVal - minVal) * 0.0001;
	glLineWidth( 3.0f );
	glColor3d( 0.8, 0.8, 0.8 );
	glBegin( GL_LINE_LOOP );
	glVertex2d( minPos+deltaPos, minVal+deltaVal );
	glVertex2d( minPos+deltaPos, maxVal-deltaVal );
	glVertex2d( maxPos-deltaPos, maxVal-deltaVal );
	glVertex2d( maxPos-deltaPos, minVal+deltaVal );
	glEnd();

	// Restore projection to pixel mode
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Write title at top
	glColor3d( 0.8, 0.8, 0.8 );
	glBegin(GL_QUADS);
		glVertex2d(0, 20);
		glVertex2d(rect.width, 20);
		glVertex2d(rect.width, 0);
		glVertex2d(0, 0);
	glEnd();

	glColor3d( 0, 0, 0 );
	glRasterPos2d( 5, 15 );
	GLUtils::gprintf( title );

}

int GLUICurveEditor::findClosestControlPoint(int x, int y)
{
	if( trajectory == NULL ) return -1;
	for( int i=0; i<trajectory->getKnotCount(); ++i ) {
		double distX = posToPixel(trajectory->getKnotPosition(i)) - x;
		double distY = valToPixel(trajectory->getKnotValue(i)) - y;
		if( distX*distX + distY*distY < 25.0 )
			return i;
	}
	return -1;
}

// Capture a control point or the background (if index == -1)
void GLUICurveEditor::captureControlPoint( int index, int x, int y ) {
	if( hasCapture() ) return;
	pointHeldIdx = index;
	holdPosX = x;
	holdPosY = y;
	captureMouse();
}
	
void GLUICurveEditor::checkReleaseCapture( GLUIMouseEvent* mouseEvent ) {
	if( !hasCapture() ) return;
	if( mouseEvent->leftDown || mouseEvent->rightDown ) return;

	releaseMouse();
	pointHeldIdx = -1;
	holdPosX = -1;
	holdPosY = -1;
}

/**
	This method is used when a mouse event gets generated. This method returns true if the message gets processed, false otherwise.
	The coordinates are relative to top-left (OpenGL are usually relative to bot-left)
	Returns false if the event is not handled, true if it was handled
*/
bool GLUICurveEditor::onLeftDown( GLUIMouseEvent* mouseEvent )  {

	int x = mouseEvent->x;
	int y = mouseEvent->y;

	// If shift if pressed, insert a knot
	if( mouseEvent->shiftDown ) {
		if( !trajectory ) return true;
		trajectory->addKnot( pixelToPos( x ), pixelToVal( y ) );
		return true;
	}

	// Capture a point if we're close enough, or the background
	captureControlPoint( findClosestControlPoint(x,y), x, y ); 

	return true;
}

bool GLUICurveEditor::onRightDown( GLUIMouseEvent* mouseEvent )  {

	int x = mouseEvent->x;
	int y = mouseEvent->y;

	// If shift if pressed, delete a knot
	if( mouseEvent->shiftDown ) {
		if( !trajectory ) return true;
		int pointRemoved = findClosestControlPoint(x,y);
		if( pointRemoved == -1 ) return true;
		trajectory->removeKnot( pointRemoved );
		return true;
	}

	// Hold the background for resizing
	captureControlPoint(-1, x, y);
	
	return true;
}


bool GLUICurveEditor::onLeftUp( GLUIMouseEvent* mouseEvent )  {
	checkReleaseCapture(mouseEvent);
	return true;
}

bool GLUICurveEditor::onRightUp( GLUIMouseEvent* mouseEvent )  {
	checkReleaseCapture(mouseEvent);
	return true;
}

bool GLUICurveEditor::onMotion( GLUIMouseEvent* mouseEvent )  {

	if( !mouseEvent->leftDown && !mouseEvent->rightDown ) return true;
	if( !hasCapture() ) return true;

	int x = mouseEvent->x;
	int y = mouseEvent->y;

	// Check if we're dragging a knot
	if( trajectory && pointHeldIdx >= 0 && pointHeldIdx < trajectory->getKnotCount() && mouseEvent->leftDown ) {

		// Compute the new value
		double newVal = pixelToVal( y );

		trajectory->setKnotValue( pointHeldIdx, newVal );

		if( canEditPosition ) {

			// Compute the new position
			double newPos = pixelToPos( x );
		
			// Make sure new position is legal (> than previous knot, < than next knot, > 0)
			if( pointHeldIdx > 0 ) {
				if( newPos <= trajectory->getKnotPosition(pointHeldIdx-1) ) 
					newPos = trajectory->getKnotPosition(pointHeldIdx-1) + 0.00001;
			}

			if( pointHeldIdx < trajectory->getKnotCount()-1 ) {
				if( newPos >= trajectory->getKnotPosition(pointHeldIdx+1) ) 
					newPos = trajectory->getKnotPosition(pointHeldIdx+1) - 0.00001;
			}

			trajectory->setKnotPosition( pointHeldIdx, newPos );
		}

		return true;
	}

	// No, then we're dragging the background
	if( mouseEvent->leftDown ) {
		// Translate view						

		double deltaY = pixelVecToVal(holdPosY - y);
		minVal += deltaY;
		maxVal += deltaY;
		holdPosY = y;

		if( canMoveHorizontally ) {		
			double deltaX = pixelVecToPos(holdPosX - x);
			minPos += deltaX;
			maxPos += deltaX;
			holdPosX = x;
		}
	}

	if( mouseEvent->rightDown ) {
		// Scale view
		
		double scaleY = (y - holdPosY)/(double)rect.height * 2.0f;
		if( scaleY >= 0 ) 
			scaleY = 1.0+scaleY;
		else
			scaleY = 1.0 / (1.0-scaleY);

		double midVal = (minVal+maxVal)/2.0;
		double sizeY = (maxVal-minVal)*scaleY;
		minVal = midVal - sizeY/2.0;
		maxVal = midVal + sizeY/2.0;
		holdPosY = y;
	
		if( canScaleHorizontally ) {		
			double scaleX = (holdPosX - x)/(double)rect.width * 2.0f;
			if( scaleX >= 0 ) 
				scaleX = 1.0+scaleX;
			else
				scaleX = 1.0 / (1.0-scaleX);

			double midPos = (minPos+maxPos)/2.0;
			double sizeX = (maxPos-minPos)*scaleX;
			minPos = midPos - sizeX/2.0;
			maxPos = midPos + sizeX/2.0;
			holdPosX = x;
		}

	}

	return true;
}
