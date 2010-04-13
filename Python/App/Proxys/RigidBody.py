'''
Created on 2009-09-02

@author: beaudoin
'''

import Physics
import Proxy, Member
import PyUtils

import Physics

def addMesh( rb, mesh ):
     """Adds a single mesh which can be either:
       - A single string (mesh filename)
       - A tuple or list with a single string (mesh filename)
       - A tuple or list with two elements (mesh filename and a 4-tuple color)
       - A dict with "obj" and "colour" as keys (mesh filename and a 4-tuple color)
     """
     
     if mesh is None : 
         return
     
     errorStr = "Mesh should be a string or a 2-tupe, string and colour."
     
     obj = None
     colour = None
     if isinstance( mesh, basestring ):
         obj = mesh
     else:
         try:
             # Maybe a list or tuple?
             if len(mesh) > 2:
                 raise TypeError( "More than 2 elements, mesh is probably a dict. This error should be caught." )
             obj = mesh[0]
             if len(mesh) > 1:
                 colour = mesh[1]
         except(TypeError, IndexError):
             try:
                 obj = mesh["obj"]
                 if mesh.has_key("colour"):
                     colour = mesh["colour"]
             except(KeyError, AttributeError):
                 raise TypeError( errorStr )
 
     if not isinstance( obj, basestring ):
         raise TypeError( errorStr )
     if colour is not None and len(colour) != 4:
         raise TypeError( errorStr )
     rb.addMeshObj( obj )
     if colour is not None:
         rb.setColour( colour[0], colour[1], colour[2], colour[3] )      
     
def setMeshes( rb, meshes ):
    PyUtils.callOnObjectOrList( meshes, lambda(mesh): addMesh(rb,mesh) )

def getMeshes( rb ):
    """DUMMY! Meshes are forgotten."""
    return None

def getGroundCoeffs(object):
    return (object.getGroundSoftness(), object.getGroundPenalty())

def setGroundCoeffs(object, value):
    return object.setODEGroundCoefficients(*value)

cls = Physics.RigidBody
RigidBody = Proxy.create( cls, loader = Physics.world().addRigidBody,
    members = [
        Member.Basic( str, 'name', 'UnnamedRigidBody', cls.getName, cls.setName ),
        Member.Basic( list, 'meshes', None, getMeshes, setMeshes ),
        Member.Basic( float, 'mass', 1.0, cls.getMass, cls.setMass ),
        Member.Vector3d( 'moi', (1.0,1.0,1.0), cls.getMOI, cls.setMOI ),
        Member.ObjectList( 'cdps', None, cls.getCollisionDetectionPrimitive, cls.getCollisionDetectionPrimitiveCount, cls.addCollisionDetectionPrimitive ),
        Member.Point3d( 'pos', (0.0,0.0,0.0), cls.getCMPosition, cls.setCMPosition ),
        Member.Vector3d( 'vel', (0.0,0.0,0.0), cls.getCMVelocity, cls.setCMVelocity ),
        Member.Quaternion( 'orientation', (0.0,(1.0,0.0,0.0)), cls.getOrientation, cls.setOrientation ),
        Member.Vector3d( 'angularVel', (0.0,0.0,0.0), cls.getAngularVelocity, cls.setAngularVelocity ),
        Member.Basic( bool, 'locked', False, cls.isLocked, cls.lockBody ),
        Member.Basic( float, 'frictionCoeff', 0.8, cls.getFrictionCoefficient, cls.setFrictionCoefficient ),
        Member.Basic( float, 'restitutionCoeff', 0.35, cls.getRestitutionCoefficient, cls.setRestitutionCoefficient ),        
        Member.Basic( tuple, 'groundCoeffs', (0.00001,0.2), getGroundCoeffs, setGroundCoeffs ),
        Member.Basic( bool, 'planar', False, cls.isPlanar, cls.setPlanar )
    ] )

cls = Physics.ArticulatedRigidBody
ArticulatedRigidBody = Proxy.create( cls, parent = RigidBody, loader = Physics.world().addRigidBody, caster = Physics.castToArticulatedRigidBody )


    
    
    
