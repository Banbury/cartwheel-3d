#include ".\objreader.h"
#include <MathLib/Point3d.h>
#include <Utils/Utils.h>


OBJReader::OBJReader(void)
{
}

OBJReader::~OBJReader(void)
{
}

#define VERTEX_INFO 0
#define FACE_INFO 1
#define TEXTURE_INFO 2
#define NOT_IMPORTANT 4



/**
	This method checks a line of input from an ASCII OBJ file, and determines its type.
*/
int getLineType(char* line){
	if (line == NULL || strlen(line)<3)
		return NOT_IMPORTANT;
	if (line[0] == 'v' && line[1] == ' ')
		return VERTEX_INFO;
	if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
		return TEXTURE_INFO;
	if (line[0] == 'f' && line[1] == ' ')
		return FACE_INFO;
	return NOT_IMPORTANT;
}



/**
	This method reads three double values separated by white space and returns a point3d populated with the information. It is assumed that the values
	are at the very beginning of the character array.
*/
Point3d readCoordinates(char* line){
	Point3d p;
	int n = sscanf(line, "%lf %lf %lf", &p.x, &p.y, &p.z);
	return p;
}



#define READ_VERTEX_INDEX 0x01
#define READ_TEXTCOORD_INDEX 0x02



/**
	This method takes a character array and does the following:
	if a number is encountered, that number is read - this will be the vertex index. If a '/' is encountered then the number after it is also read (if any).
	The method then returns the pointer to the first white space after the last / encountered from the beginning of the character array. If there were
	no valid indices to read, this method returns NULL. The flag that is passed in is set to reflect which indices were read.
*/
char* getNextIndex(char* line, int &vertexIndex, int &texcoordIndex, int&flags){
	flags = 0;
	//skip the white spaces...
	while (*line == ' ')
		line++;
	char* result = line;
	if (sscanf(line, "%d",&vertexIndex)!=1)
		return NULL;
	flags |= READ_VERTEX_INDEX;
	//check for the '/'
	while (*result != '\0' && *result != ' ' && *result != '/')
		result++;
	//did we find a '/'?
	if (*result == '/')
//		if (sscanf(result+1, "%d",&texcoordIndex)==1)
//			flags |= READ_TEXTCOORD_INDEX;
	//and now move result to the next available index to be read.
	while (*result!='\0' && *result!=' ')
		result++;

	return result;
}


/**
	This static method reads an obj file, whose name is sent in as a parameter, and returns a pointer to a GLMesh object that it created based on the file information.
	This method throws errors if the file doesn't exist, is not an obj file, etc.
*/
GLMesh* OBJReader::loadOBJFile(const char* fileName){
	if (fileName == NULL)
		throwError("fileName is NULL.");
	
//	Logger::out()<< "Loading mesh: " << fileName <<std::endl;

	FILE* f = fopen(fileName, "r");
	if (f == NULL)
		throwError("Cannot open file \'%s\'.", fileName);

	GLMesh* result = new GLMesh();

	result->setOriginalFilename( fileName );

	//have a temporary buffer used to read the file line by line...
	char buffer[200];

	//and this is an array of texture coordinates - the Point3d is a simple data type so I can use the DynamicArray
	DynamicArray<Point3d> texCoordinates;


	//this variable will keep getting populated with face information
	GLIndexedPoly temporaryPolygon;

	//this is where it happens.
	while (!feof(f)){
		//get a line from the file...
		fgets(buffer, 200, f);
		//see what line it is...
		int lineType = getLineType(buffer);
		if (lineType == VERTEX_INFO){
			//we need to read in the three coordinates - skip over the v
			Point3d vertexCoords = readCoordinates(buffer + 1);
			result->addVertex(vertexCoords);
		}

		if (lineType == TEXTURE_INFO){
			Point3d texCoords = readCoordinates(buffer + 2);
			texCoordinates.push_back(texCoords);
		}

		if (lineType == FACE_INFO){
			temporaryPolygon.indexes.clear();
			int vIndex, tIndex;
			int flag;
			char* tmpPointer = buffer+1;
			while (tmpPointer = getNextIndex(tmpPointer, vIndex, tIndex, flag)){
				temporaryPolygon.indexes.push_back(vIndex-1);
				if (flag & READ_TEXTCOORD_INDEX){
					if (tIndex<0)
						result->setVertexTexCoordinates(vIndex, texCoordinates[texCoordinates.size()+tIndex]);
					else
						result->setVertexTexCoordinates(vIndex, texCoordinates[tIndex]);
				}
			}
			if (temporaryPolygon.indexes.size() == 0)
				tprintf("Found a polygon with zero vertices.\n");
			else
				result->addPoly(temporaryPolygon);
		}

	}

	fclose(f);
	return result;
}




