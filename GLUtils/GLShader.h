#pragma once

#include <Include/glHeaders.h>

#include <Utils/Utils.h>

#include <GLUtils/GLUtilsDll.h>

/*------------------------------------------------------------------------------------------------------------------------*
 * Instances of this class will be used to easily use shaders written in the openGL Shading Language.                     *
 * For now, this class only supports one vertex shader and/or one fragment shader. At some point in the future, if need   *
 * be, this can be extended for multiple types of either shaders.                                                         *
 *------------------------------------------------------------------------------------------------------------------------*/

class GLUTILS_DECLSPEC GLShader{
private:
/**
	Returns the size in bytes of the shader fileName.
	If an error occurred, it returns -1.
*/
int shaderSize(char *VFileName);

/**
	Reads a shader from the supplied file and returns the shader in the
	arrays passed in. Returns 1 if successful, -1 if an error occurred.
	The parameter size is an upper limit of the amount of bytes to read.
	It is ok for it to be too big.
*/
int readShader(char *fileName, char *shaderText, int size);

/**
	This method reads the contenent of a file in the array of characters that it allocates for it, and returns 1 if mission succesful, false otherwise
*/
int readShaderSource(char *fileName, char **buffer);

//keep a copy of the program that we create, as well as the vertex and fragment shaders
uint programObject;
uint vertexShaderObject;
uint fragmentShaderObject;

public:

	/**
		The constructor takes as input the name of a shader file that contains the texture to be loaded.
		This constructor throws errors if the file cannot be found, or if it's height and width are not powers of 2
	*/
	GLShader(char *vertexShaderFileName, char* fragmentShaderFileName);

	/**
		destructor
	*/
	~GLShader(void);

	/**
		this method sets the current shader as the active
	*/
	void activate();

	/**
		this method makes sure that the current shader is no longer used (instead, the fixed functionality of OpenGL will be active).
	*/
	void deactivate();
};
