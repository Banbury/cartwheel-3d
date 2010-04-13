'''
Created on 2009-10-06

@author: beaudoin

A collection of useful functions for creating stock rigid bodies
'''

import Physics, MathLib, Mesh, PyUtils, math
from MathLib import Point3d, Vector3d


def createBox( size=(1,1,1), colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create a rigid body for a box of the specified size. Other rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.RigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, size[0]*size[1]*size[2] )

    from App import Proxys
    proxy = Proxys.RigidBody( **kwargs )
    
    return _createBox( proxy, size, colour, moiScale, withMesh )


def createArticulatedBox( size=(1,1,1), colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create an articulated rigid body for a box of the specified size. Other articulated rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.ArticulatedRigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If mass is negative, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, size[0]*size[1]*size[2] )

    from App import Proxys
    proxy = Proxys.ArticulatedRigidBody( **kwargs )
    
    return _createBox( proxy, size, colour, moiScale, withMesh )



def createTaperedBox( size=(1,1,1), colour=(0.6,0.6,0.6), exponentBottom = 5, exponentTop = 5, exponentSide = 5, moiScale = 1, withMesh = True, **kwargs ):
    """
    Create a rigid body for a box of the specified size with tapered ends. Other rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.RigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, size[0]*size[1]*size[2] )

    from App import Proxys
    proxy = Proxys.RigidBody( **kwargs )
    
    return _createTaperedBox( proxy, size, colour, exponentBottom, exponentTop, exponentSide, moiScale, withMesh )


def createArticulatedTaperedBox( size=(1,1,1), colour=(0.6,0.6,0.6), exponentBottom = 5, exponentTop = 5, exponentSide = 5, moiScale = 1, withMesh = True, **kwargs ):
    """
    Create an articulated rigid body for a box of the specified size with tapered ends. Other articulated rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.ArticulatedRigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If mass is negative, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, size[0]*size[1]*size[2] )

    from App import Proxys
    proxy = Proxys.ArticulatedRigidBody( **kwargs )
    
    return _createTaperedBox( proxy, size, colour, exponentBottom, exponentTop, exponentSide, moiScale, withMesh )

def createEllipsoid( radius=(1,1,1), colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create a rigid body for an ellipsoid with the specified attributes . Other rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.RigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, 4.0/3.0 * math.pi*radius*radius*radius )

    from App import Proxys
    proxy = Proxys.RigidBody( **kwargs )
    
    return _createEllipsoid( proxy, radius, colour, moiScale, withMesh )

def createArticulatedEllipsoid( radius=(1,1,1), colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create an articulated rigid body for an ellipsoid with the specified attributes . Other rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.RigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, 4.0/3.0 * math.pi*radius[0]*radius[1]*radius[2] )

    from App import Proxys
    proxy = Proxys.ArticulatedRigidBody( **kwargs )
    
    return _createEllipsoid( proxy, radius, colour, moiScale, withMesh )


def createCylinder( axis=1, basePos=-1, tipPos=1, radius=1, colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create a rigid body for a cylinder with the specified attributes (axis is 0:x, 1:y, 2:z). Other rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.RigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, math.pi*radius*radius*math.fabs(tipPos-basePos) )

    from App import Proxys
    proxy = Proxys.RigidBody( **kwargs )
    
    return _createCylinder( proxy, axis, basePos, tipPos, radius, colour, moiScale, withMesh )

def createArticulatedCylinder( axis=1, basePos=-1, tipPos=1, radius=1, colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create an articulated rigid body for a cylinder with the specified attributes (axis is 0:x, 1:y, 2:z). Other articulated rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.ArticulatedRigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, math.pi*radius*radius*math.fabs(tipPos-basePos) )

    from App import Proxys
    proxy = Proxys.ArticulatedRigidBody( **kwargs )
    
    return _createCylinder( proxy, axis, basePos, tipPos, radius, colour, moiScale, withMesh )

def createCone( axis=1, basePos=-1, tipPos=1, radius=1, colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create a rigid body for a cone with the specified attributes (axis is 0:x, 1:y, 2:z). Other rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.RigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, math.pi*radius*radius*math.fabs(tipPos-basePos) )

    from App import Proxys
    proxy = Proxys.RigidBody( **kwargs )
    
    return _createCone( proxy, axis, basePos, tipPos, radius, colour, moiScale, withMesh )

