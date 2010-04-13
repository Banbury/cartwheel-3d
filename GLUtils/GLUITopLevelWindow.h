#pragma once

#include <GLUtils/GLUtilsDll.h>

#include <GLUtils/GLUIContainer.h>


class GLUTILS_DECLSPEC GLUITopLevelWindow : public GLUIContainer {

private:
	DynamicArray<GLUIWindow*> captureStack;

protected:

	virtual bool onMouseEventHandler( MouseEventHandler handler, GLUIMouseEvent* mouseEvent ) { 
		
		if( !captureStack.empty() ) {
			mouseEvent->skip = false; // Do not skip, somebody is handling this
			// Adjust mouse position
			int deltaX = 0;
			int deltaY = 0;
			GLUIWindow* capturingWindow = captureStack.back();
			GLUIWindow* currWindow = capturingWindow;
			while( currWindow != NULL ) {
				GLUIPos pos = currWindow->getPosition();
				deltaX += pos.x;
				deltaY += pos.y;
				currWindow = currWindow->getParent();
			}
			mouseEvent->x -= deltaX;
			mouseEvent->y -= deltaY;
			bool result = ((*capturingWindow).*handler)(mouseEvent);
			mouseEvent->x += deltaX;
			mouseEvent->y += deltaY;
			return result;
		}
		// For now skip has the event has not been processed. It will be processed if its over a non-empty window.
		mouseEvent->skip = true; 
		
		// Remove the top-level window position from the mouse event
		GLUIPos pos = getPosition();
		mouseEvent->x -= pos.x;
		mouseEvent->y -= pos.y;
		bool result = GLUIContainer::onMouseEventHandler( handler, mouseEvent );
		mouseEvent->x += pos.x;
		mouseEvent->y += pos.y;
		return result;
	}

public:

	GLUITopLevelWindow(int width=0, int height=0 ) :
	   GLUIContainer(NULL,0,0,width,height) {}

	virtual void captureMouse(GLUIWindow* window=NULL) {
		// Never accept to capture on onself, it would lead to an infinite loop
		if(window == this) return;
		if( captureStack.empty() )
			startMouseCapture();		
		captureStack.push_back(window);
	}

	virtual void releaseMouse(GLUIWindow* window=NULL) {
		// We never accepted to capture on ourself, so dont release
		if(window == this) return;
		if( captureStack.empty() )
			return;
		for( uint i=captureStack.size(); i>0; --i )
			if( captureStack[i-1] == window ) {
				captureStack.erase( captureStack.begin()+i-1 );
				break;
			}
		if( captureStack.empty() )
			stopMouseCapture();		
	}
	bool mouseIsCaptured() const { return !captureStack.empty(); }

	bool hasCapture(GLUIWindow* window = NULL) { return !captureStack.empty() && captureStack.back() == window; }

	// Scrip-side, these methods should be overriden to propagate mouse capture to the external window management system
	virtual void startMouseCapture() {}
	virtual void stopMouseCapture() {}

};