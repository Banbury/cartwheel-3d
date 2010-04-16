#include <include/glew.h>
#include "GLShader.h"
#include <GLUtils/GLUtils.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>

/**
  Print out the information log for a shader object
*/
void printShaderInfoLog(GLuint shader){
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    printOpenGLError();  // Check for OpenGL errors

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

    printOpenGLError();  // Check for OpenGL errors

    if (infologLength > 0){
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
            throwError("ERROR: Could not allocate InfoLog buffer\n");
        glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog)>0)
			tprintf("Shader InfoLog: %s\n\n", infoLog);
        free(infoLog);
    }
    printOpenGLError();  // Check for OpenGL errors
}

/*
	Print out the information log for a program object
*/
void printProgramInfoLog(GLuint program){
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    printOpenGLError();  // Check for OpenGL errors

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

    printOpenGLError();  // Check for OpenGL errors

    if (infologLength > 0){
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
			throwError("ERROR: Could not allocate InfoLog buffer\n");

		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog)>0)
	        tprintf("Program InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    printOpenGLError();  // Check for OpenGL errors
}


GLShader::GLShader(char *vertexShaderFileName, char* fragmentShaderFileName){
	char* vertexShader = NULL;
	char* fragmentShader = NULL;

	if (vertexShaderFileName)
		readShaderSource(vertexShaderFileName, &vertexShader);
	if (fragmentShaderFileName)
		readShaderSource(fragmentShaderFileName, &fragmentShader);

    int  vertCompiled, fragCompiled;    // status values
    int  linked;

	vertexShaderObject = 0;
	fragmentShaderObject = 0;

    // Create a vertex shader object and a fragment shader object
	if (vertexShader)
		vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	if (fragmentShader)
		fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // Load source code strings into shaders
	if (vertexShader){
		glShaderSource(vertexShaderObject, 1, (const char**)&vertexShader, NULL);
	    // Compile the vertex shader, and print out the compiler log file.
		glCompileShader(vertexShaderObject);
		printOpenGLError();  // Check for OpenGL errors
		glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &vertCompiled);
		printShaderInfoLog(vertexShaderObject);
	}
	if (fragmentShader){
		glShaderSource(fragmentShaderObject, 1, (const char**)&fragmentShader, NULL);
    // Compile the fragment shader, and print out the compiler log file.
	    glCompileShader(fragmentShaderObject);
		printOpenGLError();  // Check for OpenGL errors
	    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &fragCompiled);
		printShaderInfoLog(fragmentShaderObject);
	}

	if (!vertCompiled || !fragCompiled)
		throwError("Could not compile fragment and/or vertex shader");

    // Create a program object and attach the two compiled shaders
    programObject = glCreateProgram();
	if (vertexShader)
		glAttachShader(programObject, vertexShaderObject);
	if (fragmentShader)
		glAttachShader(programObject, fragmentShaderObject);

    // Link the program object and print out the info log
    glLinkProgram(programObject);
	printOpenGLError();  // Check for OpenGL errors
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	printProgramInfoLog(programObject);

	if (!linked)
		throwError("Could not link program object");

	//release the memory that we had allocated for the contents of the files
	free(vertexShader);
	free(fragmentShader);
}

GLShader::~GLShader(void){
	if (vertexShaderObject!=0)
		glDetachShader(programObject, vertexShaderObject);
	if (fragmentShaderObject!=0)
		glDetachShader(programObject, fragmentShaderObject);
	glDeleteProgram(programObject);
}

/**
	Returns the size in bytes of the shader fileName.
	If an error occurred, it returns -1.
*/
int GLShader::shaderSize(char *fileName){
    int fd;
    int count = -1;

    //
    // Open the file, seek to the end to find its length
    //
    fd = _open(fileName, _O_RDONLY);
    if (fd != -1) {
        count = _lseek(fd, 0, SEEK_END) + 1;
        _close(fd);
    }

    return count;
}


/**
	Reads a shader from the supplied file and returns the shader in the
	arrays passed in. Returns 1 if successful, -1 if an error occurred.
	The parameter size is an upper limit of the amount of bytes to read.
	It is ok for it to be too big.
*/
int GLShader::readShader(char *fileName, char *shaderText, int size){
    FILE *fh;
    int count;

    fh = fopen(fileName, "r");
    if (!fh)
        return -1;

    fseek(fh, 0, SEEK_SET);
    count = (int) fread(shaderText, 1, size, fh);
    shaderText[count] = '\0';

    if (ferror(fh))
        count = -1;

    fclose(fh);
    return count;
}

/**
	This method reads the contenent of a file in the array of characters that it allocates for it
*/
int GLShader::readShaderSource(char *fileName, char **buffer){
    int size;

    // Allocate memory to hold the source of our shaders.
    size = shaderSize(fileName);

    if (size == -1){
        tprintf("Cannot determine size of the shader %s\n", fileName);
        return 0;
    }

    *buffer = (char *) malloc(size);

    if (!readShader(fileName, *buffer, size)){
        tprintf("Cannot read the file %s.vert\n", fileName);
        return 0;
    }

    return 1;
}

/**
	this method sets the current shader as the active
*/
void GLShader::activate(){
    //Install program object as part of current state
	glUseProgram(programObject);
}

/**
	this method makes sure that the current shader is no longer used (instead, the fixed functionality of OpenGL will be active).
*/
void GLShader::deactivate(){
	glUseProgram(0);
}