def createArticulatedCone( axis=1, basePos=-1, tipPos=1, radius=1, colour=(0.6,0.6,0.6), moiScale = 1, withMesh = True, **kwargs ):
    """
    Create an articulated rigid body for a cone with the specified attributes (axis is 0:x, 1:y, 2:z). Other articulated rigid body parameters can be specified with keyword arguments, look at
    App.Proxys.ArticulatedRigidBody for more details on available arguments. The following arguments will not be used:
        meshes, moi, cdps.
    If a negative mass parameter is specified, it will be scaled by the box volume and made positive.
    """

    _fixMass( kwargs, math.pi*radius*radius*math.fabs(tipPos-basePos) )

    from App import Proxys
    proxy = Proxys.ArticulatedRigidBody( **kwargs )
    
    return _createCone( proxy, axis, basePos, tipPos, radius, colour, moiScale, withMesh )



def _fixMass( kwargs, volume ):
    """
    Private function.
    If kwargs['mass'] is defined and negative, it is made positive and scaled by volume. (Considered as density.)
    """
    try:
        if kwargs['mass'] < 0 :
            kwargs['mass'] = -kwargs['mass'] * volume
    except KeyError:
        pass
    
    
def _createBox( proxy, size, colour, moiScale, withMesh ):
    """
    Private function.
    Use createBox() or createArticulatedBox() instead.
    """

    # Mesh and cdps will be set manually
    proxy.meshes = None
    proxy.cdps = None
    
    # Compute box moi
    size = PyUtils.toVector3d(size)
    proxy.moi = MathLib.Vector3d( size.y*size.y + size.z*size.z,
                                  size.x*size.x + size.z*size.z,
                                  size.x*size.x + size.y*size.y, ) * 1.0/12.0 * proxy.mass * moiScale
 
    box = proxy.createAndFillObject()
 
    
    cdp = Physics.BoxCDP()
    halfSize = PyUtils.toVector3d(size) * 0.5
    
    cdp.setPoint1( MathLib.Point3d( halfSize * -1 ) )
    cdp.setPoint2( MathLib.Point3d( halfSize ) )
    
    box.addCollisionDetectionPrimitive( cdp )
    
    if withMesh:
        mesh = Mesh.createBox( size = size, colour = colour )
        box.addMesh(mesh)
    
    return box

def _createTaperedBox( proxy, size, colour, exponentBottom, exponentTop, exponentSide, moiScale, withMesh ):
    """
    Private function.
    Use createTaperedBox() or createArticulatedTaperedBox() instead.
    """
    taperedBox = _createBox( proxy, size, colour, moiScale = moiScale, withMesh = False )
    
    if withMesh:
        mesh = Mesh.createEllipsoid(position=(0,0,0), radius=(size[0]/2.0, size[1]/2.0, size[2]/2.0), colour = colour, 
                                    exponentBottom = exponentBottom, 
                                    exponentTop = exponentTop, 
                                    exponentSide = exponentSide)
        taperedBox.addMesh(mesh)
        
    return taperedBox
    
def _createEllipsoid( proxy, radius, colour, moiScale, withMesh ):
    """
    Private function.
    Use createEllipsoid() or createArticulatedEllipsoid() instead.
    """
    # Mesh and cdps will be set manually
    proxy.meshes = None
    proxy.cdps = None
    
    # Compute box moi
    moi = [0,0,0]
    for i in range(3):
        j = (i+1)%3
        k = (i+2)%3
        moi[i] = proxy.mass * (radius[j]*radius[j]+radius[k]*radius[k]) / 5.0
    proxy.moi = PyUtils.toVector3d(moi) * moiScale
     
    ellipsoid = proxy.createAndFillObject()
 
    cdp = Physics.SphereCDP()
    cdp.setCenter( Point3d(0,0,0) )
    cdp.setRadius( min(radius) )

    ellipsoid.addCollisionDetectionPrimitive( cdp )
    
    if withMesh:
        mesh = Mesh.createEllipsoid((0,0,0), radius, colour)
        ellipsoid.addMesh(mesh)
    
    return ellipsoid

