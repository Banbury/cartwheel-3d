'''
Created on 2009-12-02

@author: beaudoin
'''

from OpenGL.GL import *
import UI, Core, GLUtils
from MathLib import Point3d, Vector3d, Quaternion
import math

class CheckBoxCallback(GLUtils.GLUICallback):
    
    def __init__(self, editorWindow):
        """Private callback class."""
        super(CheckBoxCallback,self).__init__()
        self._editorWindow = editorWindow
        
    def execute(self):
        """Callback execution"""
        self._editorWindow._checkBoxChanged()
        

class EditorWindow(GLUtils.GLUIContainer):

    def __init__( self, parent, posableCharacter, handlesSide, handlesFront, stanceKneeHandle, swingFootHandleSagittal, swingFootHandleCoronal, swingFootHandleHeight,
                  time, controller, stanceFootToSwingFootTrajectory, minWidth=-1, minHeight=-1, checkBoxVisible=True):
        super(EditorWindow,self).__init__(parent)

        self._sizer = GLUtils.GLUIBoxSizer(GLUtils.GLUI_VERTICAL)
        self.setSizer(self._sizer)
        
        handleVisible = len(handlesSide) != 0 and handlesSide[0].hasKeyframeAtTime(time)  
        
        if checkBoxVisible:
            self._checkBox = GLUtils.GLUICheckBox(self,0,0,0,0,-1,-1,handleVisible)
            self._sizer.add(self._checkBox, 0, GLUtils.GLUI_EXPAND )
            self._callback = CheckBoxCallback(self)
            self._checkBox.setCheckBoxCallback(self._callback)
        else:
            self._spacer = GLUtils.GLUIWindow(self)
            self._sizer.add(self._spacer, 1, GLUtils.GLUI_EXPAND )
            
        self._editorSide = CharacterEditorWindow(self, 
                                             posableCharacter, 
                                             handlesSide,
                                             stanceKneeHandle,
                                             swingFootHandleSagittal,
                                             swingFootHandleHeight,
                                             time, 
                                             controller, 
                                             stanceFootToSwingFootTrajectory, 
                                             ConverterZY(),
                                             0, 0, 0, 0, minWidth, minHeight )
        self._sizer.add(self._editorSide)
        self._editorSide.addHandlesVisibilityChangedCallback( self._setCheckBox )
        
        self._editorFront = CharacterEditorWindow(self, 
                                             posableCharacter, 
                                             handlesFront, 
                                             None,
                                             swingFootHandleCoronal,
                                             swingFootHandleHeight,
                                             time, 
                                             controller, 
                                             stanceFootToSwingFootTrajectory, 
                                             ConverterXY(),
                                             0, 0, 0, 0, minWidth, minHeight )
        self._sizer.add(self._editorFront)
        self._editorFront.addHandlesVisibilityChangedCallback( self._setCheckBox )

        self._checkBoxCallbacks = []

    def addCheckBoxCallback(self, callback):
        """Adds a function that will be called back whenever the checkbox state is changed."""
        self._checkBoxCallbacks.append(callback)

    def _setCheckBox(self, checked):        
        try: 
            if checked != self._checkBox.isChecked():
                self._checkBox.setChecked(checked)            
        except AttributeError:
            pass

    def _checkBoxChanged(self):
        """Called whenever the checkbox is changed."""
        for callback in self._checkBoxCallbacks:
            callback(self._checkBox.isChecked())


