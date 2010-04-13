#pragma once

#include <Utils/Utils.h>

#include <mathLib/mathLib.h>
#include <MathLib/Point3d.h>
#include <MathLib/TransformationMatrix.h>

#include <Include/glHeaders.h>

#include <GLUtils/GLUtilsDll.h>

/*=====================================================================================================================================================================*
 * This file implements the classes needed for the storage and display of simple 3d meshes. The classes are implemented in a way that promotes the use of vertex       *
 * arrays for increased efficiency.                                                                                                                                    *
 * At some point I might implement a better mesh data structure suitable for mesh editing (maybe DCEL (the half-Edge structre - http://www.holmes3d.net/graphics/dcel/)*
 * like the one used by Bruno Levy for Graphite). Until then I'll use this one...                                                                                      *
 * NOTE: One BIG limitation of this class is that each vertex can have only one set of texture coordinates and normal attached to it!!!! It will do for now.           *
 *                                                                                                                                                                     *
 * The following classes are implemented:                                                                                                                              *
 *                                                                                                                                                                     *
 * CLASS VertexInfo: For every vertex in every polygon, this class will keep track of the neighbours - this will be used for fast normal computations.                 *
 *                                                                                                                                                                     *
 * CLASS SharedVertexInfo: This class is used to store, for each vertex, a collection of VertexInfo classes - this way, when computing the normal at each vertex,      *
 *					we only need to iterate through this collection in order to get the normals from each polygon that contains this vertex, and then we average them  *
 *                                                                                                                                                                     *
 * CLASS GLIndexedPoly: This class contains a list of vertex indices that correspond to the vertices that make up the polygon. This class should be populated by the   *
 *					class that reads in the mesh file, and then passes it to the GLMeshObject.                                                                         *
 *                                                                                                                                                                     *
 * CLASS GLPolyIndexList: This class contains an array of tightly packed indeces of vertices that belong to polygons that have the same number of vertices. As an      *
 *					example the indices of all vertices that belong to traingles in the mesh, would be tightly packed together in one instance of this class. There    *
 *					will be one of these objects for all the triangles in the mesh, one for all the quads, and so on.                                                  *
 *                                                                                                                                                                     *
 * CLASS GLPolyCategory: The last in the set of helper classes, this one acts as a container of GLPolyIndexList, one for triangles, one for quads, and so on.          *
 *					Everytime a new polygon is added, this class is responsible with choosing the correct PolyIndexList that its vertices should be added to.          *
 *                                                                                                                                                                     *
 * CLASS GLMesh:	This is the class that has lists of vertex coordinates, texture coordinates and normals that will be used when handling the object. It implements  *
 *					methods for printing the model.                                                                                                                    *
 *                                                                                                                                                                     *
 *=====================================================================================================================================================================*/



//class SharedVertexInfo;

/**
	this structure is used to store, for a given vertex, and for a given polygon, the two neighbouring vertices.
	This information will be used to quickly generate the normals for each vertex. Everything in this class is
	private because it is only meant as a helper class.
*/
class GLUTILS_DECLSPEC VertexInfo {
public:
	int n1Index, n2Index;		//these are the indices of the two neighbours (the one before and the one after in the polygon)
	VertexInfo() : n1Index(-1), n2Index(-2) {}
	VertexInfo(int n1Index, int n2Index){this->n1Index = n1Index; this->n2Index = n2Index;}
	VertexInfo(const VertexInfo& other){
		this->n1Index = other.n1Index;
		this->n2Index = other.n2Index;
	}
	~VertexInfo(){
	}
};

// Instanciate used STL classes
GLUTILS_TEMPLATE( DynamicArray<VertexInfo> )


/**
	This class is used to store, for each vertex, a collection of VertexInfo classes - this way, when computing the normal at each vertex,
	we only need to iterate through this collection in order to get the normals from each polygon that contains this vertex
*/
class GLUTILS_DECLSPEC SharedVertexInfo {
friend class GLMesh;
private:
	//the VertexInfo class only has two ints, so we can use the POD DynamicArray
	DynamicArray<VertexInfo> vertexInstances;
	/**
		Default constructor
	*/
	SharedVertexInfo(){
		vertexInstances = DynamicArray<VertexInfo>();
	}

	/**
		Destructor.
	*/
	~SharedVertexInfo(){
	}

