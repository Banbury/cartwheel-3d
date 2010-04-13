'''
Created on 2009-10-06

@author: beaudoin

A collection of useful functions for creating stock meshes
'''

import GLUtils, PyUtils, MathLib, math
from MathLib import Point3d, Vector3d

def create( vertices, faces, colour=(0.6,0.6,0.6) ):
    """
    Creates a mesh having the specified vertices and faces.
    Vertices should be a list of 3-tuples of float or Point3d (positions).
    Faces should be a list of tuples of indices.
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """
    
    mesh = GLUtils.GLMesh()
    
    for vertex in vertices:
        mesh.addVertex( PyUtils.toPoint3d(vertex) )
        
    for face in faces:
        poly = GLUtils.GLIndexedPoly()
        for index in face:
            poly.addVertexIndex( index )
        mesh.addPoly(poly)
        
    try:
        mesh.setColour( *colour )
    except TypeError:
        mesh.setColour( *(colour + (1,)) )

    mesh.computeNormals()

    return mesh

def createBox( size=(1,1,1), position=(0,0,0), colour=(0.6,0.6,0.6) ):
    """
    Creates the mesh for a box having the specified size and a specified position.
    The size should be a 3-tuple (xSize, ySize, zSize).
    The position should be a 3-tuple.
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """
    
    size     = PyUtils.toVector3d(size)
    position = PyUtils.toPoint3d(position)
    vertices = []
    delta = MathLib.Vector3d()
    for repeat in range(3):
        for x in (-0.5,0.5) :
            delta.x = size.x * x
            for y in (-0.5,0.5) :
                delta.y = size.y * y
                for z in (-0.5,0.5) :
                    delta.z = size.z * z
                    vertices.append( position + delta )
    
    faces = [(0,1,3,2),(5,4,6,7),  # YZ Faces
             (9,13,15,11),(12,8,10,14),  # XY Faces
             (18,19,23,22),(17,16,20,21)]  # XZ Faces
    
    return create( vertices, faces, colour )


def createTaperedBox( position=(0,0,0), size=(1,1,1), colour=(0.6,0.6,0.6), samplesY = 20, samplesXZ = 20, exponentBottom = 4, exponentTop = 4, exponentSide = 4 ):
    """
    Creates the mesh for a box having the specified size and a specified position.
    The size should be a 3-tuple (xSize, ySize, zSize).
    The position should be a 3-tuple.
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """

    return createEllipsoid( position, (size[0]/2.0,size[1]/2.0,size[2]/2.0), colour, samplesY, samplesXZ, exponentBottom, exponentTop, exponentSide )    


def createSphere( position=(0,0,0), radius=1, colour=(0.6,0.6,0.6), samplesY = 20, samplesXZ = 20 ):
    """
    Creates the mesh for an sphere having the specified position and radius
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """
    return createEllipsoid( position, (radius,radius,radius), colour, samplesY, samplesXZ )    

def createEllipsoid( position=(0,0,0), radius=(1,1,1), colour=(0.6,0.6,0.6), samplesY = 20, samplesXZ = 20, exponentBottom = 2, exponentTop = 2, exponentSide = 2 ):
    """
    Creates the mesh for an ellipsoid having the specified position and radius
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """
    
    if exponentBottom < 2.0 or exponentTop < 2.0 or exponentSide < 2.0 :
        raise ValueError( 'Exponents for ellipsoid must all be under 2.0!' )
    
    position = PyUtils.toPoint3d(position)
    vertices = []
    for i in range(1,samplesY):
        thetaI = i*math.pi/float(samplesY)
        if i < samplesY / 2 : 
            n = exponentTop
        else:
            n = exponentBottom
        cos = math.cos(thetaI)  
        y = cos * radius[1]
        scaleXZ = math.pow( 1-math.pow(math.fabs(cos),n), 1.0/float(n) )
        for j in range(0,samplesXZ):
            thetaJ = j*2.0*math.pi/float(samplesXZ)
            n = exponentSide
            cos = math.cos(thetaJ)
            x = cos * scaleXZ * radius[0]
            z = math.pow( 1-math.pow(math.fabs(cos),n), 1.0/float(n) ) * math.copysign(1, math.sin(thetaJ)) * scaleXZ * radius[2]
            vertices.append( position + Vector3d(x,y,z) )
    vertices.append( position + Vector3d(0,radius[1],0) )
    vertices.append( position + Vector3d(0,-radius[1],0) )    

    faces = []
    for i in range(0,(samplesY-2)*samplesXZ,samplesXZ) :
        for j in range(0,samplesXZ) :
            faces.append( (i+j, i+(j+1)%samplesXZ, i+samplesXZ+(j+1)%samplesXZ, i+samplesXZ+j) ) 

    for i in range(0,samplesXZ) :
        base = (samplesY-2)*samplesXZ
        faces.append( ((i+1)%samplesXZ, i, (samplesY-1)*samplesXZ) ) 
        faces.append( (base+i, base+(i+1)%samplesXZ, (samplesY-1)*samplesXZ+1) ) 

    
    return create( vertices, faces, colour )

