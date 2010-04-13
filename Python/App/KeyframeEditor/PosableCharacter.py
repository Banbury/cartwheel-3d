'''
Created on 2009-12-08

@author: beaudoin
'''

import Core
from MathLib import Vector3d, Point3d, Quaternion
import math

class PosableCharacter(object):
    """A character that can be posed and edited in the keyframe editor."""
    
    def __init__(self, character, controller):
        """Initializes and attach to a standard Core.Character and Core.SimBiController."""
        self._character = character
        self._controller = controller
        
        self._leftLowerLeg = self._character.getARBByName('lLowerLeg')
        self._rightLowerLeg = self._character.getARBByName('rLowerLeg')
        self._leftFoot = self._character.getARBByName('lFoot')
        self._rightFoot = self._character.getARBByName('rFoot')
        self._leftHipJointIndex = self._character.getJointIndex( "lHip" )
        self._rightHipJointIndex = self._character.getJointIndex( "rHip" )
        self._leftHipJointIndex = self._character.getJointIndex( "lHip" )
        self._rightHipJointIndex = self._character.getJointIndex( "rHip" )
        self._leftKneeJointIndex = self._character.getJointIndex( "lKnee" )
        self._rightKneeJointIndex = self._character.getJointIndex( "rKnee" )
        self._leftAnkleJointIndex = self._character.getJointIndex( "lAnkle" )
        self._rightAnkleJointIndex = self._character.getJointIndex( "rAnkle" )
        self._leftHipJoint = self._character.getJoint( self._leftHipJointIndex )
        self._rightHipJoint = self._character.getJoint( self._rightHipJointIndex )
        self._leftHipJoint = self._character.getJoint( self._leftHipJointIndex )
        self._rightHipJoint = self._character.getJoint( self._rightHipJointIndex )
        self._leftKneeJoint = self._character.getJoint( self._leftKneeJointIndex )
        self._rightKneeJoint = self._character.getJoint( self._rightKneeJointIndex )
        self._leftAnkle = self._character.getJoint( self._leftAnkleJointIndex  )
        self._rightAnkle = self._character.getJoint( self._rightAnkleJointIndex )
        self._pelvis = self._leftHipJoint.getParent() 
        self._leftUpperLeg = self._leftHipJoint.getChild()
        self._rightUpperLeg = self._rightHipJoint.getChild()
        
        self._leftAnkleInStanceLowerLeg = self._leftAnkle.getParentJointPosition()
        self._rightAnkleInStanceLowerLeg = self._rightAnkle.getParentJointPosition()
        self._leftHipJointInPelvis = self._leftHipJoint.getParentJointPosition()
        self._rightHipJointInPelvis = self._rightHipJoint.getParentJointPosition()
        self._leftHipJointInPelvis = self._leftHipJoint.getParentJointPosition()
        self._rightHipJointInPelvis = self._rightHipJoint.getParentJointPosition()
        self._leftHipToKneeVectorInUpperLeg = Vector3d( self._leftHipJoint.getChildJointPosition(), 
                                                    self._leftKneeJoint.getParentJointPosition() )
        self._rightHipToKneeVectorInUpperLeg = Vector3d( self._rightHipJoint.getChildJointPosition(), 
                                                    self._rightKneeJoint.getParentJointPosition() )
        self._leftKneeToAnkleVectorInLowerLeg = Vector3d( self._leftKneeJoint.getChildJointPosition(), 
                                                      self._leftAnkle.getParentJointPosition() )
        self._rightKneeToAnkleVectorInLowerLeg = Vector3d( self._rightKneeJoint.getChildJointPosition(), 
                                                      self._rightAnkle.getParentJointPosition() )
        
    def getCharacter(self):
        """Access the character."""
        return self._character
        
    def updatePose(self, time, stanceFootToSwingFoot, stance = Core.LEFT_STANCE, dontMoveStanceAnkle = False):
        """Updates the pose of the character to match the one at the specified time.
        Always use left stance.
        stanceFootToSwingFoot is a Vector3d for the vector linking the stance foot to the swing foot
        """        
        
        # Setup the stance leg
        if stance == Core.LEFT_STANCE:
            stanceLowerLeg = self._leftLowerLeg
            stanceAnkleInStanceLowerLeg = self._leftAnkleInStanceLowerLeg
            stanceHipJointInPelvis = self._leftHipJointInPelvis
            stanceHipJointIndex = self._leftHipJointIndex
            swingHipJointInPelvis = self._rightHipJointInPelvis
            swingHipToKneeVectorInUpperLeg = self._rightHipToKneeVectorInUpperLeg
            swingKneeToAnkleVectorInLowerLeg = self._rightKneeToAnkleVectorInLowerLeg
            swingKneeJointIndex = self._rightKneeJointIndex
            swingHipJointIndex = self._rightHipJointIndex
        else:
            stanceLowerLeg = self._rightLowerLeg
            stanceAnkleInStanceLowerLeg = self._rightAnkleInStanceLowerLeg
            stanceHipJointInPelvis = self._rightHipJointInPelvis
            stanceHipJointIndex = self._rightHipJointIndex
            swingHipJointInPelvis = self._leftHipJointInPelvis
            swingHipToKneeVectorInUpperLeg = self._leftHipToKneeVectorInUpperLeg
            swingKneeToAnkleVectorInLowerLeg = self._leftKneeToAnkleVectorInLowerLeg
            swingKneeJointIndex = self._leftKneeJointIndex
            swingHipJointIndex = self._leftHipJointIndex
            
            
        if dontMoveStanceAnkle :
            finalStanceAnkleInWorld  = stanceLowerLeg.getWorldCoordinates( stanceAnkleInStanceLowerLeg )
        
        pose = Core.ReducedCharacterStateArray()
        self._controller.updateTrackingPose(pose, time, stance)
        reducedCharacter = Core.ReducedCharacterState(pose)

        # Update stanceFootToSwingFoot, adding in the delta
        stanceFootToSwingFoot += self._controller.computeSwingFootDelta(time, stance)

        # Recenter and reorient the character
        pos = reducedCharacter.getPosition()
        pos.x = pos.z = 0
        reducedCharacter.setPosition(pos)
        reducedCharacter.setOrientation(Quaternion())        
        
        self._character.setState(pose, 0, False)
        
        leftFootWorldOrientation = self._leftFoot.getOrientation() 
        rightFootWorldOrientation = self._rightFoot.getOrientation() 


        currentStanceAnkleInWorld  = stanceLowerLeg.getWorldCoordinates( stanceAnkleInStanceLowerLeg )
        currentStanceAnkleInPelvis = self._pelvis.getLocalCoordinates( currentStanceAnkleInWorld )
        currentStanceHipToAnkleInPelvis = Vector3d( stanceHipJointInPelvis, currentStanceAnkleInPelvis )
        lengthStanceHipToAnkle = currentStanceHipToAnkleInPelvis.length()

        stanceHipJointInWorld = self._pelvis.getWorldCoordinates(stanceHipJointInPelvis)
        
        desiredStanceAnkleInWorld  = Point3d(stanceFootToSwingFoot*-0.5)
        
        yLentgh2 = lengthStanceHipToAnkle*lengthStanceHipToAnkle - \
                    desiredStanceAnkleInWorld.x*desiredStanceAnkleInWorld.x - \
                    desiredStanceAnkleInWorld.z*desiredStanceAnkleInWorld.z
        
        if yLentgh2 <= 0:
            desiredStanceAnkleInWorld.y = stanceHipJointInWorld.y 
        else:
            desiredStanceAnkleInWorld.y = stanceHipJointInWorld.y - math.sqrt( yLentgh2 )
        
        desiredStanceAnkleInPelvis = self._pelvis.getLocalCoordinates( desiredStanceAnkleInWorld )        
        desiredStanceHipToAnkleInPelvis = Vector3d( stanceHipJointInPelvis, desiredStanceAnkleInPelvis )
        
        currentStanceHipToAnkleInPelvis.toUnit()
        desiredStanceHipToAnkleInPelvis.toUnit()
        rot = Quaternion( currentStanceHipToAnkleInPelvis, desiredStanceHipToAnkleInPelvis )
        
        currQuat = reducedCharacter.getJointRelativeOrientation(stanceHipJointIndex)
        currQuat *= rot
        reducedCharacter.setJointRelativeOrientation(currQuat, stanceHipJointIndex)
        
        pos = reducedCharacter.getPosition()
        pos.y -= desiredStanceAnkleInWorld.y
        reducedCharacter.setPosition(pos)
                
        self._character.setState(pose, 0, False)
        
        # Setup the swing leg
        
        currentStanceAnkleInWorld  = stanceLowerLeg.getWorldCoordinates( stanceAnkleInStanceLowerLeg )
        desiredSwingAnkleInWorld  = currentStanceAnkleInWorld + stanceFootToSwingFoot
        targetInPelvis = self._pelvis.getLocalCoordinates( desiredSwingAnkleInWorld )

        qParent = Quaternion()
        qChild = Quaternion()

        Core.TwoLinkIK_getIKOrientations(
                swingHipJointInPelvis, 
                targetInPelvis, 
                Vector3d(-1,0,0), swingHipToKneeVectorInUpperLeg,
                Vector3d(-1,0,0), swingKneeToAnkleVectorInLowerLeg, qParent, qChild)


        reducedCharacter.setJointRelativeOrientation(qChild, swingKneeJointIndex)
        reducedCharacter.setJointRelativeOrientation(qParent, swingHipJointIndex)

        self._character.setState(pose,0,False)

        leftAnkleLocalOrientation =  self._leftLowerLeg.getOrientation().getInverse() * leftFootWorldOrientation  
        rightAnkleLocalOrientation = self._rightLowerLeg.getOrientation().getInverse() * rightFootWorldOrientation 

        reducedCharacter.setJointRelativeOrientation(leftAnkleLocalOrientation, self._leftAnkleJointIndex)
        reducedCharacter.setJointRelativeOrientation(rightAnkleLocalOrientation, self._rightAnkleJointIndex)

        if dontMoveStanceAnkle :
            delta = finalStanceAnkleInWorld - stanceLowerLeg.getWorldCoordinates( stanceAnkleInStanceLowerLeg )
            pos = reducedCharacter.getPosition() + delta
            reducedCharacter.setPosition(pos)
            

        self._character.setState(pose,0,False)
        
        