'''
Created on 2009-12-02

@author: beaudoin
'''

import wx, PyUtils, GLUtils
from Handle import BaseHandle, Handle
from PosableCharacter import PosableCharacter
from ToolSet import ToolSet

from EditorWindow import EditorWindow
from MathLib import Vector3d, Point3d, Trajectory3dv

class Model(object):

    def __init__(self, nbEditors = 5):
        self._nbEditors = nbEditors
        
        app = wx.GetApp()
        app.addControllerObserver(self)
        
        app = wx.GetApp()
        glCanvas = app.getGLCanvas()
        
        self._container = glCanvas.addGLUITool( GLUtils.GLUIContainer )

        self._sizer = GLUtils.GLUIBoxSizer(GLUtils.GLUI_HORIZONTAL)
        self._container.setSizer(self._sizer)

        self._optionsObservable = PyUtils.Observable()

        self._create()

        self._toolSet = ToolSet(app.getToolPanel(), self)
        
    def addOptionsObserver(self, observer):
        """Adds an observer for the options of the style edition model."""
        self._optionsObservable.addObserver(observer)
        
    def displayInterface(self, display):
        """Indicates wheter the interface for editing style should be displayed."""
        if display != self.getDisplayInterface() :
            self._container.setVisible(display)
            self._container.getParent().layout()
            self._optionsObservable.notifyObservers()
        
    def getDisplayInterface(self):
        """Indicates wheter the interface for editing style is currently displayed."""
        return self._container.isVisible()
    

    def update(self, observable, data= None):
        """Called whenever something important is modified, recreate everything."""
        self._destroy()
        self._create()

    def _destroy(self):
        
        self._handles = []
        self._editors = []

        self._container.detachAllChildren()    
        self._container.getParent().layout()            
    
    
    def _create(self):
        """Creates the basic model class for the simple keyframe editor. Characters are always forced to left stance."""        

        
        app = wx.GetApp()
        
        try: 
            self._controller = app.getController(0)
        except IndexError:
            return
        self._character = self._controller.getCharacter()

        handlesSide = []
        handlesFront = []
        
        self._handles = []
        handlesSide.append( self._addHandle( 'SWING_Shoulder', 2, 'SWING_Elbow', minValue = -3.14, maxValue = 3.14 ) )
        handlesSide.append( self._addHandle( 'STANCE_Shoulder', 2, 'STANCE_Elbow', minValue = -3.14, maxValue = 3.14 ) )
        handlesSide.append( self._addHandle( 'SWING_Elbow', 0, reverseOppositeJoint = True, minValue = -2.8, maxValue = 0 ) )
        handlesSide.append( self._addHandle( 'STANCE_Elbow', 0, type = 'reverseCircular', reverseOppositeJoint = True,minValue = 0, maxValue = 2.8 ) )
        handlesSide.append( self._addHandle( 'SWING_Ankle', 0, 'SWING_ToeJoint' ) )
        handlesSide.append( self._addHandle( 'STANCE_Ankle', 0, 'STANCE_ToeJoint' ) )
        handlesSide.append( self._addHandle( 'pelvis_lowerback', 2, 'lowerback_torso', minValue = -1.2, maxValue = 1.2  ) )
        handlesSide.append( self._addHandle( 'lowerback_torso', 2, 'torso_head', minValue = -1.2, maxValue = 1.2  ) )
        handlesSide.append( self._addHandle( 'torso_head', 2, minValue = -1.2, maxValue = 1.2  ) )

        handlesFront.append( self._addHandle( 'SWING_Shoulder', 1, 'SWING_Elbow', type = 'reverseCircular', reverseOppositeJoint = True, minValue = -2, maxValue = 2  ) )
        handlesFront.append( self._addHandle( 'STANCE_Shoulder', 1, 'STANCE_Elbow', type = 'reverseCircular', reverseOppositeJoint = True, minValue = -2, maxValue = 2  ) )
        handlesFront.append( self._addHandle( 'SWING_Shoulder', 0, type = 'reversePerpendicular', minValue = -3.3, maxValue = 3.3 ) )
        handlesFront.append( self._addHandle( 'STANCE_Shoulder', 0, type = 'perpendicular', minValue = -3.3, maxValue = 3.3 ) )
        handlesFront.append( self._addHandle( 'pelvis_lowerback', 1, 'lowerback_torso', reverseOppositeJoint = True, minValue = -1.2, maxValue = 1.2  ) )
        handlesFront.append( self._addHandle( 'lowerback_torso', 1, 'torso_head', reverseOppositeJoint = True, minValue = -1.2, maxValue = 1.2  ) )
        handlesFront.append( self._addHandle( 'torso_head', 1, reverseOppositeJoint = True, minValue = -1.2, maxValue = 1.2  ) )


        stanceKneeHandle = Handle( self._controller, 'STANCE_Knee', 0, minValue = 0.1, maxValue = 2.2  )
        self._handles.append( stanceKneeHandle )
        
        swingFootHandleSagittal = BaseHandle( self._controller.getSwingFootTrajectoryDeltaSagittal() )
        swingFootHandleCoronal = BaseHandle( self._controller.getSwingFootTrajectoryDeltaCoronal() )
        swingFootHandleHeight = BaseHandle( self._controller.getSwingFootTrajectoryDeltaHeight() )
        self._handles.append( swingFootHandleSagittal )
        self._handles.append( swingFootHandleCoronal )
        self._handles.append( swingFootHandleHeight )


        self._editors = []
        
        self._times = [ i / float(self._nbEditors-1) for i in range(self._nbEditors) ]

        for handle in self._handles:
            handle.forceKeyframesAt([0,1],self._times)

        for handle in self._handles:
            handle.enforceSymmetry()

        stanceFootToSwingFootTrajectory = Trajectory3dv()
        stanceFootToSwingFootTrajectory.addKnot(0,Vector3d(-0.2,0,-0.4))
        stanceFootToSwingFootTrajectory.addKnot(0.5,Vector3d(-0.2,0.125,0))
        stanceFootToSwingFootTrajectory.addKnot(1,Vector3d(-0.2,0,0.4))

        glCanvasSize = wx.GetApp().getGLCanvas().GetSize()
        minWidth = glCanvasSize.x / self._nbEditors - 50
        minHeight = glCanvasSize.y / 2

        for i, time in enumerate(self._times) :
            posableCharacter = PosableCharacter(PyUtils.copy(self._character), self._controller)
            if i == 0 or i == self._nbEditors-1:
                checkBoxVisible = False
            else :
                checkBoxVisible = True
            editor = EditorWindow( self._container, 
                                   posableCharacter = posableCharacter, 
                                   handlesSide = handlesSide, 
                                   handlesFront = handlesFront,
                                   stanceKneeHandle = stanceKneeHandle, 
                                   swingFootHandleSagittal = swingFootHandleSagittal, 
                                   swingFootHandleCoronal = swingFootHandleCoronal, 
                                   swingFootHandleHeight = swingFootHandleHeight, 
                                   time = time, 
                                   controller = self._controller, 
                                   stanceFootToSwingFootTrajectory = stanceFootToSwingFootTrajectory, 
                                   minWidth = minWidth, minHeight = minHeight, 
                                   checkBoxVisible = checkBoxVisible )
            functor = SetKeyframeVisibilityFunctor(self,i)            
            editor.addCheckBoxCallback( functor.execute )
            self._sizer.add(editor, 0, GLUtils.GLUI_EXPAND )
            self._editors.append(editor)

        self._container.getParent().layout()            

    def setKeyframeVisibility(self, i, visible):
        """Sets wheter the keyframes of the editor at index i should be visible/editable or not."""
        if visible == self.isKeyframeVisible(i) : return
        time = self._times[i]
        if visible:
            for handle in self._handles:
                handle.addKeyframeAt(time)
        else:
            for handle in self._handles:
                index = handle.getIndexForTime(time)
                if index is not None :
                    handle.removeKeyframe( index )
        
    def isKeyframeVisible(self, i):
        """Returns true if the keyframes of editor at index i are visible/editable."""
        time = self._times[i]
        return self._handles[0].hasKeyframeAtTime(time)
                
    def getHandles(self):
        """Return the list of handles."""
        return self._handles

    def _updateKeyframes(self):
        """Make sure the keyframes are at the right location."""
        self._keyframeTimes = [ time for (time, state) in zip(self._times, self._keyframeVisible) if state ]


    def _addHandle(self, jointName, componentIndex, handleInfo = None, type = 'circular', reverseOppositeJoint = False, minValue = -10000, maxValue = 10000 ):
        """
        Adds a handle to the model.
        handleInfo can be the name of an joint where the handle should be located
        
        """
        oppositeJointName = jointName.replace("STANCE_","TEMP_").replace("SWING_","STANCE_").replace("TEMP_","SWING_")
        try:
            handleJointName = handleInfo.replace("STANCE_","l").replace("SWING_","r")
            posInChild = self._character.getJointByName(handleJointName).getParentJointPosition()
        except AttributeError:
            posInChild = Point3d(0,0,0) 
        
        handle = Handle(self._controller, jointName, componentIndex, type, posInChild, oppositeJointName, reverseOppositeJoint, minValue, maxValue )        
        self._handles.append( handle )
        return handle
        
        
        
class SetKeyframeVisibilityFunctor(object):
    def __init__(self,model,i):
        self._model = model
        self._i = i
    def execute(self,state):
        self._model.setKeyframeVisibility(self._i,state)
