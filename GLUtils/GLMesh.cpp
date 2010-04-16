#include "GLMesh.h"
#include <MathLib/Vector3d.h>


/**
	this is the default constructor
*/
GLMesh::GLMesh(void){
	vertexList = DynamicArray<double>();
	normalList = DynamicArray<double>();
	texCoordList = DynamicArray<double>();
	DynamicArray<SharedVertexInfo*> sharedVertices;
	polygons = new GLPolyCategory();
	useTextureMapping = false;
	useNormals = false;
	nrPolys = 0;
	vertexCount = 0;
	r = g = b = a = 1;
}


/**
	this is the destructor
*/
GLMesh::~GLMesh(void){
	for (int i=0;i<vertexCount;i++)
		if (sharedVertices[i]!=NULL)
			delete sharedVertices[i];
	delete polygons;
}

/**
	this method is used to add a new vertex to the mesh
*/
void GLMesh::addVertex(Point3d &coords){
	vertexList.push_back(coords.x);
	vertexList.push_back(coords.y);
	vertexList.push_back(coords.z);

	//also make sure the normal/text coordinates have the same size...
	texCoordList.push_back(0.0);
	texCoordList.push_back(0.0);
	texCoordList.push_back(0.0);
	normalList.push_back(0.0);
	normalList.push_back(0.0);
	normalList.push_back(0.0);
	sharedVertices.push_back(NULL);
	vertexCount++;
}

/**
	this method also adds texture coordinates for the vertex that is to be added.
*/
void GLMesh::addVertex(Point3d &coords, Point3d &texCoords){
	vertexList.push_back(coords.x);
	vertexList.push_back(coords.y);
	vertexList.push_back(coords.z);

	//also make sure the normal/text coordinates have the same size...
	texCoordList.push_back(texCoords.x);
	texCoordList.push_back(texCoords.y);
	texCoordList.push_back(texCoords.z);
	normalList.push_back(0.0);
	normalList.push_back(0.0);
	normalList.push_back(0.0);
	sharedVertices.push_back(NULL);
	useTextureMapping = true;
	vertexCount++;
}

/**
	this method sets the coordinates for an existing vertex
*/
void GLMesh::setVertexCoordinates(int index, Point3d &coords){
	if (index>=0 && index<vertexCount){
		vertexList[3*index+0] = coords.x;
		vertexList[3*index+1] = coords.y;
		vertexList[3*index+2] = coords.z;
	}
}

/**
	This method sets the texture coordinates for an existing vertex.
*/
void GLMesh::setVertexTexCoordinates(int index, Point3d &texCoords){
	if (index>=0 && index<vertexCount){
		useTextureMapping = true;
		texCoordList[3*index+0] = texCoords.getX();
		texCoordList[3*index+1] = texCoords.getY();
		texCoordList[3*index+2] = texCoords.getZ();
	}
}


/**
	This is the method that adds new polygons to the mesh. The polygons have to be populated by the class that reads in the
	mesh from a file.
*/
void GLMesh::addPoly(GLIndexedPoly &p){
	//now we must add the neighbour's info to each vertex in this poly
	for (uint i=0;i<p.indexes.size();i++){
		if (sharedVertices[p.indexes[i]] == NULL)
			sharedVertices[p.indexes[i]] = new SharedVertexInfo();
		int index, n1index, n2index;
		index = p.indexes[i];
		n1index = p.indexes[(i+1)%p.indexes.size()];
		n2index = p.indexes[(i-1+p.indexes.size())%p.indexes.size()];
		if (index<0 || index >= vertexCount ||n1index<0 || n1index >= vertexCount ||n2index<0 || n2index >= vertexCount)
			return;
		sharedVertices[index]->addVertexInfo(VertexInfo(n1index, n2index));
	}
	polygons->addPoly(p);
	nrPolys++;
}


/**
	This method offsets the mesh with the given delta.
	Normals must be recomputed after offsetting
*/
void GLMesh::offset( const Vector3d& delta ){
	if( delta.x == 0 && delta.y == 0 && delta.z == 0 )
		return;

	for (int i=0; i<vertexCount; ++i) {
		vertexList[3*i+0] += delta.x;
		vertexList[3*i+1] += delta.y;
		vertexList[3*i+2] += delta.z;
	}
}

/**
	This method scales the mesh with the given factors.
	Normals must be recomputed after scaling
*/
void GLMesh::scale( const Vector3d& scaling ){
	if( scaling.x == 1 && scaling.y == 1 && scaling.z == 1 )
		return;

	for (int i=0; i<vertexCount; ++i) {
		vertexList[3*i+0] *= scaling.x;
		vertexList[3*i+1] *= scaling.y;
		vertexList[3*i+2] *= scaling.z;
	}
}

