#include "GLUICheckBox.h"

#include <GLUtils/GLUtils.h>
#include <Include/glut.h>

#define MIN_WIDTH 14
#define MIN_HEIGHT 14

#define SIZE_X 10
#define SIZE_Y 10


/**
	Default constructor
*/
GLUICheckBox::GLUICheckBox(GLUIContainer* parent, int x, int y, int width, int height, int minWidth, int minHeight, bool checked ) :
	GLUIWindow(parent, x,y,width,height,minWidth,minHeight)
{
	this->checked = checked;
	this->checkBoxCallback = NULL;

	// Set a reasonable min size
	if( minWidth < 0 ) {
		if( width == 0 ) minWidth = MIN_WIDTH;
		else minWidth = min( width, MIN_WIDTH );
	}

	if( minHeight < 0 ) {
		if( height == 0 ) minHeight = MIN_HEIGHT;
		else minHeight = min( height, MIN_HEIGHT );
	}
	
	setMinSize( minWidth, minHeight );
}

	
/** 
	Draw the curve editor in the top-left corner of the OpenGL window
*/
void GLUICheckBox::draw() {

	int x0 = (rect.width-SIZE_X)/2;
	int y0 = (rect.height-SIZE_X)/2;

	glColor3f(1,1,1);
	glBegin( GL_LINE_LOOP );
	glVertex2i(x0,y0);
	glVertex2i(x0+SIZE_X,y0);
	glVertex2i(x0+SIZE_X,y0+SIZE_Y);
	glVertex2i(x0,y0+SIZE_Y);
	glEnd();

	if (checked) {
		glBegin( GL_LINES );
		glVertex2d(x0,y0);
		glVertex2d(x0+SIZE_X,y0+SIZE_Y);
		glVertex2d(x0+SIZE_X,y0);
		glVertex2d(x0,y0+SIZE_Y);
		glEnd();
	}

}

bool GLUICheckBox::onLeftDown( GLUIMouseEvent* mouseEvent ) {
	
	int x0 = (rect.width-SIZE_X)/2;
	int y0 = (rect.height-SIZE_X)/2;

	if ( mouseEvent->x < x0 || mouseEvent->x > x0 + SIZE_X ||
		 mouseEvent->y < y0 || mouseEvent->y > y0 + SIZE_Y )
		 return true;

	checked = !checked;
	executeCheckBoxCallback();
	return true;

}
