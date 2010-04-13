'''
Created on 2009-10-06

@author: beaudoin
'''

import Scenario, PyUtils, Physics, MathLib, math

class Staircase(Scenario.Scenario):
    """A scenario that can be used to setup a staircase."""
    
    def __init__(self, name = "Staircase scenario", staircaseWidth = 0.9, threadDepth = 0.2 , riserHeight = 0.223, stepCount = 10, position = (0,0,0), angle = 0, leftRampHeight = None, rightRampHeight = None ):
        """Setup a staircase"""
        
        super( Staircase, self ).__init__(name)
        
        self._staircaseWidth = staircaseWidth
        self._threadDepth = threadDepth
        self._riserHeight = riserHeight
        self._stepCount = stepCount
        self._position = position
        self._angle = angle
        self._leftRampHeight = leftRampHeight
        self._rightRampHeight = rightRampHeight
        
        self._loaded = False
        
        
    #
    # Accessors
    #
    def getStairWidth(self):
        """Access the staircase width"""
        return self._staircaseWidth
    
    def setStairWidth(self, staircaseWidth):
        """Sets the staircase width"""
        self._staircaseWidth = staircaseWidth
        
    def getThreadDepth(self):
        """Access the thread depth"""
        return self._staircaseWidth
    
    def setThreadDepth(self, threadDepth):
        """Sets the thread depth"""
        self._threadDepth = threadDepth
        
    def getRiserHeight(self):
        """Access the riser height"""
        return self._riserHeight
    
    def setRiserHeight(self, riserHeight):
        """Sets the riser height"""
        self._riserHeight = riserHeight
        
    def getStepCount(self):
        """Access the number of steps"""
        return self._stepCount
    
    def setStepCount(self, stepCount):
        """Sets the number of steps (a Point3d)"""
        self._stepCount = stepCount
        
    def getPosition(self):
        """
        Access the position. 
        The position is the location on the ground in the middle of the first "virtual step", that
        is, right in front of the first real step.
        """
        return self._position
    
    def setPosition(self, position):
        """
        Sets the position. 
        The position is the location on the ground in the middle of the first "virtual step", that
        is, right in front of the first real step.
        """
        self._position = position
            
    def getAngle(self):
        """Access the angle of rotation along the y axis (in radians)"""
        return self._oritentation
    
    def setAngle(self, angle):
        """Sets the angle of rotation along the y axis (in radians)"""
        self._angle = angle
    
    def getLeftRampHeight(self):
        """Access the height of the left ramp, or None if no left ramp is desired"""
        return self._leftRampHeight
    
    def setLeftRampHeight(self, leftRampHeight):
        """Sets the height of the left ramp, or None if no left ramp is desired"""
        self._leftRampHeight = leftRampHeight
    
    def getRightRampHeight(self):
        """Access the height of the right ramp, or None if no right ramp is desired"""
        return self._rightRampHeight

    def setRightRampHeight(self, rightRampHeight):
        """Sets the height of the right ramp, or None if no right ramp is desired"""
        self._rightRampHeight = rightRampHeight
    
    
    #
    # Public methods
    #
    
    def load(self):
        assert not self._loaded, "Cannot load scenario twice!"
        
        self._loaded = True
        
        # Create the rigid bodies for the main staircase        
        orientation = PyUtils.angleAxisToQuaternion( (self._angle,(0,1,0)) )
        size = MathLib.Vector3d( self._staircaseWidth, self._riserHeight, self._threadDepth )
        pos = PyUtils.toPoint3d( self._position ) + MathLib.Vector3d( 0, -self._riserHeight/2.0, 0 )
        delta = MathLib.Vector3d(size)
        delta.x = 0
        delta = orientation.rotate( delta )
        for i in range(self._stepCount):
            box = PyUtils.RigidBody.createBox( size, pos = pos + delta * (i+1), locked = True, orientation=orientation )
            Physics.world().addRigidBody(box)
        
        # Create the rigid bodies for both ramps
        rampHeights = ( self._leftRampHeight, self._rightRampHeight )
        
        deltaRamp = MathLib.Vector3d(self._staircaseWidth/2.0,0,0)
        deltaRamp = orientation.rotate( deltaRamp )
        deltaRamps = (deltaRamp, deltaRamp * -1)
        for deltaRamp, rampHeight in zip( deltaRamps, rampHeights ):
            if rampHeight is None: continue
            deltaRamp.y = rampHeight/2.0
            box = PyUtils.RigidBody.createBox( (0.02,rampHeight,0.02), pos = pos + deltaRamp + delta , locked = True, orientation=orientation )
            Physics.world().addRigidBody(box)
            box = PyUtils.RigidBody.createBox( (0.02,rampHeight,0.02), pos = pos + deltaRamp + (delta * self._stepCount) , locked = True, orientation=orientation )
            Physics.world().addRigidBody(box)
            deltaRamp.y = rampHeight
            rampOrientation = orientation * PyUtils.angleAxisToQuaternion( (math.atan2(self._riserHeight, self._threadDepth), (-1,0,0)) )
            rampLen = self._stepCount * math.sqrt( self._riserHeight*self._riserHeight + self._threadDepth*self._threadDepth )
            box = PyUtils.RigidBody.createBox( (0.04,0.02,rampLen), pos = pos + deltaRamp + (delta * ((self._stepCount+1) * 0.5)) , locked = True, orientation=rampOrientation )
            Physics.world().addRigidBody(box)
            
        
        
        
        
        
    