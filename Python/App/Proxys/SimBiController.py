'''
Created on 2009-09-03

@author: beaudoin
'''

import Proxy
import Member
from PyUtils import enum
import Controllers

import wx, Core

def getControlParams(controller, i):
    if i == 0 :
        return controller.getRootControlParams()
    else :
        return controller.getControlParams(i-1)

def getControlParamsCount(controller):
    return controller.getControlParamsCount() + 1

cls = Core.SimBiController
SimBiController = Proxy.create( cls, caster = Core.castToSimBiController, loader = wx.GetApp().addController, 
    nameGetter = cls.getName,
    icon = "../data/ui/controllerIcon.png",
    members = [
        Member.Basic( str, 'name', 'UnnamedSimBiController', cls.getName, cls.setName ),
        Member.Basic( int, 'startingState', 0, cls.getStartingState, cls.setStartingState ),
        Member.Basic( float, 'stanceHipDamping', 25, cls.getStanceHipDamping, cls.setStanceHipDamping ),
        Member.Basic( float, 'stanceHipMaxVelocity', 4, cls.getStanceHipMaxVelocity, cls.setStanceHipMaxVelocity ),
        Member.ObjectList( 'controlParamsList', None, getControlParams, getControlParamsCount, cls.addControlParams ),
        Member.ObjectList( 'states', None, cls.getState, cls.getStateCount, cls.addState, embedInParentNode = True ) 
    ] )

def setJoint(joint, jointName, controller):
    if not joint.setJoint( controller.getCharacter().getJointByName(jointName) ):
        raise ValueError( "Setting the wrong joint." )
    
cls = Core.ControlParams
ControlParams = Proxy.create( cls, caster = Core.castToControlParams,
    nameGetter = cls.getJointName,
    icon = "../data/ui/controlParamsIcon.png",
    members = [
        Member.Basic( str, 'joint', 'noJointName', cls.getJointName, setJoint, editable = False ),
        Member.Basic( float, 'kp', None, cls.getKp, cls.setKp ),
        Member.Basic( float, 'kd', None, cls.getKd, cls.setKd ),
        Member.Basic( float, 'tauMax', 1000.0, cls.getMaxAbsTorque, cls.setMaxAbsTorque ),
        Member.Vector3d( 'scale', (1.0,1.0,1.0), cls.getScale, cls.setScale )
    ] )


TransitionOn    = enum( 'FOOT_CONTACT', 'TIME_UP' )
TO_FOOT_CONTACT = TransitionOn.toInt( 'FOOT_CONTACT' )
TO_TIME_UP      = TransitionOn.toInt( 'TIME_UP' )

Stance = enum( { 'LEFT'    : Core.SimBiConState.STATE_LEFT_STANCE ,
                 'RIGHT'   : Core.SimBiConState.STATE_RIGHT_STANCE, 
                 'REVERSE' : Core.SimBiConState.STATE_REVERSE_STANCE, 
                 'KEEP'    : Core.SimBiConState.STATE_KEEP_STANCE } )

def getTransitionOn(state):
    if state.getTransitionOnFootContact(): return TO_FOOT_CONTACT
    else: return TO_TIME_UP

def setTransitionOn(state, value):
    if value == TO_FOOT_CONTACT : state.setTransitionOnFootContact(True)
    elif value == TO_TIME_UP : state.setTransitionOnFootContact(False)
    else: raise ValueError( "SimBiConState member 'transitionOn' has invalid value: '" + str(value) + "'" )

