#pragma once

#include <GLUtils/GLUtilsDll.h>

#include <GLUtils/GlTexture.h>

/*---------------------------------------------------------------------------------------------------------------------------*
 *	This class is used to store and draw the six faces of a cube that represents the background of a scene in a 3d setting.  *
 *  aka Sky Box.
 *---------------------------------------------------------------------------------------------------------------------------*/

class GLUTILS_DECLSPEC CubeBg{
private:
	GLTexture *faceTex[6];
public:
	/**
		This is the constructor. The file name that is passed in corresponds to the root name of the six texture files
		that will be loaded for this background. The file names can be obtained by adding: +x.bmp, -x.bmp, +y.bmp, etc
		to the root file name.
	*/
	CubeBg(char* rootFileName);
	/**
		Destructor
	*/
	~CubeBg(void);

	/**
		This is the method that needs to be used to draw the sky box on the screen. The OpenGL matrices should be manipulated
		in order to place the sky box where desirable. 
	*/
	void drawBox();
};
