'''
Created on 2009-09-03

@author: beaudoin
'''


import Proxy
import Member

import Physics

def getArb(af, arbName):
    if arbName is None : return None
    arb = af.getARBByName( arbName )
    if arb is None: 
        raise KeyError( "Articulated rigid body '"+arbName+"' not found in articulated figure '"+af.getName()+"'." )
    return arb

def setParent(joint, arbName, af):
    joint.setParent( getArb(af,arbName) )

def setChild(joint, arbName, af):
    joint.setChild( getArb(af,arbName) )

def getParent(joint):
    return joint.getParent().getName()

def getChild(joint):
    return joint.getChild().getName()


cls = Physics.Joint
Joint = Proxy.create( cls,
    members = [
        Member.Basic( str, 'name', 'UnnamedJoint', cls.getName, cls.setName ),
        Member.Basic( str, 'parent', None, getParent, setParent ),
        Member.Basic( str, 'child', None, getChild, setChild ),
        Member.Point3d( 'posInParent', (0.0,0.0,0.0), cls.getParentJointPosition, cls.setParentJointPosition ),
        Member.Point3d( 'posInChild', (0.0,0.0,0.0), cls.getChildJointPosition, cls.setChildJointPosition )
    ] )


cls = Physics.BallInSocketJoint
BallInSocketJoint = Proxy.create( cls, parent = Joint, caster = Physics.castToBallInSocketJoint,
    members = [
        Member.Vector3d( 'swingAxis1', (1.0,0.0,0.0), cls.getSwingAxis1, cls.setSwingAxis1 ),
        Member.Vector3d( 'swingAxis2', None, cls.getSwingAxis2, cls.setSwingAxis2 ),
        Member.Vector3d( 'twistAxis', None, cls.getTwistAxis, cls.setTwistAxis ),
        Member.Tuple( 'limits', (-3.1416,3.1416,-3.1416,3.1416,-3.1416,3.1416), None, cls.setJointLimits )
    ] )

cls = Physics.UniversalJoint
UniversalJoint = Proxy.create( cls, parent = Joint, caster = Physics.castToUniversalJoint,
    members = [
        Member.Vector3d( 'parentAxis', (1.0,0.0,0.0), cls.getParentAxis, cls.setParentAxis ),
        Member.Vector3d( 'childAxis', (1.0,0.0,0.0), cls.getChildAxis, cls.setChildAxis ),
        Member.Tuple( 'limits', (-3.1416,3.1416,-3.1416,3.1416), None, cls.setJointLimits )
    ] )

cls = Physics.HingeJoint
HingeJoint = Proxy.create( cls, parent = Joint, caster = Physics.castToHingeJoint,
    members = [
        Member.Vector3d( 'axis', (1.0,0.0,0.0), cls.getAxis, cls.setAxis ),
        Member.Tuple( 'limits', (-3.1416,3.1416), None, cls.setJointLimits )
    ] )

cls = Physics.StiffJoint
StiffJoint = Proxy.create( cls, parent = Joint, caster = Physics.castToStiffJoint )
