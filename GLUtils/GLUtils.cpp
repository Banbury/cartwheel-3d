#include ".\glutils.h"
#include <MathLib/Quaternion.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <Utils/Image.h>
#include <Utils/BMPIO.h>

#include <Include/glut.h>

#include "GLTexture.h"

//extern unsigned int fontBaseList;

GLUtils::GLUtils(void)
{
}

GLUtils::~GLUtils(void)
{
}

void GLUtils::gprintf(const char *fmt, ...){
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	int len = (int) strlen(text);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
}



/**
	This method draws a wireframe cube that is defined by the two 3d points
*/
void GLUtils::drawWireFrameBox(Point3d min, Point3d max){
	//now draw the cube that is defined by these two points...
	glBegin(GL_LINES);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);

		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);

		glVertex3d(max.x, max.y, min.z);
		glVertex3d(min.x, max.y, min.z);

		glVertex3d(min.x, max.y, min.z);
		glVertex3d(min.x, min.y, min.z);



		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, max.y, min.z);

		glVertex3d(min.x, max.y, min.z);
		glVertex3d(min.x, max.y, max.z);

		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, min.y, max.z);

		glVertex3d(min.x, min.y, max.z);
		glVertex3d(min.x, min.y, min.z);

		glVertex3d(min.x, min.y, max.z);
		glVertex3d(max.x, min.y, max.z);

		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);

		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, min.y, max.z);

		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);

		glVertex3d(max.x, max.y, min.z);
		glVertex3d(max.x, max.y, max.z);

		glVertex3d(max.x, max.y, max.z);
		glVertex3d(max.x, min.y, max.z);

		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, min.y, min.z);

	glEnd();
}

/**
	This method draws a box cube that is defined by the two 3d points
*/
void GLUtils::drawBox(Point3d min, Point3d max){
	//now draw the cube that is defined by these two points...
	glBegin(GL_QUADS);
		glNormal3d(0, 0, -1);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(min.x, max.y, min.z);

		glNormal3d(0, 0, 1);
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glNormal3d(0, -1, 0);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(min.x, min.y, max.z);

		glNormal3d(0, 1, 0);
		glVertex3d(min.x, max.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glNormal3d(-1, 0, 0);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, max.y, min.z);

		glNormal3d(1, 0, 0);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(max.x, max.y, min.z);

	glEnd();
/*
	// sides
	glBegin (GL_TRIANGLE_STRIP);
	glNormal3d (-1,0,0);
	glVertex3d (min.x,min.y,min.z);
	glVertex3d (min.x,min.y,max.z);
	glVertex3d (min.x,max.y,min.z);
	glVertex3d (min.x,max.y,max.z);
	glNormal3d (0,1,0);
	glVertex3d (max.x,max.y,min.z);
	glVertex3d (max.x,max.y,max.z);
	glNormal3d (1,0,0);
	glVertex3d (max.x,min.y,min.z);
	glVertex3d (max.x,min.y,max.z);
	glNormal3d (0,-1,0);
	glVertex3d (min.x,min.y,min.z);
	glVertex3d (min.x,min.y,max.z);
	glEnd();

	// top face
	glBegin (GL_TRIANGLE_FAN);
	glNormal3d (0,0,1);
	glVertex3d (min.x,min.y,max.z);
	glVertex3d (max.x,min.y,max.z);
	glVertex3d (max.x,max.y,max.z);
	glVertex3d (min.x,max.y,max.z);
	glEnd();

	// bottom face
	glBegin (GL_TRIANGLE_FAN);
	glNormal3d (0,0,-1);
	glVertex3d (min.x,min.y,min.z);
	glVertex3d (min.x,max.y,min.z);
	glVertex3d (max.x,max.y,min.z);
	glVertex3d (max.x,min.y,min.z);
	glEnd();
*/
}


