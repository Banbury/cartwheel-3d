#pragma once

#include <MathLib/Point3d.h>
#include <MathLib/Vector3d.h>
#include <Include/glHeaders.h>
#include <Utils/Utils.h>

#define printOpenGLError() GLUtils::printOglError(__FILE__, __LINE__)

#include <GLUtils/GLUtilsDll.h>

/*=======================================================================================================================================================================*
 | This class implements a set of general, useful methods that have to do with OpenGL (such as drawing a sphere).                                                        |
 *=======================================================================================================================================================================*/
class GLUTILS_DECLSPEC GLUtils{
public:
	GLUtils(void);
	~GLUtils(void);

	//print in an openGL window. The raster position needs to be already defined.
	static void gprintf(const char *format, ...);

	/**
		This method draws a wireframe cube that is defined by the two 3d points
	*/
	static void drawWireFrameBox(Point3d min, Point3d max);

	/**
		This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
	*/
	static void drawSphere(Point3d origin, double r, int nrPoints = 11);


	/**
		This method draws a system of coordinate axes of length n
	*/
	static void drawAxes(double n);

	/**
		This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
	*/
	static void drawCapsule(double r, Vector3d v, Point3d org, int nrPoints);

	/**
		This method draws a disc of radius r, centered on point org with normal norm
	*/
	static void drawDisk(double r, Point3d org, Vector3d norm, int nrPoints = 16);


	/**
		This method draws a cylinder of thinkness r, along the vector v, starting from the point org.
	*/
	static void drawCylinder(double r, Vector3d v, Point3d org, int nrPoints = 6);

	/**
		This method draws a cone of radius r, along the vector v, with the center of its base at org.
	*/
	static void drawCone(double r, Vector3d v, Point3d org, int nrPoints = 6);

	/**
		This method is used to draw an arrow, in the direction pointed by the vector dir, originating at the point org.
		The thickness of the cylinder used, as well as the length of the arrow head are estimated based on the length of the direction vector.
	*/
	static void drawArrow(Vector3d dir, Point3d org);

	/**
		This method is used to draw an arrow, in the direction pointed by the vector dir, originating at the point org.
		The thickness of the cylinder used, as well as the length of the arrow head are estimated based on the length of the direction vector.
	*/
	static void drawArrow(Vector3d dir, Point3d org, double scale);

	/**
		This method will take a screenshot of the current scene and it will save it to a file with the given name
	*/
	static void saveScreenShot(char* fileName, int x, int y, int width, int height);


	/**
		This method draws a checker-board pattern (centered at the origin, parallel to the XZ plane) based on the current parameters:
		int n - the number of squares (we'll assume square checkerboards for now!)
		double w - the size of each square int the checkerboard
		double h - the height at which the checkerboard is to be drawn
	*/
	static void drawCheckerboard(int n, double w, double h);
	/**
		This method draws a box cube that is defined by the two 3d points
	*/
	static void drawBox(Point3d min, Point3d max);

	/**
		This method draws a grid pattern (centered at the origin, parallel to the XZ plane) based on the current parameters:
		int n - the number of squares (we'll assume square grid for now!)
		double w - the size of each square in the grid
		double h - the height at which the grid is to be drawn
	*/
	static void drawGrid(int n, double w, double h);

	/**
		Prints the openGL errors
	*/
	static int printOglError(char *file, int line);

	static void drawGround(double size, double spotRadius, int nbSquares);

};


