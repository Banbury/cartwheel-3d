'''
Created on 2009-11-23

@author: beaudoin
'''

import wx, GLUtils, PyUtils, Core, Physics, Controllers

from CharacterDescription import CharacterDescription
from CharacterScaler import CharacterScalerFront
from ToolSet import ToolSet

class Model(object):
    
    def __init__(self):
        """Creates a new model for a character description, including the required UI."""
        
        app = wx.GetApp()
        glCanvas = app.getGLCanvas()             
        
        self._container = glCanvas.addGLUITool( GLUtils.GLUIContainer )
        self._container.setVisible(False)

        self._optionsObservable = PyUtils.Observable()

        self._sizer = GLUtils.GLUIBoxSizer(GLUtils.GLUI_HORIZONTAL)
        self._container.setSizer(self._sizer)
        self.reset()
                       
        self._toolSet = ToolSet(app.getToolPanel(), self)

    def isSymmetric(self):
        """True if the character is forced to be symmetrical."""
        return self._characterDescription.isSymmetric()

    def setSymmetric(self, value):
        """Indicates if the character is forced to be symmetrical."""
        if value == self.isSymmetric() : return
        self._characterDescription.setSymmetric(value)
        self._optionsObservable.notifyObservers()

    def displayInterface(self, display):
        """Indicates wheter the interface for scaling the character should be displayed."""
        if display != self.getDisplayInterface() :
            self._container.setVisible(display)
            self._container.getParent().layout()
            self._optionsObservable.notifyObservers()
        
    def getDisplayInterface(self):
        """Indicates wheter the interface for scaling the character is currently displayed."""
        return self._container.isVisible()
    
    def addOptionsObserver(self, observer):
        """Adds an observer for the options of the instant character model."""
        self._optionsObservable.addObserver(observer)
        
    def reset(self):
        """Resets character description to default values."""
        self._characterDescription = CharacterDescription()
        self._container.detachAllChildren()
        self._frontScaler = CharacterScalerFront( self._container, characterDescription = self._characterDescription, minWidth = 250, minHeight = 500 )
#        self._sideScaler = CharacterScalerSide( self._container, characterDescription = self._characterDescription, minWidth = 125, minHeight = 250 )
        self._sizer.add(self._frontScaler)
#        self._sizer.add(self._sideScaler)
        self._container.getParent().layout()
        self._optionsObservable.notifyObservers()
        
    def create(self):
        """Creates the instant character based on the available description. Attach a reasonable controller, if available."""       
        app = wx.GetApp()
        
        try:
            wrappedController = PyUtils.wrapCopy( app.getController(0) )
        except IndexError:
            wrappedController = None
                
        try:
            character = app.getCharacter(0)
            previousMass = character.getMass()
        except IndexError:
            previousMass = None
        
        
        app.deleteAllObjects()
        PyUtils.load( "RigidBodies.FlatGround" )
        character = self._characterDescription.createCharacter()
        character.computeMass()
        app.addCharacter(character)

        if wrappedController is not None:
            controller = wrappedController.createAndFillObject(None, character)
            if previousMass is not None:
                massRatio = character.getMass() / previousMass
                controller.scaleGains( massRatio )
            app.addController(controller)
            controller.setStance( Core.LEFT_STANCE )
            self.connectController()
        
        return character

    def connectController(self, useCurrentSliders = True):
        """Connects the current controller to a behaviour."""
        
        app = wx.GetApp()
        worldOracle = app.getWorldOracle()
        character = app.getCharacter(0)
        controller = app.getController(0)
        
        behaviour = Core.TurnController(character, controller, worldOracle)
        behaviour.initializeDefaultParameters()
        controller.setBehaviour(behaviour)

        if useCurrentSliders :
            behaviour.requestVelocities(self._toolSet.getCurrentSpeed(), 0);
            behaviour.requestStepTime(self._toolSet.getCurrentDuration());
            behaviour.requestCoronalStepWidth(self._toolSet.getCurrentStepWidth());
        behaviour.requestHeading(0);
        behaviour.conTransitionPlan();
        
        self._toolSet.update()
        
        app.takeSnapshot()
        
        
    def getDesiredDuration(self):
        """Return the desired duration of a step."""
        try:
            return wx.GetApp().getController(0).getBehaviour().getDesiredStepTime()
        except Exception:
            return 0
        
    def setDesiredDuration(self, duration):
        """Sets the desired duration of a step."""
        try:
            wx.GetApp().getController(0).getBehaviour().requestStepTime(duration)
        except Exception:
            pass
        
    def getDesiredSpeed(self):
        """Return the desired speed of the character."""
        try:
            return wx.GetApp().getController(0).getBehaviour().getDesiredVelocitySagittal()
        except Exception:
            return 0
        
    def setDesiredSpeed(self, speed):
        """Sets the desired speed of the character."""
        try:
            wx.GetApp().getController(0).getBehaviour().requestVelocities(speed, 0)
        except Exception:
            pass
        
    def getDesiredStepWidth(self):
        """Return the desired step width of the character."""
        try:
            return wx.GetApp().getController(0).getBehaviour().getCoronalStepWidth()
        except Exception:
            return 0
        
    def setDesiredStepWidth(self, stepWidth):
        """Sets the desired step width of the character."""
        try:
            wx.GetApp().getController(0).getBehaviour().requestCoronalStepWidth(stepWidth)
        except Exception:
            pass