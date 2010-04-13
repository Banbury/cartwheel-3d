#pragma once

#include <GLUtils/GLUtilsDll.h>

#include <GLUtils/GLUIWindow.h>
#include <GLUtils/GLUISizer.h>
#include <algorithm>


class GLUTILS_DECLSPEC GLUIContainer : public GLUIWindow {
friend class GLUIWindow;
private:
	void addChild( GLUIWindow* child ) {
		children.push_back( child );
	}


protected:

	DynamicArray<GLUIWindow*> children;

	virtual bool onMouseEventHandler( MouseEventHandler handler, GLUIMouseEvent* mouseEvent ) { 
		
		// Change mouseEvent to relative position
		for( uint i=0; i<children.size(); ++i ) {
			if( !children[i]->isVisible() ) continue;
			if( children[i]->pointInside(mouseEvent->x,mouseEvent->y) ) {
				if( children[i]->hasContent() )
					mouseEvent->skip = false; // The mouse is over a window with content, event has been processed!
				GLUIPos pos = children[i]->getPosition();
				mouseEvent->x -= pos.x;
				mouseEvent->y -= pos.y;
				bool result = ((*children[i]).*handler)(mouseEvent);
				mouseEvent->x += pos.x;
				mouseEvent->y += pos.y;
				return result;
			}
		}
		return false;
	}

public:

	GLUIContainer(GLUIContainer* parent, int x=0, int y=0, int width=0, int height=0, int minWidth=-1, int minHeight=-1 ) : 
		GLUIWindow(parent,x,y,width,height,minWidth) {
		setHasContent(false);
	}
	
	virtual ~GLUIContainer() {
		detachAllChildren();
	}

	virtual void refresh() {
		for( uint i=0; i<children.size(); ++i )
			if( children[i]->isVisible() )
				children[i]->refresh();
	}

	void detachAllChildren() {
		for( uint i=0; i<children.size(); ++i ) {
			children[i]->parent = NULL;
			if( sizer != NULL )
				sizer->detach( children[i] );
		}
		children.clear();
	}

	void detachChild( GLUIWindow* child ) {
		children.erase( std::remove( children.begin(), children.end(), child ), children.end() );
		child->parent = NULL;
		if( sizer != NULL )
			sizer->detach( child );
	}
};