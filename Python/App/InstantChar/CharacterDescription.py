'''
Created on 2009-11-20

@author: beaudoin
'''

import Core, Physics, PyUtils
from MathLib import Vector3d, Point3d

class CharacterDescription(object):
    """A simple description of a character
    Be careful: 
       Left refer to the left-hand-side of the character (which is seen to the right from the front view)
       So Right goes towards negative X, while Left goes towards positive X       
    """
    
    def getNativeVar(self, varName):
        """Access a native variable by name."""
        return self.__getattribute__(varName).get()
    
    def getNativeVarSymmetric(self, varName, side):
        """Access a symmetric native variable by name and side."""
        return self.__getattribute__(varName).getSide(side)
    
    def setNativeVar(self, varName, value):
        """Sets a native variable by name."""
        variable = self.__getattribute__(varName)
        if variable.get() == value : return
        self._indirectVarsAreValid = False
        variable.set(value)
    
    def setNativeVarSymmetric(self, varName, side, value):
        """Sets a symmetric native variable by name and side."""
        variable = self.__getattribute__(varName)
        if variable.getSide(side) == value : return
        if self._isSymmetric : side = 0
        self._indirectVarsAreValid = False
        variable.setSide( side, value )
    
    def getIndirectVar(self, varName):
        """Access an indirect variable by name."""
        if not self._indirectVarsAreValid : self._computeIndirectVars()
        return self.__getattribute__(varName).get()
    
    def getIndirectVarSymmetric(self, varName, side):
        """Access a symmetric indirect variable by name and side."""
        if not self._indirectVarsAreValid : self._computeIndirectVars()
        return self.__getattribute__(varName).getSide(side)
    
    def _addNativeVar(self, varName, initialValue):
        """Add a variable with he given name, as well as its getter and setter."""
        if isinstance(initialValue, _Symmetric) :
            self.__setattr__( PyUtils.getterName(varName), lambda side: self.getNativeVarSymmetric(varName, side) )
            self.__setattr__( PyUtils.setterName(varName), lambda side, value: self.setNativeVarSymmetric(varName, side, value) )
        else :
            self.__setattr__( PyUtils.getterName(varName), lambda: self.getNativeVar(varName) )
            self.__setattr__( PyUtils.setterName(varName), lambda value: self.setNativeVar(varName, value) )
        self.__setattr__( varName, initialValue )

    def _addIndirectVar(self, varName, initialValue):
        """Add an indirect variable with the given name, as well as its getter and setter."""
        if isinstance(initialValue, _Symmetric) :
            self.__setattr__( PyUtils.getterName(varName), lambda side: self.getIndirectVarSymmetric(varName, side) )
        else:
            self.__setattr__( PyUtils.getterName(varName), lambda: self.getIndirectVar(varName) )
        self._indirectVarsAreValid = None
        self.__setattr__(varName, initialValue)
    
    def __init__(self):
        
        super(CharacterDescription,self).__init__()
        
        # Roughly based on http://www.idrawdigital.com/wp-content/uploads/2009/01/prop_male.gif
        # 1 head unit = 0.2286 meters
        hu = 0.2286

        self._isSymmetric = True
        self._indirectVarsAreValid = False

        self._natives = {                
            '_footSizeX' : _Symmetric( 0.45 * hu, 0.6*hu, hu ),
            '_footSizeZ' : _Symmetric(1 * hu, 0.1*hu ),
            '_ankleRelativePosY' : _Symmetric(0.05, 0.05, 0.3 ),
            '_lowerLegDiameter' : _Symmetric( 0.33 * hu, 0.2*hu, hu  ),
            '_upperLegDiameter' : _Symmetric( 0.4 * hu, 0.2*hu, hu ),
            '_legSizeY' : _Value( 4*hu, 2*hu, 6*hu ),
            '_kneeRelativePosY' : _Symmetric( 0.52, 0.2, 0.8 ),
            '_legRelativeAnchorX' : _Symmetric( 0.6, 0.2, 0.8 ),
            '_pelvisDiameter' : _Value( 1.1 * hu, 0.1*hu, 2.5*hu ),
            '_torsoDiameter' : _Value( 1.4 * hu, hu, 2.6*hu ),
            '_trunkSizeY' : _Value( 2.66 * hu, 1.8*hu, 3.5*hu ),
            '_waistRelativePosY' : _Value( 0.17, 0.1, 0.4 ),
            '_chestRelativePosY' : _Value( 0.5, 0.2, 0.8 ),
            '_neckSizeY' : _Value( 0.05 * hu, 0.01*hu, 2*hu ),
            '_headSizeX' : _Value( 0.9 * hu, 0.1*hu, 2*hu ), 
            '_headSizeY' : _Value( 1.1 * hu, 0.1*hu, 2*hu ), 
            '_headSizeZ' : _Value( 1.0 * hu, 0.1*hu, 2*hu ),
            
            '_upperArmDiameter' : _Symmetric( 0.35 * hu, 0.2*hu, hu ), 
            '_lowerArmDiameter' : _Symmetric( 0.28 * hu, 0.2*hu, hu ), 
            '_armSizeY' : _Symmetric( 2.7 * hu, 1*hu, 5*hu ), 
            '_elbowRelativePosY' : _Symmetric( 0.44444444, 0.01, 0.99 ) }
        
        for var, val in self._natives.iteritems() :
            self._addNativeVar( var, val )        
        
        self._indirects = {
            '_legPosX' : _Symmetric(0),
            '_groundPosY' : _Value(0),
            '_anklePosY' : _Symmetric(0),
            '_kneePosY' : _Symmetric(0),
            '_hipPosY' : _Value(0),
            '_waistPosY' : _Value(0),
            '_chestPosY' : _Value(0),
            '_shoulderPosY' : _Value(0),
            '_neckPosY' : _Value(0),
            '_armPosX' : _Symmetric(0),
            '_elbowPosY' : _Symmetric(0),
            '_wristPosY' : _Symmetric(0)
        }
        
        for var, val in self._indirects.iteritems() :
            self._addIndirectVar( var, val )        

    def _computeLegPosX(self, side):
        self._legPosX.setSide( side, side*self._pelvisDiameter.get()/2.0*self._legRelativeAnchorX.getSide(side))

    def setLegPosX(self, side, value):
        self.setLegRelativeAnchorX( side, value/self._pelvisDiameter.get()*2.0*side )

        
    def _computeAnklePosY(self, side):
        self._anklePosY.setSide( side, self._groundPosY.get() + self._legSizeY.get() * self._ankleRelativePosY.getSide(side) )
        
    def setAnklePosY(self, side, value):
        self.setAnkleRelativePosY( side, (value - self._groundPosY.get())/self._legSizeY.get() )

    def _computeKneePosY(self, side):
        self._kneePosY.setSide( side, self._anklePosY.getSide(side) + (self._hipPosY.get()-self._anklePosY.getSide(side)) * self._kneeRelativePosY.getSide(side) )

    def setKneePosY(self, side, value):
        self.setKneeRelativePosY( side, (value - self._anklePosY.getSide(side)) / (self._hipPosY.get()-self._anklePosY.getSide(side)) )

    def _computeHipPosY(self):
        self._hipPosY.set( self._groundPosY.get() + self._legSizeY.get() )
        
    def setHipPosY(self, value):
        self.setLegSizeY( value - self._groundPosY.get() )

    def _computeShoulderPosY(self):
        self._shoulderPosY.set( self._hipPosY.get() + self._trunkSizeY.get() )

    def setShoulderPosY(self, value):
        self.setTrunkSizeY( value - self._hipPosY.get() )

    def _computeWaistPosY(self):
        self._waistPosY.set( self._hipPosY.get() + self._trunkSizeY.get() * self._waistRelativePosY.get() )

    def setWaistPosY(self, value):
        self.setWaistRelativePosY( (value - self._hipPosY.get())/self._trunkSizeY.get() )

    def _computeChestPosY(self):
        self._chestPosY.set( self._waistPosY.get() + (self._shoulderPosY.get()-self._waistPosY.get()) * self._chestRelativePosY.get() )

    def setChestPosY(self, value):
        self.setChestRelativePosY( (value - self._waistPosY.get())/(self._shoulderPosY.get()-self._waistPosY.get()) )

    def _computeNeckPosY(self):
        self._neckPosY.set( self._shoulderPosY.get() + self._neckSizeY.get() )

    def setNeckPosY(self, value):
        self.setNeckSizeY( value - self._shoulderPosY.get() )

    def _computeArmPosX(self, side):
        self._armPosX.setSide(side, side*(self._torsoDiameter.get() + self._upperArmDiameter.getSide(side))/2.0)

    def _computeWristPosY(self, side):
        self._wristPosY.setSide(side, self._shoulderPosY.get() - self._armSizeY.getSide(side))

    def setWristPosY(self, side, value):
        self.setArmSizeY( side, self._shoulderPosY.get() - value )

    def _computeElbowPosY(self, side):
        self._elbowPosY.setSide(side, self._shoulderPosY.get() - self._armSizeY.getSide(side)*self._elbowRelativePosY.getSide(side))

    def setElbowPosY(self, side, value):
        self.setElbowRelativePosY( side, (self._shoulderPosY.get() - value)/self._armSizeY.getSide(side) )


    def _computeIndirectVars(self):
        """Compute all the variables that are not directly part of the editable character description."""

        if self._indirectVarsAreValid : return

        # Careful! The order in which the _compute functions are called is important!
        
        self._groundPosY.set( 0 )
        self._hipPosY.set( self._groundPosY.get() + self._legSizeY.get() )
        self._computeHipPosY()
        self._computeShoulderPosY()
        self._computeWaistPosY()
        self._computeChestPosY()
        self._computeNeckPosY()
        
        for side in (-1,1) :

            self._computeLegPosX( side )
        
            self._computeAnklePosY(side)
            self._computeKneePosY(side)
            
            self._computeArmPosX(side)
            self._computeWristPosY(side)
            self._computeElbowPosY(side)
        
        self._indirectVarsAreValid = True

    def isSymmetric(self):
        """True if the character should be symmetrical."""
        return self._isSymmetric

    def setSymmetric(self, value):
        """Indicates whether the character should be symmetrical."""
        if value == self._isSymmetric : return
        self._isSymmetric = value
        for val in self._natives.itervalues() :
            try: val.forceSymmetric()
            except AttributeError: pass  
            self._indirectVarsAreValid = False

                
    def createCharacter(self):
        """Creates a 3D character that follows this description."""
        from App import Proxys

        blue  = ( 0.5, 0.6, 0.8, 1 )
        green = ( 0.5, 0.8, 0.6, 1 )
        red   = ( 0.892, 0.716, 0.602, 1 )
        gray  = ( 0.5, 0.5, 0.5, 1 )
        
        character = Core.Character()
        
        character.setName("Instant Character")
        
        massScale = 900
        
        pelvisSizeY = self.getWaistPosY() - self.getHipPosY()
        pelvisBottomPos = -pelvisSizeY/2.0-self.getLegSizeY()*0.1
        pelvisTopPos = pelvisSizeY/2.0 
        pelvisRadius = self.getPelvisDiameter()/2.0
        rootPosY = self.getHipPosY() + pelvisSizeY/2.0 + 0.007
        pelvis = PyUtils.RigidBody.createArticulatedTaperedBox( 
            name = "pelvis", 
            size = (pelvisRadius*2.0, pelvisSizeY*1.5, pelvisRadius*1.2),
            moiScale = 3,
            exponentBottom = 2,
            exponentSide = 2,
            mass = -massScale, pos=(0, rootPosY, 0), colour = blue )
        character.setRoot( pelvis )
    
        totalLowerBackSizeY = self.getChestPosY() - self.getWaistPosY()
        lowerBackOffsetY = 0 #0.15 * totalLowerBackSizeY        
        lowerBackSizeX = self.getTorsoDiameter() * 0.7
        lowerBackSizeY = totalLowerBackSizeY - lowerBackOffsetY
        lowerBackSizeZ = lowerBackSizeX * 0.7
        lowerback = PyUtils.RigidBody.createArticulatedTaperedBox( 
            name = "lowerBack", 
            size = (lowerBackSizeX, lowerBackSizeY, lowerBackSizeZ),
            exponentTop = 3, exponentBottom = 2, exponentSide = 2,
            mass = -massScale, colour = green )
        character.addArticulatedRigidBody( lowerback )

        joint = Proxys.BallInSocketJoint(
            name = "pelvis_lowerback",
            posInParent = (0, pelvisSizeY/2.0, 0),
            posInChild = (0, -lowerBackSizeY/2.0 -lowerBackOffsetY, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis  = (0, 0, 1),
            limits = (-1.6, 1.6, -1.6, 1.6, -1.6, 1.6) ).createAndFillObject()            
        joint.setParent(pelvis)
        joint.setChild(lowerback)            
        character.addJoint(joint)

        totalTorsoSizeY = self.getShoulderPosY() - self.getChestPosY()
        torsoOffsetY = -0.2 * totalTorsoSizeY
        torsoSizeX = self.getTorsoDiameter()        
        torsoSizeY = totalTorsoSizeY - torsoOffsetY
        torsoSizeZ = torsoSizeX * 0.6
        torso = PyUtils.RigidBody.createArticulatedTaperedBox( 
            name = "torso", 
            size = (torsoSizeX, torsoSizeY, torsoSizeZ),
            exponentTop = 2.2, exponentBottom = 4, exponentSide = 3,
            mass = -massScale, colour = green )
        character.addArticulatedRigidBody( torso )

        joint = Proxys.BallInSocketJoint(
            name = "lowerback_torso",
            posInParent = (0, lowerBackSizeY/2.0, 0),
            posInChild = (0, -torsoSizeY/2.0 -torsoOffsetY, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis  = (0, 0, 1),
            limits = (-1.6, 1.6, -1.6, 1.6, -1.6, 1.6) ).createAndFillObject()            
        joint.setParent(lowerback)
        joint.setChild(torso)            
        character.addJoint(joint)

        headOffsetY = self.getNeckSizeY()
        headSizeX = self.getHeadSizeX()        
        headSizeY = self.getHeadSizeY()
        headSizeZ = self.getHeadSizeZ()
        head = PyUtils.RigidBody.createArticulatedEllipsoid( 
            name = "head", 
            radius = (headSizeX/2.0, headSizeY/2.0, headSizeZ/2.0),
            mass = -massScale, withMesh = False )
        character.addArticulatedRigidBody( head )
        head.addMeshObj( "data/StockMeshes/head.obj", Vector3d(0,-0.064,0), Vector3d(headSizeX*6.5,headSizeY*4.6,headSizeZ*5.5) )
        head.setColour( *red ) 
        head.addMesh( PyUtils.Mesh.createCylinder( 
            basePoint = (0,-headSizeY/2.0 - headOffsetY - torsoSizeY*0.1,0),
            tipPoint = (0,-headSizeY*0.2,0),
            radius = 0.12*headSizeX, colour = red ))

        joint = Proxys.BallInSocketJoint(
            name = "torso_head",
            posInParent = (0, torsoSizeY/2.0, 0),
            posInChild = (0, -headSizeY/2.0 - headOffsetY, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 0, 1),
            limits = (-1.6, 1.6, -1.6, 1.6, -1.6, 1.6) ).createAndFillObject()            
        joint.setParent(torso)
        joint.setChild(head)            
        character.addJoint(joint)
        
        leftUpperArmSizeY = self.getShoulderPosY() - self.getElbowPosY(1)        
        leftUpperArmDiameter = self.getUpperArmDiameter(1)        
        lUpperArm = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "lUpperArm", 
            moiScale = 3,
            axis = 0, basePos = -leftUpperArmSizeY/2.0, tipPos = leftUpperArmSizeY/2.0, 
            radius = leftUpperArmDiameter/2.0,
            mass = -massScale, colour = green )
        character.addArticulatedRigidBody( lUpperArm )
        lUpperArm.addMesh( PyUtils.Mesh.createSphere( (-leftUpperArmSizeY/2.0, 0, 0), leftUpperArmDiameter*0.65, colour = green ) ) 
        lUpperArm.addMesh( PyUtils.Mesh.createSphere( (leftUpperArmSizeY/2.0, 0, 0), leftUpperArmDiameter*0.5, colour = green ) ) 
        
        joint = Proxys.BallInSocketJoint(
            name = "lShoulder",
            posInParent = (torsoSizeX*0.52, torsoSizeY*0.32, 0),
            posInChild = (-leftUpperArmSizeY/2.0, 0, 0),
            swingAxis1 = (0, 0, 1),
            twistAxis  = (1, 0, 0),
            limits = (-100, 100, -1.5, 1.5, -100, 100) ).createAndFillObject()            
        joint.setParent(torso)
        joint.setChild(lUpperArm)            
        character.addJoint(joint)
        
        rightUpperArmSizeY = self.getShoulderPosY() - self.getElbowPosY(-1)        
        rightUpperArmDiameter = self.getUpperArmDiameter(-1)        
        rUpperArm = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "rUpperArm", 
            moiScale = 3,
            axis = 0, basePos = -rightUpperArmSizeY/2.0, tipPos = rightUpperArmSizeY/2.0, 
            radius = rightUpperArmDiameter/2.0,
            mass = -massScale, colour = green )
        character.addArticulatedRigidBody( rUpperArm )
        rUpperArm.addMesh( PyUtils.Mesh.createSphere( (rightUpperArmSizeY/2.0, 0, 0), rightUpperArmDiameter*0.65, colour = green ) ) 
        rUpperArm.addMesh( PyUtils.Mesh.createSphere( (-rightUpperArmSizeY/2.0, 0, 0), rightUpperArmDiameter*0.5, colour = green ) ) 
        
        joint = Proxys.BallInSocketJoint(
            name = "rShoulder",
            posInParent = (-torsoSizeX*0.52, torsoSizeY*0.32, 0),
            posInChild = (rightUpperArmSizeY/2.0, 0, 0),
            swingAxis1 = (0, 0, 1),
            twistAxis  = (1, 0, 0),
            limits = (-100, 100, -1.5, 1.5, -100, 100) ).createAndFillObject()            
        joint.setParent(torso)
        joint.setChild(rUpperArm)            
        character.addJoint(joint)
        
        leftLowerArmSizeY = self.getElbowPosY(1) - self.getWristPosY(1)        
        leftLowerArmDiameter = self.getLowerArmDiameter(1)        
        lLowerArm = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "lLowerArm", 
            moiScale = 3,
            axis = 0, basePos = -leftLowerArmSizeY/2.0, tipPos = leftLowerArmSizeY/2.0, 
            radius = leftLowerArmDiameter/2.0,
            mass = -massScale, colour = red )
        character.addArticulatedRigidBody( lLowerArm )
        lLowerArm.addMesh( PyUtils.Mesh.createTaperedBox( 
            position=(leftLowerArmSizeY*0.5+leftLowerArmDiameter*0.8,0,0),
            size=(leftLowerArmDiameter*1.6, leftLowerArmDiameter*0.5, leftLowerArmDiameter*1.15), colour = red ) ) 
        
        joint = Proxys.HingeJoint(
            name = "lElbow",
            posInParent = (leftUpperArmSizeY/2.0, 0, 0),
            posInChild = (-leftLowerArmSizeY/2.0, 0, 0),
            axis = (0, 1, 0),
            limits = (-2.7, 0) ).createAndFillObject()            
        joint.setParent(lUpperArm)
        joint.setChild(lLowerArm)            
        character.addJoint(joint)
        
        rightLowerArmSizeY = self.getElbowPosY(-1) - self.getWristPosY(-1)        
        rightLowerArmDiameter = self.getLowerArmDiameter(-1)        
        rLowerArm = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "rLowerArm", 
            moiScale = 3,
            axis = 0, basePos = -rightLowerArmSizeY/2.0, tipPos = rightLowerArmSizeY/2.0, 
            radius = rightLowerArmDiameter/2.0,
            mass = -massScale, colour = red )
        character.addArticulatedRigidBody( rLowerArm )
        rLowerArm.addMesh( PyUtils.Mesh.createTaperedBox( 
            position=(-rightLowerArmSizeY*0.5-rightLowerArmDiameter*0.8,0,0),
            size=(rightLowerArmDiameter*1.6, rightLowerArmDiameter*0.5, rightLowerArmDiameter*1.15), colour = red ) )
            
        joint = Proxys.HingeJoint(
            name = "rElbow",
            posInParent = (-rightUpperArmSizeY/2.0, 0, 0),
            posInChild = (rightLowerArmSizeY/2.0, 0, 0),
            axis = (0, -1, 0),
            limits = (-2.7, 0) ).createAndFillObject()            
        joint.setParent(rUpperArm)
        joint.setChild(rLowerArm)            
        character.addJoint(joint)
        
        leftUpperLegSizeY = self.getHipPosY() - self.getKneePosY(1)        
        leftUpperLegDiameter = self.getUpperLegDiameter(1)
        lUpperLeg = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "lUpperLeg", 
            axis = 1, basePos = -leftUpperLegSizeY/2.0, tipPos = leftUpperLegSizeY/2.0, 
            radius = leftUpperLegDiameter/2.0,
            moiScale = 4,
            mass = -massScale, colour = blue )
        character.addArticulatedRigidBody( lUpperLeg )
        lUpperLeg.addMesh( PyUtils.Mesh.createSphere( (0, leftUpperLegSizeY/2.0, 0), leftUpperLegDiameter*0.5, colour = blue ) ) 
        lUpperLeg.addMesh( PyUtils.Mesh.createSphere( (0, -leftUpperLegSizeY/2.0, 0), leftUpperLegDiameter*0.5, colour = blue ) ) 
        
        joint = Proxys.BallInSocketJoint(
            name = "lHip",
            posInParent = (pelvisRadius*self.getLegRelativeAnchorX(1), -pelvisSizeY/2.0, 0),
            posInChild = (0, leftUpperLegSizeY/2.0, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 0, 1),
            limits = (-1.3, 1.9, -1, 1, -1, 1) ).createAndFillObject()            
        joint.setParent(pelvis)
        joint.setChild(lUpperLeg)            
        character.addJoint(joint)
        
        rightUpperLegSizeY = self.getHipPosY() - self.getKneePosY(-1)        
        rightUpperLegDiameter = self.getUpperLegDiameter(-1)
        rUpperLeg = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "rUpperLeg", 
            axis = 1, basePos = -rightUpperLegSizeY/2.0, tipPos = rightUpperLegSizeY/2.0, 
            radius = rightUpperLegDiameter/2.0,
            moiScale = 4,
            mass = -massScale, colour = blue )
        character.addArticulatedRigidBody( rUpperLeg )
        rUpperLeg.addMesh( PyUtils.Mesh.createSphere( (0, -rightUpperLegSizeY/2.0, 0), rightUpperLegDiameter*0.5, colour = blue ) ) 
        rUpperLeg.addMesh( PyUtils.Mesh.createSphere( (0, rightUpperLegSizeY/2.0, 0), rightUpperLegDiameter*0.5, colour = blue ) ) 
        
        joint = Proxys.BallInSocketJoint(
            name = "rHip",
            posInParent = (-pelvisRadius*self.getLegRelativeAnchorX(-1), -pelvisSizeY/2.0, 0),
            posInChild = (0, rightUpperLegSizeY/2.0, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 0, 1),
            limits = (-1.3, 1.9, -1, 1, -1, 1) ).createAndFillObject()            
        joint.setParent(pelvis)
        joint.setChild(rUpperLeg)            
        character.addJoint(joint)
        
        leftLowerLegSizeY = self.getKneePosY(1) - self.getAnklePosY(1)        
        leftLowerLegDiameter = self.getLowerLegDiameter(1)
        lLowerLeg = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "lLowerLeg", 
            axis = 1, basePos = -leftLowerLegSizeY/2.0, tipPos = leftLowerLegSizeY/2.0, 
            radius = leftLowerLegDiameter/2.0,
            moiScale = 4,
            mass = -massScale, colour = blue )
        character.addArticulatedRigidBody( lLowerLeg )
        
        joint = Proxys.HingeJoint(
            name = "lKnee",
            posInParent = (0, -leftUpperLegSizeY/2.0, 0),
            posInChild = (0, leftLowerLegSizeY/2.0, 0),
            axis = (1, 0, 0),
            limits = (0, 2.5) ).createAndFillObject()            
        joint.setParent(lUpperLeg)
        joint.setChild(lLowerLeg)            
        character.addJoint(joint)
        
        rightLowerLegSizeY = self.getKneePosY(-1) - self.getAnklePosY(-1)        
        rightLowerLegDiameter = self.getLowerLegDiameter(-1)
        rLowerLeg = PyUtils.RigidBody.createArticulatedCylinder( 
            name = "rLowerLeg", 
            axis = 1, basePos = -rightLowerLegSizeY/2.0, tipPos = rightLowerLegSizeY/2.0, 
            radius = rightLowerLegDiameter/2.0,
            moiScale = 4,
            mass = -massScale, colour = blue )
        character.addArticulatedRigidBody( rLowerLeg )
        
        joint = Proxys.HingeJoint(
            name = "rKnee",
            posInParent = (0, -rightUpperLegSizeY/2.0, 0),
            posInChild = (0, rightLowerLegSizeY/2.0, 0),
            axis = (1, 0, 0),
            limits = (0, 2.5) ).createAndFillObject()            
        joint.setParent(rUpperLeg)
        joint.setChild(rLowerLeg)            
        character.addJoint(joint)
        
        leftFootSizeX = self.getFootSizeX(1)
        leftFootSizeY = self.getAnklePosY(1) - self.getGroundPosY()        
        leftFootSizeZ = self.getFootSizeZ(1) * 0.75
        lFoot = PyUtils.RigidBody.createArticulatedTaperedBox( 
            name = "lFoot", 
            size = (leftFootSizeX,leftFootSizeY,leftFootSizeZ), 
            exponentSide = 20, 
            groundCoeffs = (0.0005,0.2),
            moiScale = 3,
            mass = -massScale, 
            colour = gray )
        character.addArticulatedRigidBody( lFoot )
        
        joint = Proxys.UniversalJoint(
            name = "lAnkle",
            posInParent = (0, -leftLowerLegSizeY/2.0, 0),
            posInChild = (0, leftFootSizeY/2.0, -leftFootSizeZ*0.33 + leftLowerLegDiameter/2.0),
            parentAxis = (0, 0, 1),
            childAxis = (1, 0, 0),
            limits = (-0.75, 0.75, -0.75, 0.75) ).createAndFillObject()            
        joint.setParent(lLowerLeg)
        joint.setChild(lFoot)            
        character.addJoint(joint)
        
        rightFootSizeX = self.getFootSizeX(-1)
        rightFootSizeY = self.getAnklePosY(-1) - self.getGroundPosY()        
        rightFootSizeZ = self.getFootSizeZ(-1) * 0.75
        rFoot = PyUtils.RigidBody.createArticulatedTaperedBox(
            name = "rFoot", 
            size = (rightFootSizeX,rightFootSizeY,rightFootSizeZ),
            exponentSide = 20,             
            groundCoeffs = (0.0005,0.2),
            moiScale = 3,
            mass = -massScale, colour = gray )
        character.addArticulatedRigidBody( rFoot )
        
        joint = Proxys.UniversalJoint(
            name = "rAnkle",
            posInParent = (0, -rightLowerLegSizeY/2.0, 0),
            posInChild = (0, rightFootSizeY/2.0, -rightFootSizeZ*0.33 + rightLowerLegDiameter/2.0),
            parentAxis = (0, 0, -1),
            childAxis = (1, 0, 0),
            limits = (-0.75, 0.75, -0.75, 0.75) ).createAndFillObject()            
        joint.setParent(rLowerLeg)
        joint.setChild(rFoot)            
        character.addJoint(joint)
        
        leftToesSizeX = leftFootSizeX
        leftToesSizeY = leftFootSizeY * 0.66
        leftToesSizeZ = self.getFootSizeZ(1) - leftFootSizeZ
        lToes = PyUtils.RigidBody.createArticulatedTaperedBox( 
            name = "lToes", 
            size = (leftToesSizeX,leftToesSizeY,leftToesSizeZ), 
            exponentSide = 20,
            groundCoeffs = (0.0005,0.2),
            moiScale = 3,
            mass = -massScale, colour = gray )
        character.addArticulatedRigidBody( lToes )
        
        joint = Proxys.HingeJoint(
            name = "lToeJoint",
            posInParent = (0, (leftToesSizeY-leftFootSizeY)/2.0+0.003, leftFootSizeZ/2.0),
            posInChild = (0, 0, -leftToesSizeZ/2.0),
            axis = ( 1, 0, 0 ),
            limits = (-0.52, 0.1) ).createAndFillObject()
        joint.setParent(lFoot)
        joint.setChild(lToes)            
        character.addJoint(joint)
        
        rightToesSizeX = rightFootSizeX
        rightToesSizeY = rightFootSizeY * 0.66   
        rightToesSizeZ = self.getFootSizeZ(-1) - rightFootSizeZ
        rToes = PyUtils.RigidBody.createArticulatedTaperedBox( 
            name = "rToes", 
            size = (rightToesSizeX,rightToesSizeY,rightToesSizeZ), 
            exponentSide = 20, 
            groundCoeffs = (0.0005,0.2),
            moiScale = 3,
            mass = -massScale, colour = gray )
        character.addArticulatedRigidBody( rToes )
        
        joint = Proxys.HingeJoint(
            name = "rToeJoint",
            posInParent = (0, (rightToesSizeY-rightFootSizeY)/2.0+0.003, rightFootSizeZ/2.0),
            posInChild = (0, 0, -rightToesSizeZ/2.0),
            axis = ( 1, 0, 0 ),
            limits = (-0.52, 0.1) ).createAndFillObject()            
        joint.setParent(rFoot)
        joint.setChild(rToes)            
        character.addJoint(joint)
        
        return character
        