cls = Core.SimBiConState
SimBiConState = Proxy.create( cls, caster = Core.castToSimBiConState, 
    nameGetter = cls.getName,
    icon = "../data/ui/fsmstateIcon.png",
    members = [
        Member.Basic( str, 'name', 'UnnamedSimBiConState', cls.getName, cls.setName ),
        Member.Basic( int, 'nextStateIndex', None, cls.getNextStateIndex, cls.setNextStateIndex ),
        Member.Enum( TransitionOn, 'transitionOn', TO_FOOT_CONTACT, getTransitionOn, setTransitionOn ),
        Member.Enum( Stance, 'stance', Core.SimBiConState.STATE_REVERSE_STANCE, cls.getStance, cls.setStance ),
        Member.Basic( float, 'duration', 0.5, cls.getDuration, cls.setDuration ),
        Member.Trajectory1d( 'dTrajX', None, cls.getDTrajX, cls.setDTrajX ),
        Member.Trajectory1d( 'dTrajZ', None, cls.getDTrajZ, cls.setDTrajZ ),
        Member.Trajectory1d( 'vTrajX', None, cls.getVTrajX, cls.setVTrajX ),
        Member.Trajectory1d( 'vTrajZ', None, cls.getVTrajZ, cls.setVTrajZ ),
        Member.ObjectList( 'externalForces', None, cls.getExternalForce, cls.getExternalForceCount, cls.addExternalForce ),
        Member.ObjectList( 'trajectories', None, cls.getTrajectory, cls.getTrajectoryCount, cls.addTrajectory, embedInParentNode = True ) 
    ] )

cls = Core.ExternalForce
ExternalForce = Proxy.create( cls, caster = Core.castToExternalForce, 
    nameGetter = cls.getBodyName,
    icon = "../data/ui/externalForceIcon.png",
    members = [
        Member.Basic( str, 'body', 'UnnamedExternalForce', cls.getBodyName, cls.setBodyName ),
        Member.Trajectory1d( 'forceX', None, cls.getForceX, cls.setForceX ),
        Member.Trajectory1d( 'forceY', None, cls.getForceY, cls.setForceY ),
        Member.Trajectory1d( 'forceZ', None, cls.getForceZ, cls.setForceZ ),
        Member.Trajectory1d( 'torqueX', None, cls.getTorqueX, cls.setTorqueX ),
        Member.Trajectory1d( 'torqueY', None, cls.getTorqueY, cls.setTorqueY ),
        Member.Trajectory1d( 'torqueZ', None, cls.getTorqueZ, cls.setTorqueZ )
    ] )

ReferenceFrame = enum( 'PARENT_RELATIVE', 'CHARACTER_RELATIVE' )
RF_PARENT_RELATIVE    = ReferenceFrame.toInt( 'PARENT_RELATIVE' )
RF_CHARACTER_RELATIVE = ReferenceFrame.toInt( 'CHARACTER_RELATIVE' )

def getReferenceFrame(trajectory):
    if trajectory.isRelativeToCharacterFrame(): return RF_CHARACTER_RELATIVE
    else: return RF_PARENT_RELATIVE

def setReferenceFrame(trajectory, value):
    if value == RF_PARENT_RELATIVE : trajectory.setRelativeToCharacterFrame(False)
    elif value == RF_CHARACTER_RELATIVE : trajectory.setRelativeToCharacterFrame(True)
    else: raise ValueError( "SimBiConTrajectory member 'referenceFrame' has invalid value: '" + str(value) + "'" )

cls = Core.Trajectory
Trajectory = Proxy.create( cls, caster = Core.castToTrajectory, 
    nameGetter = cls.getJointName,
    icon = "../data/ui/trajectoryIcon.png",
    members = [
        Member.Basic( str, 'joint', 'UnnamedTrajectory', cls.getJointName, cls.setJointName ),
        Member.Trajectory1d( 'strength', None, cls.getStrengthTrajectory, cls.setStrengthTrajectory ),
        Member.Enum( ReferenceFrame, 'referenceFrame', RF_PARENT_RELATIVE, getReferenceFrame, setReferenceFrame ),
        Member.ObjectList( 'components', None, cls.getTrajectoryComponent, cls.getTrajectoryComponentCount, cls.addTrajectoryComponent, embedInParentNode = True ) 
    ] )

