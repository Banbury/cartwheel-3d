'''
Created on 2009-12-02

@author: beaudoin
'''

import math, MathLib
        

class BaseHandle(object):
    
    def __init__( self, trajectory, oppositeTrajectory = None, reverseOppositeJoint = False, minValue = -1000, maxValue = 1000 ):
        """
        Creates a handle that can be manipulated and is linked to a trajectory
        """
        
        self._trajectory = trajectory
        self._oppositeTrajectory = oppositeTrajectory

        if reverseOppositeJoint:
            self._oppositeSign = -1
        else:
            self._oppositeSign = 1

        self._minValue = minValue
        self._maxValue = maxValue
        
    def forceKeyframesAt(self,forcedTimeArray,allowedTimeArray):
        """Make sure keyframes are found only at specific times.
        The function first create keyframes at every time found in forcedTimeArray. 
        Then it deletes any keyframe that does not fall in allowedTimeArray.
        In general all the times in forcedTimeArray should also be found in allowedTimeArray.
        The resulting curve will be an approximation of the current curve."""
        
        values = []
        for time in forcedTimeArray:
            values.append( (time, self._trajectory.evaluate_catmull_rom(time)) )
        for time in allowedTimeArray:
            if forcedTimeArray.count(time) == 0:
                index = self.getIndexForTime(time)
                if index is not None:
                    values.append( (time, self._trajectory.getKnotValue(index)) )
            
        self._trajectory.clear()
        for time, value in values:
            self._trajectory.addKnot(time, value)
            
    def addKeyframeAt(self,time):
        """Adds a single keyframe at the specified time. The curve might be slightly modified as a result."""        
        self._trajectory.addKnot(time, self._trajectory.evaluate_catmull_rom(time))

    def getIndexForTime(self, time):
        """Gets the index for the keyframe at the specified time. None if no keyframe found at that time."""
        for i in range( self._trajectory.getKnotCount() ):
            if math.fabs( time - self._trajectory.getKnotPosition(i) ) < 0.00001  :
                return i
        return None

    def removeKeyframe(self, index):
        """Removes the keyframe at the specified index."""        
        self._trajectory.removeKnot(index)

    def hasKeyframeAtTime(self, time):
        """Checks if this handle has a keyframe at the specified time."""
        return self.getIndexForTime(time) != None
                
    def getKeyframeValue(self, index):
        """Returns the keyframe value at the specified index"""
        return self._trajectory.getKnotValue(index)
 
    def enforceSymmetry(self):
        """Make sure the beginning of the trajectory matches the end (of the other stance)."""
        self.setKeyframeValue(0, self.getKeyframeValue(0))

    def setKeyframeValue(self, index, value):
        """Changes the value of the keyframe at the specified index."""
        value = self.clampValue(value)
        self._trajectory.setKnotValue(index, value)        

        # This forces the symmetry
        lastIndex = self._trajectory.getKnotCount()-1
        try:
            otherLastIndex = self._oppositeTrajectory.getKnotCount()-1
            if lastIndex != otherLastIndex or self._trajectory.getKnotPosition(lastIndex) != self._oppositeTrajectory.getKnotPosition(lastIndex) :
                return 
            if index == 0 : 
                self._oppositeTrajectory.setKnotValue(lastIndex, value * self._oppositeSign )
            elif index == lastIndex :
                self._oppositeTrajectory.setKnotValue(0, value * self._oppositeSign )
        except AttributeError:
            if index == 0 :
                self._trajectory.setKnotValue(lastIndex, value * self._oppositeSign )
            elif index == lastIndex :
                self._trajectory.setKnotValue(0, value * self._oppositeSign )        
        
        
    def clampValue(self, value):
        """Return the value clamped to lie between supported extreme values."""
        if value < self._minValue: return self._minValue
        if value > self._maxValue: return self._maxValue
        return value
        
class Handle(BaseHandle):
    
    def __init__( self, controller, jointName, componentIndex, type='unknown', posInChild=MathLib.Point3d(), oppositeJointName = None, reverseOppositeJoint = False, minValue = -1000, maxValue = 1000 ):
        """
        Creates a handle that can be used to access a specific component in a controller in a standard (direct) way.
        type should be 'circular', 'reverseCircular', 'perpendicular', 'reversePerpendicular' or 'unknown' to indicate how the handle behaves 
        posInChild should be of type MathLib.Point3d
        reverse should be True if the handle works in a reverse way (i.e. going clockwise increase the angle (?))
        oppositeJointName should be the name of the corresponding joint on the other stance.
        reverseOppositeJoint should be True if the opposite joint sign is different
        """
                
        self._controller = controller
        self._jointName  = jointName
        trajectory = controller.getState(0).getTrajectory(jointName).getTrajectoryComponent(componentIndex).getBaseTrajectory()
        
        self._oppositeJointName  = oppositeJointName
        self._posInChild = posInChild
        if oppositeJointName is not None:
            oppositeTrajectory = controller.getState(0).getTrajectory(oppositeJointName).getTrajectoryComponent(componentIndex).getBaseTrajectory()
        else:
            oppositeTrajectory = None
        self._type = type

        super(Handle,self).__init__(trajectory, oppositeTrajectory, reverseOppositeJoint, minValue, maxValue)
        

    def getController(self):
        """Gets the controller associated with that handle."""
        return self._controller
        
    def getJointName(self):
        """Return the joint name for this handle."""
        return self._jointName
    
    def getType(self):
        """Return the type desired for this handle."""
        return self._type
        
    def getPosInChild(self):
        """Return the position of the handle in child coordinate."""
        return self._posInChild
    
                