	/**
		Adding a new vertexInfo object.
	*/
	void addVertexInfo(VertexInfo &vi){
		vertexInstances.push_back(vi);
	}
};

// Instanciate used STL classes
GLUTILS_TEMPLATE( DynamicArray<SharedVertexInfo*> )


/**
	This class contains a list of vertex indices that correspond to the vertices that make up the polygon. This class should be populated by the
	class that reads in the mesh file, and then passes it to the GLMeshObject.
*/
class GLUTILS_DECLSPEC GLIndexedPoly {
	friend class GLPolyIndexList;
	friend class GLPolyCategory;
	friend class GLMesh;
	friend class OBJReader;
//private:
public:
	DynamicArray<int> indexes;
	
	/**
		clear the indices
	*/
	void clear(){
		indexes.clear();
	}
		
	/**
		adding the index of a new vertex
	*/
	void addVertexIndex(int index){
		indexes.push_back(index);
	}

	/**
		a copy constructor.
	*/

	GLIndexedPoly& operator = (const GLIndexedPoly& other){
		this->indexes = other.indexes;
		return *this;
	}

	/**
		default constructor.
	*/
	GLIndexedPoly(void){
		indexes = DynamicArray<int>();
	}
	/**
		destructor.
	*/
	~GLIndexedPoly(void){
	}
};

/**
	This class contains an array of tightly packed indeces of vertices that belong to polygons that have the same number of vertices. As an
	example the indices of all vertices that belong to traingles in the mesh, would be tightly packed together in one instance of this class. 
	There will be one of these objects for all the triangles in the mesh, one for all the quads, and so on. 
*/
class GLUTILS_DECLSPEC GLPolyIndexList {
	friend class GLPolyCategory;
	friend class GLMesh;
private:
	DynamicArray<unsigned int> indexList;		//a growing list of indexes
	unsigned int polyVertexCount;				//that's how many vertices belong to each polygon... list has listSize/polyVertexCount polygons
public:
	GLPolyIndexList(unsigned int vertCount){indexList = DynamicArray<unsigned int>(); this->polyVertexCount = vertCount;}
	~GLPolyIndexList(){
	}


	/**
		This method adds the indices of the vertices of the polygon p to indexList.
	*/
	int addPoly(GLIndexedPoly &p){
		if (p.indexes.size()!=this->polyVertexCount)
			return -1;
		for (uint i=0;i<p.indexes.size();i++)
			indexList.push_back(p.indexes[i]);
		return (indexList.size()%this->polyVertexCount)-1;
	}
};

// Instanciate used STL classes
GLUTILS_TEMPLATE( DynamicArray<GLPolyIndexList*> )

/**
	finally, this class acts as a container of GLIndexLists, and everytime a new poly (GLIndexedPoly) is added,
	it checks to see if there is a list of the corect poly indexed count, and adds the data to that one, or it creates a new
	category.
*/
class GLUTILS_DECLSPEC GLPolyCategory {
friend class GLMesh;
private:
	DynamicArray<GLPolyIndexList*> categories;

	/**
		This method adds a new polygon to the category that has the same vertex count. If none exists then one is created.
	*/
	int addPoly(GLIndexedPoly &p){
		for (uint i=0;i<categories.size();i++){
			if (categories[i]->polyVertexCount == p.indexes.size())
				return categories[i]->addPoly(p);
		}
		categories.push_back(new GLPolyIndexList(p.indexes.size()));
		return categories[categories.size()-1]->addPoly(p);
	}

	/**
		constructor...
	*/
	GLPolyCategory(){categories = DynamicArray<GLPolyIndexList*>();};

	/**
		destructor - make sure we free the memory
	*/
	~GLPolyCategory(){
		for (uint i=0;i<categories.size();i++)
			delete categories[i];
	}

	/**
		This method returns the PolyIndexList whose polygons all have vertexCount vertices.
	*/
	GLPolyIndexList* getIndexList(int vertexCount){
		for (uint i=0;i<categories.size();i++){
			if (categories[i]->polyVertexCount == vertexCount)
				return categories[i];
		}
		return NULL;
	}
};


