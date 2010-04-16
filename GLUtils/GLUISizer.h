#pragma once

#include <GLUtils/GLUtilsDll.h>

#include <GLUtils/GLUIWindow.h>
#include <algorithm>


class GLUISizer;
class GLUTILS_DECLSPEC GLUISizedWindow {
public:
	GLUIWindow* window;
	GLUISizer* sizer;
	GLUISize size;
	int proportion;
	int flag;

	GLUISizedWindow() :
		window(NULL),
		sizer(NULL),
		size(0,0),
		proportion(proportion),
		flag(flag) {}

	GLUISizedWindow( GLUIWindow* window, int proportion = 0, int flag = 0 ) :
		window(window),
		sizer(NULL),
		size(0,0),
		proportion(proportion),
		flag(flag) {}

	GLUISizedWindow( GLUISizer* sizer, int proportion = 0, int flag = 0 ) :
		window(NULL),
		sizer(sizer),
		size(0,0),
		proportion(proportion),
		flag(flag) {}

	GLUISizedWindow( int width, int height, int proportion = 0, int flag = 0 ) :
		window(NULL),
		sizer(NULL),
		size(width,height),
		proportion(proportion),
		flag(flag) {}

};

GLUTILS_TEMPLATE( DynamicArray<GLUISizedWindow> )

class GLUTILS_DECLSPEC GLUISizer {

protected:
	DynamicArray<GLUISizedWindow> windows;

	GLUIRect rect;

	void deleteAllSizers() {
		for( uint i=0; i < windows.size(); ++i ) {
			if( windows[i].sizer != NULL ) {
				windows[i].sizer->deleteAllSizers();
				delete windows[i].sizer;
			}
		}
	}
public:
	GLUISizer() :
		rect(0,0,-1,-1) {}
	virtual ~GLUISizer() { 
		deleteAllSizers();
		windows.clear(); 
	}

	void add( GLUIWindow* window, int proportion = 0, int flag = 0 ) {
		windows.push_back( GLUISizedWindow(window, proportion, flag) );
	}

	void addSizer( GLUISizer* sizer_disown, int proportion = 0, int flag = 0 ) {
		windows.push_back( GLUISizedWindow(sizer_disown, proportion, flag) );
	}

	void addSpacer( int size ) {
		windows.push_back( GLUISizedWindow(size,size,0) );
	}

	void addStretchSpacer( int proportion ) {
		windows.push_back( GLUISizedWindow(0,0,proportion) );
	}

	void insert( int index, GLUIWindow* window, int proportion = 0, int flag = 0 ) {
		windows.insert( windows.begin()+index, GLUISizedWindow(window, proportion, flag) );
	}

	void insertSizer( int index, GLUISizer* sizer_disown, int proportion = 0, int flag = 0 ) {
		windows.insert( windows.begin()+index, GLUISizedWindow(sizer_disown, proportion, flag) );
	}

	void insertSpacer( int index, int size ) {
		windows.insert( windows.begin()+index, GLUISizedWindow(size,size,0) );
	}

	void insertStretchSpacer( int index, int proportion ) {
		windows.insert( windows.begin()+index, GLUISizedWindow(0,0,proportion) );
	}

	void detach( GLUIWindow* window ) {
		if( window == NULL ) return;
		for( uint i=0; i < windows.size(); ++i ) {
			if( windows[i].window == window ) {
				windows.erase( windows.begin()+i );
				return;
			}
			if( windows[i].sizer != NULL ) {
				windows[i].sizer->detach(window);
			}
		}
	}

	void detachSizer( GLUISizer* sizer ) {
		if( sizer == NULL ) return;
		for( uint i=0; i < windows.size(); ++i ) {
			if( windows[i].sizer != NULL ) {
				windows[i].sizer->detachSizer(sizer);
			}
			if( windows[i].sizer == sizer ) {
				windows.erase( windows.begin()+i );
				delete sizer;
				return;
			}
		}
	}

