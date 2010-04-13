#pragma once

#include <GLUtils/GLUtilsDll.h>
#include <GLUtils/GLUI.h>


class GLUIContainer;
class GLUISizer;
class GLUTILS_DECLSPEC GLUIWindow {
friend class GLUIContainer;

public:
	typedef bool (GLUIWindow::*MouseEventHandler)(GLUIMouseEvent*);

private:
	bool beginDraw(int offsetX=0, int offsetY=0, int width=0, int height=0, int minWidth=-1, int minHeight=-1) const;
	void endDraw() const;

protected:
	
	GLUIContainer* parent;

	GLUIRect rect;

	GLUISize minSize;

	unsigned long flags;

	GLUIColor bgColor;

	GLUISizer* sizer;

	bool hasContent() const { return (flags & GLUI_HAS_CONTENT)?true:false; }
	void setHasContent(bool hasContent = true) { 
		if(hasContent) flags |= GLUI_HAS_CONTENT; 
		else           flags &= ~GLUI_HAS_CONTENT; 
	}

	virtual bool onMouseEventHandler( MouseEventHandler handler, GLUIMouseEvent* mouseEvent ) { return false; }

public:

	GLUIWindow(GLUIContainer* parent, int x=0, int y=0, int width=0, int height=0, int minWidth = -1, int minHeight = -1 );

	virtual ~GLUIWindow();

	void setSizer( GLUISizer* sizer_disown );

	GLUISizer* getSizer() const { return sizer; }

	GLUIContainer* getParent() { return parent; }

	virtual void refresh() {
		if( !hasContent() ) return;
		if( !beginDraw() ) return;
		draw();
		endDraw();
	}

	void setPosition( int x, int y ) {
		rect.x = x;
		rect.y = y;
	}

	void setSize( int width, int height );

	void setDimension( int x, int y, int width, int height );

	GLUIPos getPosition() const { return GLUIPos( rect.x, rect.y ); }
	GLUISize getSize() const { return GLUISize( rect.width, rect.height ); }

	void setMinSize( int width, int height ) {
		minSize.width  = width;
		minSize.height = height;
	}

	virtual GLUISize getMinSize() const;
	
	void layout();

	bool isVisible() const { return (flags & GLUI_VISIBLE)?true:false; }

	void setVisible(bool visible = true);

	bool pointInside( int x, int y ) { return rect.pointInside(x,y); }



	virtual void captureMouse(GLUIWindow* window = NULL);
	virtual void releaseMouse(GLUIWindow* window = NULL);
	virtual bool hasCapture(GLUIWindow* window = NULL);

	// All supported mouse callbacks
	virtual bool onLeftDown( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onLeftDown,mouseEvent); }
	virtual bool onLeftUp( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onLeftUp,mouseEvent); }
	virtual bool onLeftDClick( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onLeftDClick,mouseEvent); }
	virtual bool onMiddleDown( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onMiddleDown,mouseEvent); }
	virtual bool onMiddleUp( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onMiddleUp,mouseEvent); }
	virtual bool onMiddleDClick( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onMiddleDClick,mouseEvent); }
	virtual bool onRightDown( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onRightDown,mouseEvent); }
	virtual bool onRightUp( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onRightUp,mouseEvent); }
	virtual bool onRightDClick( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onRightDClick,mouseEvent); }
	virtual bool onMotion( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onMotion,mouseEvent); }
	virtual bool onMouseWheel( GLUIMouseEvent* mouseEvent ) { return onMouseEventHandler(&GLUIWindow::onMouseWheel,mouseEvent); }

	virtual void draw() {}

};
GLUTILS_TEMPLATE( DynamicArray<GLUIWindow*> )