/**
	This class is responsible with the storage and drawing of 3d static meshes. It is designed to work with OpenGL
	array lists in order to improve performance.
*/
class GLUTILS_DECLSPEC GLMesh {
	friend class ParticleSystem;;
private:
	//this is the array of vertex coordinates.
	DynamicArray<double> vertexList;
	//this array stores the normals for each vertex - NOTE: there can only be one for each vertex
	DynamicArray<double> normalList;
	//this array holds the texture coordinates - NOTE: there can only be one set of texture coordinates for each vertex
	DynamicArray<double> texCoordList;
	//this variable is used to store information related to which polys share each vertex. Only used for computation of normals
	DynamicArray<SharedVertexInfo*> sharedVertices;	

	//this is the total number of vertices in the mesh
	int vertexCount;

	//this PolyCategory object holds the list of triangles, quads, etc in the mesh.
	GLPolyCategory *polygons;

	//variable that is used to decide whether or not texture mapping is to be used.
	bool useTextureMapping;
	//variable that indicates whether or not the normals are to be used.
	bool useNormals;

	//this is the total number of polygons in the mesh.
	int nrPolys;

	//and the base colour of the mesh (white by default)
	double r, g, b, a;

	// Keep the original filename around
	char originalFilename[100];

public:
	/**
		this is the default constructor
	*/
	GLMesh(void);

	/**
		this is the destructor
	*/
	~GLMesh(void);

	void setOriginalFilename( const char* filename ) {
		strncpy( originalFilename, filename, 99 );
	}

	const char* getOriginalFilename() const {
		return originalFilename;
	}

	/**
		this method is used to add a new vertex to the mesh
	*/
	void addVertex(Point3d &coords);

	/**
		this method also adds texture coordinates for the vertex that is to be added.
	*/
	void addVertex(Point3d &coords, Point3d &texCoords);

	/**
		this method sets the coordinates for an existing vertex
	*/
	void setVertexCoordinates(int index, Point3d &coords);

	/**
		This method sets the texture coordinates for an existing vertex.
	*/
	void setVertexTexCoordinates(int index, Point3d &texCoords);

	/**
		This method offsets the mesh with the given delta.
		Normals must be recomputed after offsetting
	*/
	void offset( const Vector3d& delta );

	/**
		This method scales the mesh with the given factors.
		Normals must be recomputed after scaling
	*/
	void scale( const Vector3d& scaling );

	/**
		This method is used to compute the normals. The modifier passed in as a parameter should be either 1 or -1, and it
		should indicate if the polygons in this mesh have their vertices expressed in clockwise or anticlockwise order.
	*/
	void computeNormals(double modifier = 1);
	
	/**
		This is the method that adds new polygons to the mesh. The polygons have to be populated by the class that reads in the
		mesh from a file.
	*/
	void addPoly(GLIndexedPoly &p);

	/**
		This method draws the model.
	*/
	void drawMesh(bool useColours = true);

	/**
		This method prints out the normals of the model - for testing purposes.
	*/
	void drawNormals();

	
	/**
		This method renders the mesh, including its current transformations, as a set of vertices 
		and faces that will be appended to the passed OBJ file.	

		vertexIdxOffset indicates the index of the first vertex for this object, this makes it possible to render
		multiple different meshes to the same OBJ file
		
		The passed transformation matrix transforms from the model coordinate to the world

		Returns the number of vertices written to the file

	*/	
	uint renderToObjFile( FILE* fp, uint vertexIdxOffset, TransformationMatrix toWorld );


	/**
		This method returns the number of polygons in the current mesh.
	*/
	int getPolyCount() const {return nrPolys;}

	/**
		This method returns the number of vertices in the current mesh.
	*/
	int getVertexCount() const {return vertexList.size()/3;}

	/**
		This method returns a reference to the dynamic array that stores the vertex positions.
	*/
	double* getVertexArray(){
		return &vertexList[0];
	}

	/**
		This method makes sure that the texture information will not be used
	*/
	void dontUseTextureMapping(){
		this->useTextureMapping = false;
	}

	/**
		This method will force the mesh to use texture mapping
	*/

	void doUseTextureMapping(){
		this->useTextureMapping = true;
	}

	/**
		Sets the base colour of the mesh
	*/
	void setColour(double r_, double g_, double b_, double a_){
		this->r = r_;
		this->g = g_;
		this->b = b_;
		this->a = a_;
	}

	double getColourR() const { return r; }
	double getColourG() const { return g; }
	double getColourB() const { return b; }
	double getColourA() const { return a; }

};

GLUTILS_TEMPLATE( DynamicArray<GLMesh*> )