	void setDimension( int x, int y, int width, int height ) {
		rect.set(x,y,width,height);
		layout();
	}

	virtual GLUISize calcMinSize(int* totalProportionOut = NULL, int* leftOverSpaceOut = NULL) const =0;

	virtual void layout() = 0;
};


class GLUTILS_DECLSPEC GLUIBoxSizer : public GLUISizer {

	int orientation;

public:
	GLUIBoxSizer(int orientation) :
	  orientation(orientation) {}

	virtual GLUISize calcMinSize(int* totalProportionOut = NULL, int* leftOverSpaceOut = NULL) const {
		
		// Major is width if orientation is HORIZONTAL, or height is orientation is VERTICAL
		int minSizeMajor=0, minSizeMinor=0;
		int totalProportion=0;
		int otherOrientation = GLUIFlipOrientation(orientation);
		for( uint i=0; i < windows.size(); ++i ) {			
			if( windows[i].window != NULL && !windows[i].window->isVisible() )
				continue;

			if( windows[i].proportion != 0 )
				totalProportion += windows[i].proportion;
			else
				minSizeMajor += windows[i].size.get(orientation);
			if( windows[i].window != NULL ) {
				const GLUISize& size = windows[i].window->getMinSize();
				if( windows[i].proportion == 0 )
					minSizeMajor += size.get(orientation);
				minSizeMinor = max( minSizeMinor, size.get(otherOrientation) );
			} else if( windows[i].sizer != NULL ) {
				GLUISize size = windows[i].sizer->calcMinSize();
				if( windows[i].proportion == 0 )
					minSizeMajor += size.get(orientation);
				minSizeMinor = max( minSizeMinor, size.get(otherOrientation) );
			}
		}

		int leftOverSpace = 0;
		if( totalProportion > 0)
			leftOverSpace = rect.getSize( orientation ) - minSizeMajor;

		GLUISize result;
		result.setOrientation( orientation, minSizeMajor );
		result.setOrientation( otherOrientation, minSizeMinor );

		if( totalProportionOut != NULL )
			*totalProportionOut = totalProportion;

		if( leftOverSpaceOut != NULL )
			*leftOverSpaceOut = leftOverSpace;

		return result;	
	}
	
	virtual void layout() {

		int totalProportion;
		int leftOverSpace;
		GLUISize minSize = calcMinSize(&totalProportion, &leftOverSpace);
		int remainingProportion = totalProportion;
		int remainingSpace = leftOverSpace;
		int otherOrientation = GLUIFlipOrientation(orientation);

		GLUIPos pos( rect.x, rect.y );
		for( uint i=0; i < windows.size(); ++i ) {
			if( windows[i].window != NULL && !windows[i].window->isVisible() )
				continue;

			int usedSpace = -1;
			if( windows[i].proportion != 0 ) {
				usedSpace = (windows[i].proportion*leftOverSpace)/totalProportion;
				if( usedSpace > remainingSpace )
					usedSpace = remainingSpace;
				remainingProportion -= windows[i].proportion;
				if( remainingProportion == 0 )
					usedSpace = remainingSpace;
				remainingSpace -= usedSpace;
			}

			GLUISize size;
			if( windows[i].window != NULL )
				size = windows[i].window->getMinSize();
			else if( windows[i].sizer != NULL )
				size = windows[i].sizer->calcMinSize();
			else
				size = windows[i].size;

			if( usedSpace >= 0 )
				size.setOrientation( orientation, max( usedSpace, size.get(orientation) ) );
			if( windows[i].flag == GLUI_EXPAND )
				size.setOrientation( otherOrientation, minSize.get(otherOrientation) );
			if( windows[i].window != NULL ) {
				windows[i].window->setDimension( pos.x, pos.y, size.width, size.height );
			} else if( windows[i].sizer != NULL ) {
				windows[i].sizer->setDimension( pos.x, pos.y, size.width, size.height );
			}
			pos.setOrientation( orientation, pos.get( orientation ) + size.get( orientation ) );
		}
	
	}
};