def createCylinder( basePoint=(0,-1,0), tipPoint=(0,1,0), radius = 1.0, colour=(0.6,0.6,0.6), samples = 20 ):
    """
    Creates the mesh for a cylinder between the two specified points.
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """
    
    basePoint = PyUtils.toPoint3d(basePoint)
    tipPoint = PyUtils.toPoint3d(tipPoint)
    baseToTipVector = Vector3d(basePoint,tipPoint)
    if baseToTipVector.isZeroVector() :
        raise ValueError( 'Invalid points for cylinder: base and tip are equal!' )
    baseToTipUnitVector = baseToTipVector.unit()
    xUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(0,0,1) )
    if xUnitVector.length() < 0.5 :
        xUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(0,-1,0) )
    xUnitVector.toUnit()
    yUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(-1,0,0) )
    if yUnitVector.length() < 0.5 :
        yUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(0,1,0) )
    yUnitVector.toUnit()

    vertices = []
    for i in range(samples):
        theta = i * 2 * math.pi / float(samples)
        vertices.append( basePoint + xUnitVector * math.cos(theta) * radius + yUnitVector * math.sin(theta) * radius )
    for i in range(samples):
        theta = i * 2 * math.pi / float(samples)
        vertices.append( tipPoint + xUnitVector * math.cos(theta) * radius + yUnitVector * math.sin(theta) * radius )
    for i in range(samples):
        theta = i * 2 * math.pi / float(samples)
        vertices.append( basePoint + xUnitVector * math.cos(theta) * radius + yUnitVector * math.sin(theta) * radius )
        vertices.append( tipPoint + xUnitVector * math.cos(theta) * radius + yUnitVector * math.sin(theta) * radius )
        
    faces = [ range(0,samples), range(samples,2*samples) ]
    for i in range(0,2*samples,2) :
        base = 2*samples
        size = 2*samples
        faces.append( (base+i, base+i+1, base+(i+3)%size, base+(i+2)%size ) )
    
    return create( vertices, faces, colour )
    

def createCone( basePoint=(0,-1,0), tipPoint=(0,1,0), radius = 1.0, colour=(0.6,0.6,0.6), samples = 20 ):
    """
    Creates the mesh for a cone between the two specified points.
    Colour should be a 3-tuple (R,G,B) or a 4-tuple (R,G,B,A)
    """
    
    basePoint = PyUtils.toPoint3d(basePoint)
    tipPoint = PyUtils.toPoint3d(tipPoint)
    baseToTipVector = Vector3d(basePoint,tipPoint)
    if baseToTipVector.isZeroVector() :
        raise ValueError( 'Invalid points for cylinder: base and tip are equal!' )
    baseToTipUnitVector = baseToTipVector.unit()
    xUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(0,0,1) )
    if xUnitVector.length() < 0.5 :
        xUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(0,-1,0) )
    xUnitVector.toUnit()
    yUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(-1,0,0) )
    if yUnitVector.length() < 0.5 :
        yUnitVector = baseToTipUnitVector.crossProductWith( Vector3d(0,1,0) )
    yUnitVector.toUnit()

    vertices = []
    for i in range(samples):
        theta = i * 2 * math.pi / float(samples)
        vertices.append( basePoint + xUnitVector * math.cos(theta) * radius + yUnitVector * math.sin(theta) * radius )
    for i in range(samples):
        theta = i * 2 * math.pi / float(samples)
        vertices.append( basePoint + xUnitVector * math.cos(theta) * radius + yUnitVector * math.sin(theta) * radius )
    vertices.append( tipPoint )
        
    faces = [ range(0,samples) ]
    for i in range(0,samples) :
        base = samples
        size = samples
        faces.append( (base+i, base+(i+1)%size, 2*samples ) )
    
    return create( vertices, faces, colour )
        
    