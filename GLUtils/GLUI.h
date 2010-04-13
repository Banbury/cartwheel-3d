#pragma once

// Defines and some basic classes for OpenGL UI elements.

#include <GLUtils/GLUtilsDll.h>

#include <Utils/Utils.h>
#include <Include/glHeaders.h>

// All GLUI elements are defined with respect to top-left corner (as typical UI, not OGL)

#define GLUI_VISIBLE     0x00001
#define GLUI_HAS_CONTENT 0x00002


#define GLUI_EXPAND     0x00001

#define GLUI_VERTICAL     0x00001
#define GLUI_HORIZONTAL   0x00002


#define GLUIFlipOrientation( orientation ) (((orientation)==GLUI_VERTICAL)?GLUI_HORIZONTAL:GLUI_VERTICAL)

class GLUTILS_DECLSPEC GLUISize {
public:
	int width, height;

	GLUISize() : width(0), height(0) {}
	GLUISize( int width, int height ) : width(width), height(height) {}
	void set( int width, int height) {
		this->width = width; this->height = height;
	}
	int get( int orientation ) const {
		if( orientation == GLUI_VERTICAL )
			return height;
		else
			return width;
	}
	void setOrientation( int orientation, int size ) {
		if( orientation == GLUI_VERTICAL )
			height = size;
		else
			width = size;
	}

};

class GLUTILS_DECLSPEC GLUIPos {
public:
	int x, y;

	GLUIPos() : x(0), y(0) {}
	GLUIPos( int x, int y ) : x(x), y(y) {}
	void set( int x, int y ) {
		this->x = x; this->y = y;
	}
	int get( int orientation ) const {
		if( orientation == GLUI_VERTICAL )
			return y;
		else
			return x;
	}
	void setOrientation( int orientation, int loc ) {
		if( orientation == GLUI_VERTICAL )
			y = loc;
		else
			x = loc;
	}

};

class GLUTILS_DECLSPEC GLUIRect {
public:
	int x, y, width, height;

	GLUIRect() : x(0), y(0), width(0), height(0) {}
	GLUIRect( int x, int y, int width, int height ) : x(x), y(y), width(width), height(height) {}
	void set(int x,int y, int width, int height) {
		this->x = x; this->y = y; this->width = width; this->height = height;
	}

	bool pointInside( int x, int y ) {
		return x >= this->x && y >= this->y && x < this->x+width && y < this->y+height;
	}

	int getSize( int orientation ) const {
		if( orientation == GLUI_VERTICAL )
			return height;
		else
			return width;
	}
};

class GLUTILS_DECLSPEC GLUIColor {
public:
	double r,g,b,a;

	GLUIColor() : r(0), g(0), b(0), a(0) {}
	GLUIColor( double r, double g, double b, double a ) : r(r), g(g), b(b), a(a) {}
	void set( double r, double g, double b, double a ) {
		this->r = r; this->g = g; this->b = b; this->a = a;
	}

	void activate() const { glColor4dv((const GLdouble *)&r); }

};

class GLUTILS_DECLSPEC GLUIMouseEvent {

public:
	bool altDown;
	bool controlDown;
	bool leftDown;
	bool middleDown;
	bool rightDown;
	bool metaDown;
	bool shiftDown;
	long x;
	long y;
	int wheelRotation;
	int wheelDelta;
	int linesPerAction;
	bool dragging; // True when moving with button down
	bool moving;   // True when moving without any button

	bool skip;     // Will be set to true if the event should skip GLUI and go to the container of the top-level window
};
