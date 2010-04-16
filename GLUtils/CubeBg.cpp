#include ".\cubebg.h"
#include <stdio.h>
#include <MathLib/Point3d.h>

/**
	This is the constructor. The file name that is passed in corresponds to the root name of the six texture files
	that will be loaded for this background. The file names can be obtained by adding: +x.bmp, -x.bmp, +y.bmp, etc
	to the root file name.
*/
CubeBg::CubeBg(char* rootFileName){
	char *ext[] = {"+x","-x","+y","-y","+z","-z"};
	char fName[200];
	for (int i=0;i<6;i++){
		sprintf(fName, "%s%s.bmp", rootFileName, ext[i]);
		this->faceTex[i] = new GLTexture(fName);
	}
}

/**
	Destructor.
*/
CubeBg::~CubeBg(void){
	for (int i=0;i<6;i++){
		delete this->faceTex[i];
	}
}

/**
	This method draws a quad (No Lighting, but uses texture coordinates). This quad is always alligned with the xy plane
	drawn at z=0.
*/
void drawTexturedQuad(){
	double d = 1.005;

	glBegin(GL_QUADS);
		glTexCoord2d(0,0);
		glVertex3d(-d,-d,0);
		glTexCoord2d(0,1);
		glVertex3d(-d,d,0);
		glTexCoord2d(1,1);
		glVertex3d(d,d,0);
		glTexCoord2d(1,0);
		glVertex3d(d,-d,0);
	glEnd();
}


/**
	This is the method that needs to be used to draw the sky box on the screen. The OpenGL matrices should be manipulated
	in order to place the sky box where desirable. 
*/
void CubeBg::drawBox(){
	//draw the +x plane:
	glPushMatrix();
	glTranslated(1,0,0);
	glRotated(90,0,1,0);
	this->faceTex[0]->activate();
	drawTexturedQuad();
	glPopMatrix();

	//draw the -x plane:
	glPushMatrix();
	glTranslated(-1,0,0);
	glRotated(-90,0,1,0);
	this->faceTex[1]->activate();
	drawTexturedQuad();
	glPopMatrix();

	//draw the +y plane:
	glPushMatrix();
	glTranslated(0,1,0);
	glRotated(-90,1,0,0);
	this->faceTex[2]->activate();
	drawTexturedQuad();
	glPopMatrix();

	//draw the -y plane:
	glPushMatrix();
	glTranslated(0,-1,0);
	glRotated(90,1,0,0);
	this->faceTex[3]->activate();
	drawTexturedQuad();
	glPopMatrix();

	//draw the +z plane:
	glPushMatrix();
	glTranslated(0,0,1);
	this->faceTex[4]->activate();
	drawTexturedQuad();
	glPopMatrix();

	//draw the -z plane:
	glPushMatrix();
	glTranslated(0,0,-1);
	glRotated(180,0,1,0);
	this->faceTex[5]->activate();
	drawTexturedQuad();
	glPopMatrix();

}