/**
	This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
*/
void GLUtils::drawSphere(Point3d origin, double r, int nrPoints){
	int j;
	double i, angle = PI/nrPoints;
	//this is the normal vector
	Vector3d n, v;

	glPushMatrix();
	glTranslated(origin.x, origin.y, origin.z);

	Point3d p, q;
	
	glBegin(GL_QUAD_STRIP);
		p.x = r*cos(-PI/2);
		p.y = r*sin(-PI/2);
		p.z = 0;
		for (i=-PI/2+angle;i<=PI/2;i+=angle)
		{
			q.x = r*cos(i);
			q.y = r*sin(i);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(p.x,p.y,p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(q.x,q.y,q.z);

			for (j=0;j<=2*nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(q.x * cos(j * angle), q.y, q.x * sin(j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(v.x, v.y, v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(j * angle), p.y, p.x * sin(j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(v.x, v.y, v.z);
			}
			p = q;
		}

	glEnd();
	glPopMatrix();
}



/**
	This method draws a system of coordinate axes of length n
*/
void GLUtils::drawAxes(double n){
	glBegin(GL_LINES);								//draw the axis...
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3d(0,0,0);							//X axis
		glVertex3d(n,0,0);

		glColor3f(0.0f,1.0f,0.0f);
		glVertex3d(0,0,0);							//Y axis
		glVertex3d(0,n,0);

		glColor3f(0.0f,0.0f,1.0f);
		glVertex3d(0,0,0);							//Z axis
		glVertex3d(0,0,n);

	glEnd();
}

/**
	This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
*/
void GLUtils::drawCapsule(double r, Vector3d dir, Point3d org, int nrPoints){
	Vector3d axis = Vector3d(0,1,0);
	//we first need a rotation that gets dir to be aligned with the y-axis...
	Vector3d rotAxis = dir.unit().crossProductWith(axis);
	double rotAngle = asin(rotAxis.length());
	if (dir.dotProductWith(axis) < 0){
		org += dir;
		dir *= -1;
	}
	rotAxis.toUnit();

	Quaternion rot = Quaternion::getRotationQuaternion(-rotAngle, rotAxis);
	glPushMatrix();
	glTranslated(org.x, org.y, org.z);
	double rotData[16];
	TransformationMatrix rotM;
	rot.getRotationMatrix(&rotM);
	rotM.getOGLValues(rotData);
	glMultMatrixd(rotData);

	org.x = 0;
	org.y = 0;
	org.z = 0;


	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;


	//try to get a vector that is perpendicular to r.
	n = Vector3d(1,0,0);
	dir = axis * dir.length();

	(n.toUnit()) *= r;
	glBegin(GL_TRIANGLE_STRIP);

	//now, we we'll procede by rotating the vector n around v, and create the cylinder that way.
	for (int i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
		Point3d p2 = org + dir + p;
		glVertex3d(p2.x, p2.y, p2.z);
	}
	glEnd();

	//now we'll draw a half sphere...
	Point3d p, q;
	double angle = PI/nrPoints;
		p.x = r*cos(-PI/2);
		p.y = r*sin(-PI/2);
		p.z = 0;
		for (int i=nrPoints/2;i>=0;i--){
			glBegin(GL_QUAD_STRIP);
			q.x = r*cos(-i*angle);
			q.y = r*sin(-i*angle);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x+p.x,org.y + p.y, org.z + p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x + q.x,org.y + q.y,org.z + q.z);

			for (int j=0;j<=nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				Vector3d v = Vector3d(q.x * cos(2*j * angle), q.y, q.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(2*j * angle), p.y, p.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);
			}
			p = q;
			glEnd();
		}	

	org += dir;

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//and now draw the other half - horrible, quick research code...
	
		p.x = r*cos(PI/2);
		p.y = r*sin(PI/2);
		p.z = 0;
		for (int i=nrPoints/2;i>=0;i--){
			glBegin(GL_QUAD_STRIP);
			q.x = r*cos(i*angle);
			q.y = r*sin(i*angle);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x+p.x,org.y + p.y, org.z + p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(org.x + q.x,org.y + q.y,org.z + q.z);

			for (int j=0;j<=nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				Vector3d v = Vector3d(q.x * cos(2*j * angle), q.y, q.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(2*j * angle), p.y, p.x * sin(2*j * angle));
				n = v.unit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(org.x+v.x, org.y+v.y, org.z+v.z);
			}
			p = q;
			glEnd();
		}
	

	glPopMatrix();

}


/**
	This method draws a disc of radius r, centered on point org with normal norm
*/
void GLUtils::drawDisk(double r, Point3d org, Vector3d norm, int nrPoints) {
	Vector3d n = norm;
	n.toUnit();
	Vector3d x = Vector3d(0,1,0).crossProductWith(n);
	Vector3d y = Vector3d(1,0,0).crossProductWith(n);
	if( x.length() < 0.01 )
		x = Vector3d(0,0,1).crossProductWith(n);
	else if( y.length() < 0.01 )
		y = Vector3d(0,0,1).crossProductWith(n);
	x.toUnit();
	y.toUnit();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d( n.x, n.y, n.z );
	glVertex3d( org.x, org.y, org.z );
	for( int i=0; i<=nrPoints; ++i ) {
		double theta = (i%nrPoints) / (double)nrPoints * 2.0 * 3.14159265;
		Point3d p = org + x * cos(theta) * r + y * sin(theta) * r;
		glVertex3d( p.x, p.y, p.z );
	}
	glEnd();
}


/**
	This method draws a cylinder of thinkness r, along the vector dir.
*/
void GLUtils::drawCylinder(double r, Vector3d v, Point3d org, int nrPoints){
	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;
	Vector3d axis = v;
	axis.toUnit();
	int i;
	//try to get a vector that is not colinear to v.
	if (v.x != 0 || v.y != 0)
		n = Vector3d(v.x, v.y, v.z + 1);
	else if (v.y != 0 || v.z != 0)
		n = Vector3d(v.x, v.y+1, v.z);
	else
		n = Vector3d(v.x+1, v.y, v.z);

	n = n.crossProductWith(v);

	if (IS_ZERO(v.length()) || IS_ZERO(n.length()))
		return;
	(n.toUnit()) *= r;
	glBegin(GL_TRIANGLE_STRIP);

	//now, we we'll procede by rotating the vector n around v, and create the cylinder that way.
	for (i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
		Point3d p2 = org + v + p;
		glVertex3d(p2.x, p2.y, p2.z);
	}

	glEnd();

}


/**
	This method draws a cone of radius r, along the vector dir, with the center of its base at org.
*/
void GLUtils::drawCone(double r, Vector3d v, Point3d org, int nrPoints){
	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;
	Vector3d axis = v;
	axis.toUnit();
	int i;
	//try to get a vector that is not colinear to v.
	if (v.x != 0 || v.y != 0)
		n = Vector3d(v.x, v.y, v.z + 1);
	else if (v.y != 0 || v.z != 0)
		n = Vector3d(v.x, v.y+1, v.z);
	else
		n = Vector3d(v.x+1, v.y, v.z);

	n = n.crossProductWith(v);

	if (IS_ZERO(v.length()) || IS_ZERO(n.length()))
		return;
	(n.toUnit()) *= r;
	glBegin(GL_TRIANGLE_FAN);

	Point3d p2 = org + v;
	glNormal3d(axis.x, axis.y, axis.z);
	glVertex3d(p2.x, p2.y, p2.z);


	//now, we we'll procede by rotating the vector n around v, and creating the cone that way.
	for (i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
	}
	glEnd();


	//now we need to draw the bottom of the cone.
	glBegin(GL_POLYGON);

	//now, we we'll procede by rotating the vector n around v, and creating the cone that way.
	for (i=0;i<=nrPoints;i++){
		Vector3d p = n.rotate(2*i*PI/nrPoints, axis);
		Vector3d normal = p.unit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
	}
	glEnd();
}

/**
	This method is used to draw an arrow, in the direction pointed by the vector dir, originating at the point org.
	The thickness of the cylinder used, as well as the length of the arrow head are estimated based on the length of the direction vector.
*/
void GLUtils::drawArrow(Vector3d dir, Point3d org){
	drawCylinder(dir.length()/20, dir*0.8, org);
	drawCone(dir.length()/10, dir/4, org+dir*0.75);
}


/**
	This method is used to draw an arrow, in the direction pointed by the vector dir, originating at the point org.
	The thickness of the cylinder used, as well as the length of the arrow head are estimated based on the length of the direction vector.
*/
void GLUtils::drawArrow(Vector3d dir, Point3d org, double scale){
	drawCylinder(scale, dir*0.8, org);
	drawCone(2*scale, dir/(dir.length())*scale*5, org+dir*0.75);
}


/**
	This method will take a screenshot of the current scene and it will save it to a file with the given name
*/
void GLUtils::saveScreenShot(char* fileName, int x, int y, int width, int height){
	if (fileName == NULL)
		return;
	std::ofstream out(fileName, std::ofstream::binary);
	if(!out)
		return;

	glReadBuffer(GL_BACK);

	Image *img = new Image(3, width, height, NULL);

	glReadPixels(x , y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img->getDataPointer());

	BMPIO b(fileName);
	b.writeToFile(img);

	delete img;
}


/**
	This method draws a checker-board pattern (centered at the origin, parallel to the XZ plane) based on the current parameters:
	int n - the number of squares (we'll assume square checkerboards for now!)
	double w - the size of each square int the checkerboard
	double h - the height at which the checkerboard is to be drawn
*/
void GLUtils::drawCheckerboard(int n, double w, double h){
#define BRIGHT 0.5f
#define DARK 0.2f
	double start = n/2.0 * w;
	for (int i=0;i<n;i++){
		for (int j=0;j<n;j++){
			if ((i+j)%2 == 1)
				glColor3d(BRIGHT, BRIGHT, BRIGHT);
			else
				glColor3d(DARK, DARK, DARK);

			glBegin(GL_QUADS);
				glVertex3d(-start+i*w,h,-start+j*w);
				glVertex3d(-start+i*w+w,h,-start+j*w);
				glVertex3d(-start+i*w+w,h,-start+j*w+w);
				glVertex3d(-start+i*w,h,-start+j*w+w);
			glEnd();
		}
	}
}


/**
	This method draws a grid pattern (centered at the origin, parallel to the XZ plane) based on the current parameters:
	int n - the number of squares (we'll assume square grid for now!)
	double w - the size of each square in the grid
	double h - the height at which the grid is to be drawn
*/
void GLUtils::drawGrid(int n, double w, double h){
	glColor3d(0.5f, 0.5f, 0.5f);
	double start = n/2.0 * w;
	glLineWidth(0.5);
	for (int i=1;i<n;i++){
		glBegin(GL_LINES);
			glVertex3d(-start+i*w,h,-start);
			glVertex3d(-start+i*w,h,start);

			glVertex3d(-start,h,-start+i*w);
			glVertex3d(start, h,-start+i*w);
		glEnd();
	}
	glLineWidth(1);

//draw a thicker line in the middle now, 
	glColor3d(0.7f, 0.7f, 0.7f);
	glLineWidth(2);
	glBegin(GL_LINES);
		glVertex3d(0,h,-start);
		glVertex3d(0,h,start);
		glVertex3d(-start,h,0);
		glVertex3d(start, h,0);
	glEnd();
	glLineWidth(1);

}


/**
	Prints the openGL errors
*/
int GLUtils::printOglError(char *file, int line){
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR){
        tprintf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void applyColor(double x, double z, double spotRadius) {

	glColor3d(1, 1, 1);
	double fadeOff = 20;

	double dist2 = x*x + z*z;
	if( dist2 > spotRadius*spotRadius ) {
		double diff = sqrt(dist2) - spotRadius;
		double color = (fadeOff-diff)/fadeOff;
		glColor3d( color, color, color );
	}
}

GLTexture* groundTexture = NULL;

void GLUtils::drawGround(double size, double spotRadius, int nb){

	glDisable(GL_LIGHTING);
	glColor3d(1, 1, 1);

	double x, z, x1, z1;

	if (groundTexture == NULL )
		groundTexture = new GLTexture("../data/textures/grid.bmp");

	glEnable(GL_TEXTURE_2D);
	groundTexture->activate();
	glBegin(GL_QUADS);

	double smallSize = 2 * size / (float)nb;
	for (int i=0; i<nb; ++i) {
		x1 = -size + i * smallSize;
		for (int j=0; j<nb; ++j) {
			z1 = -size + j * smallSize;
			x = x1; z = z1;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/2, z/2);
			glVertex3d(x, 0,z);
			z = z1+smallSize;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/2, z/2);
			glVertex3d(x, 0,z);
			x = x1+smallSize;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/2, z/2);
			glVertex3d(x, 0,z);
			z = z1;
			applyColor(x,z,spotRadius);
			glTexCoord2d(x/2, z/2);
			glVertex3d(x, 0,z);
		}
	}
	glEnd();


    glDisable(GL_TEXTURE_2D);
}