/**
	This method is used to compute the normals. The modifier passed in as a parameter should be either 1 or -1, and it
	should indicate if the polygons in this mesh have their vertices expressed in clockwise or anticlockwise order.
*/
void GLMesh::computeNormals(double modifier){
/*	Replace this... store the indexes of the two neighbours, instead of storing the way to retrieve them... 
		makes the code simpler*/
	useNormals = true;
	for (int i=0;i<vertexCount;i++){
		int nrNormals = 0;
		Vector3d result = Vector3d(0,0,0);
		if (sharedVertices[i] == NULL)
			continue;
		for (uint j=0;j<sharedVertices[i]->vertexInstances.size();j++){
			VertexInfo tempVertexInfo = sharedVertices[i]->vertexInstances[j];
			//now look in that list, and retrieve the indexes of the current vertex, and its neighbours
			int n1Index = tempVertexInfo.n1Index;
			int n2Index = tempVertexInfo.n2Index;

			//now look in the vertex list, and construct the normal vector (i is the current index...)
			Vector3d v1 = Vector3d(Point3d(vertexList[3*i+0],vertexList[3*i+1],vertexList[3*i+2]),
								Point3d(vertexList[3*n1Index+0],vertexList[3*n1Index+1],vertexList[3*n1Index+2]));

			Vector3d v2 = Vector3d(Point3d(vertexList[3*n2Index+0],vertexList[3*n2Index+1],vertexList[3*n2Index+2]),
								Point3d(vertexList[3*i+0],vertexList[3*i+1],vertexList[3*i+2]));
			result+=((v2.crossProductWith(v1))*modifier).toUnit();
			nrNormals++;
		}
		//and finally, compute the resulting normal vector and store it...
		if (nrNormals>0)
			(result/=nrNormals);
		result.toUnit();
		normalList[3*i+0] = result.getX();
		normalList[3*i+1] = result.getY();
		normalList[3*i+2] = result.getZ();
	}
}


/**
	This method prints out the normals of the model - for testing purposes.
*/
void GLMesh::drawNormals(){
	for (int i=0;i<vertexCount;i++){
		glBegin(GL_LINES);
			glVertex3dv(&(vertexList.front())+3*i);
			glVertex3d(vertexList[3*i+0]+normalList[3*i+0],vertexList[3*i+1]+normalList[3*i+1],vertexList[3*i+2]+normalList[3*i+2]);
		glEnd();
	}
}

/**
	This method draws the model.
*/
void GLMesh::drawMesh(bool useColours){
	/* enable the vertex array list*/
	if (vertexList.size()>0){
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_DOUBLE,0,&(vertexList.front()));
	}
	if (useNormals){
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE,0,&(normalList.front()));
	}
	if (useColours){
		/*enable the normal array list */
		if (useNormals){
			float tempColor[] = {(float)r,(float)g,(float)b,(float)(a)};
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tempColor);
		}
		else glColor4d(r, g, b, a);
	}

	/* enable the texture coordinates arrays */
	if (useTextureMapping){
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3,GL_DOUBLE,0,&(texCoordList.front()));
	}

	for (uint i = 0;i<polygons->categories.size();i++){
		GLPolyIndexList* tempIndexList = polygons->categories[i];
		if (tempIndexList == NULL)
			return;//PANIC
		if (tempIndexList->polyVertexCount == 3)
			glDrawElements(GL_TRIANGLES, tempIndexList->indexList.size(), GL_UNSIGNED_INT, &(tempIndexList->indexList.front()));
		else if (tempIndexList->polyVertexCount == 4)
			glDrawElements(GL_QUADS, tempIndexList->indexList.size(), GL_UNSIGNED_INT, &(tempIndexList->indexList.front()));
		else {
			//hopefully there aren't too many polys that are not triangles or quads... it would slow things down a little...
			for (unsigned int j=0;j<tempIndexList->indexList.size()/tempIndexList->polyVertexCount;j++)
				glDrawElements(GL_POLYGON,tempIndexList->polyVertexCount, GL_UNSIGNED_INT, &(tempIndexList->indexList.front()) + j*tempIndexList->polyVertexCount);
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



/**
	This method renders the mesh, including its current transformations, as a set of vertices 
	and faces that will be appended to the passed OBJ file.	

	vertexIdxOffset indicates the index of the first vertex for this object, this makes it possible to render
	multiple different meshes to the same OBJ file
	
	The passed transformation matrix transforms from the model coordinate to the world

	Returns the number of vertices written to the file
*/	
uint GLMesh::renderToObjFile( FILE* fp, uint vertexIdxOffset, TransformationMatrix toWorld ) {

	Point3d vertex;
	uint nbVerts = vertexList.size()/3;
	for( uint i = 0; i < nbVerts; i++ ) {
	
		vertex.setValues(vertexList[i*3], vertexList[i*3+1], vertexList[i*3+2]);
		vertex = toWorld * vertex;
		fprintf( fp, "v %lf %lf %lf\n", vertex.getX(), vertex.getY(), vertex.getZ() );
	
	}

	fprintf( fp, "\n" );

	// Render faces
	for ( uint i = 0; i<polygons->categories.size(); i++ ){
		GLPolyIndexList* tempIndexList = polygons->categories[i];
		uint vertsPerFace = tempIndexList->polyVertexCount;
		uint nbFaces = tempIndexList->indexList.size() / vertsPerFace;
		uint idx = 0;
		for ( uint j = 0; j<nbFaces; j++ ) {
			uint nbTriInFan = vertsPerFace - 2;
			for ( uint k = 0; k < nbTriInFan; k++ ) {
				// Vertex indices are 1-based
				fprintf( fp, "f %d %d %d \n", tempIndexList->indexList[idx]+vertexIdxOffset+1,
					tempIndexList->indexList[idx+1+k]+vertexIdxOffset+1,
					tempIndexList->indexList[idx+2+k]+vertexIdxOffset+1 );
			}
			idx += vertsPerFace;
		}
	}

	fprintf( fp, "\n\n" );

	return nbVerts;
}