ReverseOnStance = enum( { 'LEFT'    : Core.TrajectoryComponent.ROS_LEFT ,
                          'RIGHT'   : Core.TrajectoryComponent.ROS_RIGHT, 
                          'DONT_REVERSE' : Core.TrajectoryComponent.ROS_DONT_REVERSE } )

def getTrajectoryComponentName(trajectoryComponent):
    """Called whenever the trajectory component is updated."""
    axis = trajectoryComponent.getRotationAxis()
    if( axis.x != 0 and axis.y == 0 and axis.z == 0 ) :
        return "X axis"
    elif( axis.x == 0 and axis.y != 0 and axis.z == 0 ) :
        return "Y axis"
    elif( axis.x == 0 and axis.y == 0 and axis.z != 0 ) :
        return "Z axis"
    else :
        return "Axis (%f,%f,%f)" % ( axis.x, axis.y, axis.z )

cls = Core.TrajectoryComponent
TrajectoryComponent = Proxy.create( cls, caster = Core.castToTrajectoryComponent, 
    nameGetter = getTrajectoryComponentName,
    icon = "../data/ui/trajectoryComponentIcon.png",
    members = [
        Member.Vector3d( 'rotationAxis', None, cls.getRotationAxis, cls.setRotationAxis ),
        Member.Enum( ReverseOnStance, 'reverseOnStance', Core.TrajectoryComponent.ROS_DONT_REVERSE, cls.getReverseOnStance, cls.setReverseOnStance),
        Member.Object( 'feedback', None, cls.getFeedback, cls.setFeedback ),
        Member.Trajectory1d( 'baseTrajectory', None, cls.getBaseTrajectory, cls.setBaseTrajectory ),
        Member.Trajectory1d( 'dScaledTrajectory', None, cls.getDTrajScale, cls.setDTrajScale ),
        Member.Trajectory1d( 'vScaledTrajectory', None, cls.getVTrajScale, cls.setVTrajScale ) 
    ] )

def getDLimits(object):
    return (object.getDMin(), object.getDMax())

def setDLimits(object, value):
    return object.setDLimits(*value)

def getVLimits(object):
    return (object.getVMin(), object.getVMax())

def setVLimits(object, value):
    return object.setVLimits(*value)

cls = Core.LinearBalanceFeedback
LinearBalanceFeedback = Proxy.create( cls, caster = Core.castToLinearBalanceFeedback,
    icon = "../data/ui/feedbackIcon.png",
    members = [
        Member.Vector3d( 'axis', None, cls.getProjectionAxis, cls.setProjectionAxis ),
        Member.Basic( float, 'cd', 0, cls.getCd, cls.setCd ),
        Member.Basic( float, 'cv', 0, cls.getCv, cls.setCv ),
        Member.Basic( tuple, 'dLimits', (-1000,1000), getDLimits, setDLimits ),
        Member.Basic( tuple, 'vLimits', (-1000,1000), getVLimits, setVLimits )
    ] )
    
    
cls = Core.IKVMCController
IKVMCController = Proxy.create( cls,  parent = SimBiController, caster = Core.castToIKVMCController, loader = wx.GetApp().addController,
    members = [
        Member.Trajectory1d( 'sagittalTrajectory',  None, cls.getSwingFootTrajectoryDeltaSagittal, cls.setSwingFootTrajectoryDeltaSagittal ),
        Member.Trajectory1d( 'coronalTrajectory', None, cls.getSwingFootTrajectoryDeltaCoronal, cls.setSwingFootTrajectoryDeltaCoronal ),
        Member.Trajectory1d( 'heightTrajectory', None, cls.getSwingFootTrajectoryDeltaHeight, cls.setSwingFootTrajectoryDeltaHeight ) 
    ] )

cls = Controllers.DanceController
DanceController = Proxy.create( cls,  parent = IKVMCController, loader = wx.GetApp().addController ) 

cls = Controllers.WalkController
WalkController = Proxy.create( cls,  parent = IKVMCController, loader = wx.GetApp().addController ) 
