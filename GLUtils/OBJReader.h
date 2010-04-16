#pragma once

#include <GLUtils/GLMesh.h>

#include <GLUtils/GLUtilsDll.h>

/*======================================================================================================================================================================*
 | This class implements the routines that are needed to load a mesh from an OBJ file. This class loads a GLMesh with the polygonal mesh that is stored in a file.      |
 | Only vertex coordinates, vertex texture coordinates and connectivity information are loaded from the file.                                                           |
 *======================================================================================================================================================================*/
class GLUTILS_DECLSPEC OBJReader{
public:
	OBJReader(void);
	~OBJReader(void);

	/**
		This static method reads an obj file, whose name is sent in as a parameter, and returns a pointer to a GLMesh object that it created based on the file information.
		This method throws errors if the file doesn't exist, is not an obj file, etc.
	*/
	static GLMesh* loadOBJFile(const char* fileName);
};