def _createCylinder( proxy, axis, basePos, tipPos, radius, colour, moiScale, withMesh ):
    """
    Private function.
    Use createCylinder() or createArticulatedCylinder() instead.
    """
    if axis != 0 and axis != 1 and axis != 2 :
        raise ValueError( 'Axis must be 0 for x, 1 for y or 2 for z.' )

    # Mesh and cdps will be set manually
    proxy.meshes = None
    proxy.cdps = None
    
    # Compute box moi
    moi = [0,0,0]
    height = math.fabs(tipPos-basePos)
    for i in range(3):
        if i == axis :
            moi[i] = proxy.mass*radius*radius / 2.0
        else :
            moi[i] = proxy.mass*(3*radius*radius + height*height) / 12.0
        ### HACK!        
        moi[i] = max(moi[i], 0.01)
    proxy.moi = PyUtils.toVector3d(moi) * moiScale
     
    cylinder = proxy.createAndFillObject()
 
    basePoint = [0,0,0]
    tipPoint  = [0,0,0]
    basePoint[axis] = basePos
    tipPoint[axis] = tipPos
    basePoint3d = PyUtils.toPoint3d(basePoint)
    tipPoint3d = PyUtils.toPoint3d(tipPoint)
    baseToTipVector3d = Vector3d(basePoint3d, tipPoint3d)
    if baseToTipVector3d.isZeroVector() :
        raise ValueError( 'Invalid points for cylinder: base and tip are equal!' )
    baseToTipUnitVector3d = baseToTipVector3d.unit()
    
    if height <= radius*2.0 :
        cdp = Physics.SphereCDP()
        cdp.setCenter( basePoint3d + baseToTipVector3d*0.5 )
        cdp.setRadius( height/2.0 )
    else:
        cdp = Physics.CapsuleCDP()
        cdp.setPoint1( basePoint3d + baseToTipUnitVector3d * radius )
        cdp.setPoint2( tipPoint3d + baseToTipUnitVector3d * -radius )
        cdp.setRadius( radius )
    
    cylinder.addCollisionDetectionPrimitive( cdp )
    
    if withMesh:
        mesh = Mesh.createCylinder( basePoint, tipPoint, radius, colour )
        cylinder.addMesh(mesh)
    
    return cylinder

def _createCone( proxy, axis, basePos, tipPos, radius, colour, moiScale, withMesh ):
    """
    Private function.
    Use createCone() or createCone() instead.
    """
    if axis != 0 and axis != 1 and axis != 2 :
        raise ValueError( 'Axis must be 0 for x, 1 for y or 2 for z.' )

    # Mesh and cdps will be set manually
    proxy.meshes = None
    proxy.cdps = None
    
    # Compute box moi
    moi = [0,0,0]
    height = math.fabs(tipPos-basePos)
    for i in range(3):
        if i == axis :
            moi[i] = proxy.mass*radius*radius * 3.0 / 10.0
        else :
            moi[i] = proxy.mass*(radius*radius/4.0 + height*height) * 3.0 / 5.0
    proxy.moi = PyUtils.toVector3d(moi) * moiScale
     
    cone = proxy.createAndFillObject()
 
    basePoint = [0,0,0]
    tipPoint  = [0,0,0]
    basePoint[axis] = basePos
    tipPoint[axis] = tipPos
    basePoint3d = PyUtils.toPoint3d(basePoint)
    tipPoint3d = PyUtils.toPoint3d(tipPoint)
    baseToTipVector3d = Vector3d(basePoint3d, tipPoint3d)
    if baseToTipVector3d.isZeroVector() :
        raise ValueError( 'Invalid points for cone: base and tip are equal!' )
    baseToTipUnitVector3d = baseToTipVector3d.unit()
    
    if height <= radius*2.0 :
        cdp = Physics.SphereCDP()
        cdp.setCenter( basePoint3d + baseToTipVector3d*0.5 )
        cdp.setRadius( height/2.0 )
    else:
        cdp = Physics.CapsuleCDP()
        cdp.setPoint1( basePoint3d + baseToTipUnitVector3d * radius )
        cdp.setPoint2( tipPoint3d + baseToTipUnitVector3d * -radius )
        cdp.setRadius( radius )
    
    cone.addCollisionDetectionPrimitive( cdp )
    
    if withMesh:
        mesh = Mesh.createCone( basePoint, tipPoint, radius, colour )
        cone.addMesh(mesh)
    
    return cone