class CharacterEditorWindow(UI.GLUITools.WindowWithControlPoints):
    
    def __init__( self, parent, posableCharacter, handles, stanceKneeHandle, swingFootHandleX, swingFootHandleY, 
                  time, controller, stanceFootToSwingFootTrajectory, converter, x=0, y=0, width=0, height=0, minWidth=-1, minHeight=-1 ):
        """A keyframe edition window. Character are always forced to left stance."""
        
        super(CharacterEditorWindow,self).__init__(parent,x,y,width,height, minWidth, minHeight, boundsY=(-0.1,1.9), forceAspectRatio='x')
        
        self._converter = converter

        self._posableCharacter = posableCharacter
        self._character = posableCharacter.getCharacter()
        self._handles = handles
        self._stanceKneeHandle = stanceKneeHandle
        self._swingFootHandleX = swingFootHandleX
        self._swingFootHandleY = swingFootHandleY
        self._time = time
        self._controller = controller
        self._stanceFootToSwingFootTrajectory = stanceFootToSwingFootTrajectory
    
        self._lineTorso = [ '_pelvis', 'pelvis_lowerback', 'lowerback_torso', 'torso_head', '_head' ]
        self._lArm  = [ '_torso', 'lShoulder', 'lElbow', '_lLowerArm' ]
        self._rArm = [ '_torso', 'rShoulder', 'rElbow', '_rLowerArm' ]
        self._lLeg  = [ '_pelvis', 'lHip', 'lKnee', 'lAnkle', 'lToeJoint' ]
        self._rLeg = [ '_pelvis', 'rHip', 'rKnee', 'rAnkle', 'rToeJoint' ]

        self._handlesVisibilityChangedCallbacks = []

        self._handlesVisible = None
        self._updateHandlesVisibility()        
        

    def _setHandlesVisible(self, visible):
        """Make sure the handles are visible or not."""
        if self._handlesVisible == visible: return
        self._handlesVisible = visible
        self.deleteAllControlPoints()
        if visible:
            for handle in self._handles:
                self.addControlPoint( HandleControlPoint(self._posableCharacter, self._time, self._converter, handle) )
            if self._stanceKneeHandle is not None:
                self.addControlPoint( HandleStanceKneeControlPoint(self._posableCharacter, self._time, self._converter, self._stanceKneeHandle) )
            if self._time > 0 and self._time < 1 and self._swingFootHandleX is not None and self._swingFootHandleY is not None:
                self.addControlPoint( SwingFootControlPoint(self._posableCharacter, self._time, self._converter, self._swingFootHandleX, self._swingFootHandleY) )
        for callback in self._handlesVisibilityChangedCallbacks:
            callback(visible)

    def _updateHandlesVisibility(self):
        """Check if the control points should be visible. 
        Only check the first handle. Assume they all have keyframes at the same time."""        
        if len(self._handles) == 0 :
            return                
        else:
            self._setHandlesVisible( self._handles[0].hasKeyframeAtTime(self._time) )

    def addHandlesVisibilityChangedCallback(self, callback):
        """Adds a callback that will be called whenever the visibility of the handles change.
        The callback should take one boolean parameter, true if they are visible, false otherwise."""
        self._handlesVisibilityChangedCallbacks.append(callback)
    
    def areHandlesVisible(self):
        """Return true if the handles are visible, false otherwise."""
        return self._handlesVisible
    
    def drawContent(self):
        """Draw the character from front view."""
        self._updateHandlesVisibility()
        
        self._posableCharacter.updatePose( self._time, self._stanceFootToSwingFootTrajectory.evaluate_catmull_rom(self._time) )

        try:
            glColor3d(0.4,0.5,0.0)
            
            self._drawLine( self._lArm )
            self._drawLine( self._lLeg )
            glColor3d(1,1,1)
            self._drawLine( self._rArm )
            self._drawLine( self._rLeg )

            self._drawLine( self._lineTorso )
                    
        except Exception as e:
            glEnd()
            print "Exception while drawing scaled character interface: " + str(e)
            traceback.print_exc(file=sys.stdout)
 
    def _drawLine(self, line ):
        
        glBegin( GL_LINE_STRIP )
        for name in line:
            if name[0] == '_' :
                pos = self._character.getARBByName(name[1:]).getCMPosition()
            else:
                joint = self._character.getJointByName(name)
                arb = joint.getParent()
                pos = arb.getWorldCoordinates( joint.getParentJointPosition() )
            glVertex2d( *self._converter.to2d(pos) )
        glEnd()


class _Type(object):
    circular = 0
    perpendicular = 1
    


class BaseControlPoint(UI.GLUITools.ControlPoint):
    
    def __init__( self, posableCharacter, time, converter ):
        super(BaseControlPoint,self).__init__()
        self._posableCharacter = posableCharacter
        self._character = posableCharacter.getCharacter()
        self._time = time
        self._converter = converter
        
class BaseHandleControlPoint(BaseControlPoint):
    
    def __init__( self, posableCharacter, time, converter, handle ):
        super(BaseHandleControlPoint,self).__init__(posableCharacter, time, converter)
        self._handle  = handle        
        
        
perpendicularSpeed = 3 # Increase this to make perpendicular handle rotate more quickly
stanceKneeSpeed = 4.2  # Increase this to make stance knee handle rotate more quickly
    
