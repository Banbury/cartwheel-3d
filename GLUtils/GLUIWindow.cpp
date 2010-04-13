#include "GLUIWindow.h"
#include "GLUIContainer.h"
#include "GLUISizer.h"


GLUIWindow::GLUIWindow(GLUIContainer* parent, int x, int y, int width, int height, int minWidth, int minHeight ) : 
	parent(parent),
	rect(x,y,width,height),
	minSize(minWidth,minHeight),
	bgColor(0,0,0,0.7),
	sizer(NULL),
	flags(GLUI_VISIBLE|GLUI_HAS_CONTENT) {
	if( parent != NULL )
		parent->addChild(this);
}

GLUIWindow::~GLUIWindow() {
	if( parent != NULL )
		parent->detachChild(this);
	if( sizer != NULL )
		delete sizer;
}

void GLUIWindow::setSizer( GLUISizer* sizer ) {
	if( this->sizer != NULL )
		delete this->sizer;
	this->sizer = sizer;
	if( sizer != NULL )
		sizer->setDimension( rect.x, rect.y, rect.width, rect.height );
}


void GLUIWindow::setSize( int width, int height ) {
	rect.width  = width;
	rect.height = height;
	if( sizer != NULL )
		sizer->setDimension( 0, 0, width, height );
}

void GLUIWindow::setDimension( int x, int y, int width, int height ) {
	rect.x = x;
	rect.y = y;
	rect.width  = width;
	rect.height = height;
	if( sizer != NULL )
		sizer->setDimension( 0, 0, width, height );
}

GLUISize GLUIWindow::getMinSize() const {
	if( sizer == NULL )
		return minSize;
	GLUISize realMinSize = sizer->calcMinSize();
	realMinSize.width = max( realMinSize.width, minSize.width );
	realMinSize.height = max( realMinSize.height, minSize.height );
	return realMinSize;
}

void GLUIWindow::setVisible(bool visible) { 
	if(visible) {
		if (flags & GLUI_VISIBLE) return; 
		flags |= GLUI_VISIBLE; 
		getParent()->layout();
	}
	else        
		flags &= ~GLUI_VISIBLE;
}

void GLUIWindow::layout() {
	if( sizer != NULL )
		sizer->layout();
}

void GLUIWindow::captureMouse(GLUIWindow* window) {
	if( window == NULL ) window = this;
	if( parent != NULL )
		parent->captureMouse(window);
}

void GLUIWindow::releaseMouse(GLUIWindow* window) {
	if( window == NULL ) window = this;
	if( parent != NULL )
		parent->releaseMouse(window);
}

bool GLUIWindow::hasCapture(GLUIWindow* window) {
	if( window == NULL ) window = this;
	if( parent != NULL )
		return parent->hasCapture(window);
	return false;
}

bool GLUIWindow::beginDraw(int offsetX, int offsetY, int maxWidth, int maxHeight, int width, int height) const {

	if(width < 0) {
		width = rect.width;
		maxWidth = width;
	}
	if(height < 0) {
		height = rect.height;
		maxHeight = height;
	}

	int subWindowX   = rect.x+offsetX;
	int subWindowY   = rect.y+offsetY;

	// int subWindowRight  = subWindowX + width
	// int thisWindowRight = rect.x + rect.width
	// int goodRight       = min(subWindowRight,thisWindowRight)
	// int subWindowWidth  = goodRight - subWindowX
	maxWidth  = min( maxWidth,  rect.x + rect.width  - subWindowX );
	maxHeight = min( maxHeight, rect.y + rect.height - subWindowY );

	if( width <= 0 || height <= 0 || maxWidth <= 0 || maxHeight <= 0 )
		return false;

	if( parent != NULL )
		return parent->beginDraw( subWindowX, subWindowY, maxWidth, maxHeight, width, height );



	// Save current viewport settings to restore state at the end
	int viewportSettings[4];
	glGetIntegerv(GL_VIEWPORT, viewportSettings);

	// Check that the desired partly falls inside the currend viewport
	int subWindowGLY = viewportSettings[3]-subWindowY-height;

	// Cohen-sutherland algorithm
	// bits: left-right-top-bottom
	int vx0 = viewportSettings[0];
	int vx1 = viewportSettings[0] + viewportSettings[2];
	int vy0 = viewportSettings[1];
	int vy1 = viewportSettings[1] + viewportSettings[3];
	int mx0 = subWindowX;
	int mx1 = subWindowX + width;
	int my0 = subWindowGLY;
	int my1 = subWindowGLY + height;

	int left  = 0x1;
	int right = 0x2;
	int bottom= 0x4;
	int top   = 0x8;
	int p1Bits = 0; // p1 = (mx0,my0)
	int p2Bits = 0; // p2 = (mx0,my1)
	int p3Bits = 0; // p3 = (mx1,my0)
	int p4Bits = 0; // p4 = (mx1,my1)
	if( mx0 < vx0 ) { p1Bits |= left; p2Bits |= left; }
	if( mx0 > vx1 ) { p1Bits |= right; p2Bits |= right; }
	if( mx1 < vx0 ) { p3Bits |= left; p4Bits |= left; }
	if( mx1 > vx1 ) { p3Bits |= right; p4Bits |= right; }
	if( my0 < vy0 ) { p1Bits |= bottom; p3Bits |= bottom; }
	if( my0 > vy1 ) { p1Bits |= top; p3Bits |= top; }
	if( my1 < vy0 ) { p2Bits |= bottom; p4Bits |= bottom; }
	if( my1 > vy1 ) { p2Bits |= top; p4Bits |= top; }
	if( p1Bits != 0 && p2Bits != 0 && p3Bits != 0 && p4Bits != 0 && (p1Bits & p2Bits & p3Bits & p4Bits) != 0 )
		return false;

	// Save the attributes and set them to basic values
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Setup the viewport in the upper-left corner
	glViewport(subWindowX,subWindowGLY,width,height);


	int clippedWindowGLY = viewportSettings[3]-subWindowY-maxHeight;
	glScissor(subWindowX,clippedWindowGLY,maxWidth,maxHeight);
	glEnable(GL_SCISSOR_TEST);

	// Save projection matrix and sets it to a pixel-wise projection
	// The 0-0 pixel is at the top of the window
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glScaled(1,-1,1);
	gluOrtho2D(0, width, 0, height);
	
	// Sets model-view matrix to identity
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Clear the colors to draw on a black background

	bgColor.activate();
	glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(maxWidth, 0);
		glVertex2d(maxWidth, maxHeight);
		glVertex2d(0, maxHeight);
	glEnd();

	return true;
}

void GLUIWindow::endDraw() const {


	// Restore attributes and matrices
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();


	glPopAttrib();


}

