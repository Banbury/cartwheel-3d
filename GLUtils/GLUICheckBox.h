#pragma once

#include <GLUtils/GLUtilsDll.h>

#include <GLUtils/GLUIWindow.h>
#include <GLUtils/GLUICallback.h>
#include <MathLib/Trajectory.h>


/**
  * This class is used to draw a simple checkbox
  */
class GLUTILS_DECLSPEC GLUICheckBox : public GLUIWindow {

private:
	bool checked;

	GLUICallback* checkBoxCallback;

	void executeCheckBoxCallback() const { if ( checkBoxCallback == NULL ) return; checkBoxCallback->execute(); }

public:

	/**
		Default constructor
	*/
	GLUICheckBox(GLUIContainer* parent, int x=0, int y=0, int width=0, int height=0, int minWidth=-1, int minHeight=-1, bool checked = false );

	void draw();

	bool onLeftDown( GLUIMouseEvent* mouseEvent );

	void setChecked( bool checked ) { if( this->checked == checked ) return; this->checked = checked; executeCheckBoxCallback(); }

	bool isChecked() const { return checked; }

	void setCheckBoxCallback( GLUICallback* checkBoxCallback ) { this->checkBoxCallback = checkBoxCallback; }
};