class HandleControlPoint(BaseHandleControlPoint):
    
    def __init__( self, posableCharacter, time, converter, handle ):
        super(HandleControlPoint,self).__init__(posableCharacter, time, converter, handle)
        
        characterJointName = handle.getJointName().replace("STANCE_","l").replace("SWING_","r")
        joint = self._character.getJointByName( characterJointName )
        
        self._jointChild = joint.getChild()
        self._pivotPosInChild = joint.getChildJointPosition()
        self._handlePosInChild = handle.getPosInChild()

        type = handle.getType()
        if type == 'circular':
            self._type = _Type.circular
            self._sign = 1
        elif type == 'reverseCircular':
            self._type = _Type.circular
            self._sign = -1
        elif type == 'perpendicular':
            self._type = _Type.perpendicular
            self._sign = 1
        elif type == 'reversePerpendicular':
            self._type = _Type.perpendicular
            self._sign = -1
        else:
            raise ValueError( 'Handle, supported type = "circular", "reverseCircular", "perpendicular", or "reversePerpendicular"' )

    def getPos(self):       
        posHandle = self._jointChild.getWorldCoordinates( self._handlePosInChild )
        return self._converter.to2d( posHandle )

    def setPos(self, pos):
        if self._type == _Type.circular :
            posPivot = self._jointChild.getWorldCoordinates( self._pivotPosInChild )
            self._converter.project( posPivot )
            v1 = Vector3d( posPivot, Point3d(*self._converter.to3d(self._previousMousePos) ) ).unit()
            v2 = Vector3d( posPivot, Point3d(*self._converter.to3d(pos) ) ).unit()
            cos = v1.dotProductWith(v2)
            sin = v1.crossProductWith(v2).dotProductWith(Vector3d(*self._converter.normal())) * self._sign
            theta = math.atan2(sin, cos)
        else: # _Type.perpendicular
            posPivot = self._jointChild.getWorldCoordinates( self._pivotPosInChild )
            handlePos = self._jointChild.getWorldCoordinates( self._handlePosInChild )
            vector = Vector3d( posPivot, handlePos ).unit().crossProductWith( Vector3d(*self._converter.normal()) )
            vector2d = self._converter.to2d( vector )
            theta = (vector2d[0]*(pos[0]-self._previousMousePos[0]) + vector2d[1]*(pos[1]-self._previousMousePos[1])) * self._sign * perpendicularSpeed
            
        index = self._handle.getIndexForTime(self._time)
        if index is None: 
            return
        value = self._handle.getKeyframeValue(index)
        self._handle.setKeyframeValue(index, value+theta)


class HandleStanceKneeControlPoint(BaseHandleControlPoint):
    
    def __init__( self, posableCharacter, time, converter, handle ):
        super(HandleStanceKneeControlPoint,self).__init__(posableCharacter, time, converter, handle)

        joint = self._character.getJointByName( 'lHip' )
        self._jointChild = joint.getChild()
        self._handlePosInChild = joint.getChildJointPosition()
                
    def getPos(self):       
        posHandle = self._jointChild.getWorldCoordinates( self._handlePosInChild )
        return self._converter.to2d( posHandle )

    def setPos(self, pos):
        theta = (self._previousMousePos[1]-pos[1]) * stanceKneeSpeed 
        index = self._handle.getIndexForTime(self._time)
        if index is None: 
            return
        value = self._handle.getKeyframeValue(index)
        self._handle.setKeyframeValue(index, value+theta)

        
class SwingFootControlPoint(BaseControlPoint):
    
    def __init__( self, posableCharacter, time, converter, handleX, handleY ):
        super(SwingFootControlPoint,self).__init__(posableCharacter, time, converter)
      
        joint = self._character.getJointByName( 'rAnkle' )
        self._jointChild = joint.getChild()
        self._handlePosInChild = joint.getChildJointPosition()
        
        self._handleX = handleX
        self._handleY = handleY
                  
    def getPos(self):       
        posHandle = self._jointChild.getWorldCoordinates( self._handlePosInChild )
        return self._converter.to2d( posHandle )

    def setPos(self, pos):
        deltaX = pos[0]-self._previousMousePos[0]
        deltaY = pos[1]-self._previousMousePos[1]
        index = self._handleX.getIndexForTime(self._time)
        if index is None: 
            return
        assert index == self._handleY.getIndexForTime(self._time), 'Unexpected error: handle X and Y keyframes are out-of-sync.'
        valueX = self._handleX.getKeyframeValue(index)
        valueY = self._handleY.getKeyframeValue(index)
        self._handleX.setKeyframeValue(index, valueX+deltaX)
        self._handleY.setKeyframeValue(index, valueY+deltaY)        
        return

        
class ConverterZY(object):
    
    def to2d(self,vec):
        return (vec.z, vec.y)
    
    
    def to3d(self,vec):
        return (0, vec[1], vec[0])

    def normal(self):
        return (1,0,0)
    
    def project(self, vec):
        vec.x = 0

class ConverterXY(object):
    
    def to2d(self,vec):
        return (vec.x, vec.y)
    
    def to3d(self,vec):
        return (vec[0], vec[1], 0)

    def normal(self):
        return (0,0,1)
    
    def project(self, vec):
        vec.z = 0
    