class _Value(object):
    
    def __init__(self, value, minValue=None, maxValue=None):
        """Create a native value with a given maximum and minimum"""
        self._value = value
        if maxValue is not None and minValue is not None and maxValue < minValue :
            raise ValueError( 'Max must be equal or greater than minValue!' )
        self._minValue = minValue
        self._maxValue = maxValue
        
    def get(self):
        return self._value

    def set(self, value):
        self._value = self._clamp(value)    
        
    def _clamp(self, value):
        if self._minValue is not None and value < self._minValue:
            return self._minValue
        if self._maxValue is not None and value > self._maxValue:
            return self._maxValue
        return value

class _Symmetric(_Value):
    
    def __init__(self, value, minValue=None, maxValue=None ):
        """Create a symmetric native value"""
        super(_Symmetric,self).__init__(None, minValue, maxValue)
        self._side = [value,value]
    
    def getSide(self, side):
        """Gets the right side (-1) or the left side (1) value."""
        return self._side[(side+1)/2]

    def getRight(self):
        """Gets the right side value."""
        return self._side[0]

    def getLeft(self):
        """Gets the left side value."""
        return self._side[1]

    def setSide(self, side, value):
        """Sets the right side (-1) or the left side (1) value or both at once (0)."""
        value = self._clamp(value)
        if side == 0 :
            self._side = [value,value]
            return
        if side != -1 and side != 1 :
            raise IndexError( 'Side must be -1, 0 or 1.' ) 
        self._side[(side+1)/2] = value

    def setRight(self, value):
        """Sets the right side value."""
        self._side[0] = self._clamp(value)

    def setLeft(self, value):
        """Sets the left side value."""
        self._side[1] = self._clamp(value)

    def forceSymmetric(self):
        """Make sure the value is symmetric by coping the left side (1) into the right side (-1)."""
        self._side[0] = self._side[1]
        
    