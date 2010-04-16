'''
Created on 2009-08-26

@author: beaudoin
'''

from OpenGL.GL import *
from OpenGL.GLU import *
import PyUtils, wx, Physics, Utils, time, math, sys, Core
from ObservableList import ObservableList
from Curve import Curve
from SnapshotTree import SnapshotBranch 
from MathLib import Vector3d, Point3d

def _printout( text ):
    """Private. Redirect the passed text to stdout."""
    sys.stdout.write(text)
    
# Make sure the DLLs use the python stdout for printout
Utils.registerPrintFunction( _printout )

class SNMApp(wx.App):
    """A simple class that should handle almost everything a simbicon application typically needs."""

    def __init__(self, appTitle="Simbicon Application", 
                 fps = 30.0,
                 dt = 1/2000.0,
                 glCanvasSize=wx.DefaultSize,
                 size=wx.DefaultSize, redirect=False, filename=None,
                 useBestVisual=False, clearSigInt=True, showConsole=True):
        """
        appTitle is the window title
        fps is the desired number of frames per seconds
        dt is the desired simulation timestep
        :see: wx.BasicApp.__init__`
        """
        
        wx.App.__init__(self, redirect, filename, useBestVisual, clearSigInt)

        # No annoying error logging window
        wx.Log.SetActiveTarget(wx.LogStderr())

        import UI

        # Setup the main window style
        style = wx.DEFAULT_FRAME_STYLE
        if size == wx.DefaultSize :
            size = wx.GetDisplaySize()
            size.height *= 0.75        
            size.width *= 0.75
            if glCanvasSize == wx.DefaultSize :
                style |= wx.MAXIMIZE
       
        # Setup the environment for the python interactive console
        consoleEnvironment = {
            "wx" : wx,
            "Physics" : Physics,
            "Utils" : Utils }
        exec "from MathLib import *\n" + \
             "app = wx.GetApp()\n" + \
             "from PyUtils import load" in consoleEnvironment, consoleEnvironment
       
        # Create the main window
        self._frame = UI.MainWindow(None, -1, appTitle, size = size, style = style, 
                                    fps = fps, glCanvasSize = glCanvasSize,
                                    showConsole = showConsole,
                                    consoleEnvironment = consoleEnvironment)
        
        # Define GL callbacks
        self._glCanvas = self._frame.getGLCanvas()
        self._glCanvas.addDrawCallback( self.draw )
        self._glCanvas.addPostDrawCallback( self.postDraw )
        self._glCanvas.addOncePerFrameCallback( self.advanceAnimation )
        self._glCanvas.setDrawAxes(False)
        self._glCanvas.setPrintLoad(True)
        self._glCanvas.setCameraTargetFunction( self.cameraTargetFunction )
        
        # Get the tool panel
        self._toolPanel = self._frame.getToolPanel()
        
        # Show the application
        self._frame.Show()
        
        # Set-up starting state
        self._dt = dt      
        self._drawShadows = True
        self._simulationSecondsPerSecond = 1   # 1 = real time, 2 = twice real time, 1/2 = half real time
        self._animationRunning = False
        self._cameraFollowCharacter = False
        self._drawCollisionVolumes = False
        self._followedCharacter = None # Pointer to focused character
        self._captureScreenShots = False
        self._printStepReport = True
        self._screenShotNumber = 0
        self._worldOracle = Core.WorldOracle()
        self._worldOracle.initializeWorld( Physics.world() )
        self._kinematicMotion = False
        
        # Set-up starting list of characters and controllers
        self._characters = []
    
        # Define the observables
        self._controllerList = ObservableList() 
        self._characterObservable = PyUtils.Observable()
        self._animationObservable = PyUtils.Observable()
        self._cameraObservable = PyUtils.Observable()
        self._optionsObservable = PyUtils.Observable()
        self._curveList = ObservableList()
        self._snapshotTree = SnapshotBranch()
            
    #
    # Private methods
        
    def draw(self):
        """Draw the content of the world"""
        world = Physics.world()
                    
        glEnable(GL_LIGHTING)
        if self._drawCollisionVolumes:
            world.drawRBs(Physics.SHOW_MESH|Physics.SHOW_CD_PRIMITIVES)
        else:
            world.drawRBs(Physics.SHOW_MESH|Physics.SHOW_COLOURS)            
#        world.drawRBs(Physics.SHOW_MESH|Physics.SHOW_CD_PRIMITIVES)
        glDisable(GL_LIGHTING);
    
        if self._drawShadows:
            self._glCanvas.beginShadows()
            world.drawRBs(Physics.SHOW_MESH)
            self._glCanvas.endShadows()    

    def postDraw(self):
        """Perform some operation once the entire OpenGL window has been drawn"""
        if self._captureScreenShots:
            self._glCanvas.saveScreenshot("../screenShots/%04d.bmp" % self._screenShotNumber )
            self._screenShotNumber += 1

    def advanceAnimation(self):
        """Called once per frame"""
        if self._animationRunning :
            self.simulationFrame()

    def simulationFrame(self):
        """Performs enough simulation steps to fill one frame"""
        
        # Enough time elapsed perform simulation loop and render
        simulationSeconds = 1.0/self._glCanvas.getFps() * self._simulationSecondsPerSecond
        nbSteps = int( math.ceil( simulationSeconds / self._dt ) )
 
        for i in range(0,nbSteps):
            self.simulationStep()

    def advanceAnimationUntilControllerEnds(self, controller):
        """Advances the animation until the specified controller reaches the end.
        Specify either a name, an index, or an instance of a controller object."""
        controller = self.getController(controller)
        initialPhi = controller.getPhase()
        currPhi = initialPhi + 1
        while currPhi > initialPhi :
            self.simulationStep()
            currPhi = controller.getPhase()
        
    def simulationStep(self):
        """Performs a single simulation step"""

        # TODO Quite hacky
        if self._kinematicMotion:
            import KeyframeEditor 
            from MathLib import Trajectory3dv
            try:
                pc = self._posableCharacter
                traj = self._stanceFootToSwingFootTrajectory
            except AttributeError:
                pc = self._posableCharacter = KeyframeEditor.PosableCharacter.PosableCharacter(self.getCharacter(0),self.getController(0))        
                traj = self._stanceFootToSwingFootTrajectory = Trajectory3dv()
                traj.addKnot(0,Vector3d(-0.13,0,-0.4))
                traj.addKnot(0.5,Vector3d(-0.13,0.125,0))
                traj.addKnot(1,Vector3d(-0.13,0,0.4))
                self._phase = 0                        
                self._stance = Core.LEFT_STANCE
                
            stanceToSwing = traj.evaluate_catmull_rom(self._phase)
            if self._stance == Core.RIGHT_STANCE:
                stanceToSwing.x = stanceToSwing.x * -1
            pc.updatePose( self._phase, stanceToSwing, self._stance, True )
            
            self._phase += 0.00069
            if self._phase >= 1.0:
                self._phase = 0
                if self._stance == Core.LEFT_STANCE:
                    self._stance = Core.RIGHT_STANCE
                else:
                    self._stance = Core.LEFT_STANCE
            return


        
        world = Physics.world()
        controllers = self._controllerList._objects
        contactForces = world.getContactForces()
        for controller in controllers :
            controller.performPreTasks(self._dt, contactForces)
        world.advanceInTime(self._dt)
        
        contactForces = world.getContactForces()
        for controller in controllers :
            if controller.performPostTasks(self._dt, contactForces) :
                step = Vector3d (controller.getStanceFootPos(), controller.getSwingFootPos())
                step = controller.getCharacterFrame().inverseRotate(step);
                v = controller.getV()
                phi = controller.getPhase()
                if self._printStepReport:
                    print "step: %3.5f %3.5f %3.5f. Vel: %3.5f %3.5f %3.5f  phi = %f" % ( step.x, step.y, step.z, v.x, v.y, v.z, phi)

        
    
    def cameraTargetFunction(self, currentTarget):
        """Private! Return the point to target, or None if nothing to target."""
        if not self._cameraFollowCharacter or self._followedCharacter == None:
            return None
        
        pos = self._followedCharacter.getRoot().getCMPosition()
        pos.y = currentTarget.y
        return pos
    

    #    
    # Accessors
    
    def getWorldOracle(self):
        """Return the world oracle for the application."""
        return self._worldOracle
    
    def getFrame(self):
        """Returns the application frame."""
        return self._frame
    
    def setDrawShadows(self, drawShadows):
        """Indicates whether the app should draw shadows or not"""
        self._drawShadows = drawShadows
        
    def getDrawShadows(self):
        """Checks if the app is drawing shadows"""
        return self._drawShadows
    
    def getGLCanvas(self):
        """Returns the GL canvas"""
        return self._glCanvas
    
    def getToolPanel(self):
        """Returns the tool panel"""
        return self._toolPanel
    
    def setAnimationRunning(self, animationRunning):
        """Indicates whether the animation should run or not"""
        self._animationRunning = animationRunning
        self._animationObservable.notifyObservers()

    def isAnimationRunning(self):
        """Return true if the animation is currently running"""
        return self._animationRunning

    def setSimulationSecondsPerSecond(self, simulationSecondsPerSecond):
        """Sets the speed of the playback. 1 is realtime, 0.5 is slower, 2 is faster"""
        self._simulationSecondsPerSecond = simulationSecondsPerSecond
        self._animationObservable.notifyObservers()

    def getSimulationSecondsPerSecond(self):
        """Return the speed of the playback"""
        return self._simulationSecondsPerSecond
    
    def setCameraFollowCharacter(self, follow):
        """Indicates whether the camera should follow a character or not"""
        if follow != self._cameraFollowCharacter:
            # Need to toggle
            self._cameraFollowCharacter = follow
            if self._followedCharacter == None :
                try: self._followedCharacter = self._characters[0]
                except IndexError: pass
            self._cameraObservable.notifyObservers()
    
    def doesCameraFollowCharacter(self):
        """Checks if the camera is currently following a character."""
        return self._cameraFollowCharacter  and  self._followedCharacter != None
    
    def setFollowedCharacter(self, character):
        """Indicates which character the camera should be following. Pass an index of a string."""
        character = self.getCharacter(character)

        self._cameraFollowCharacter = True
        self._followedCharacter = character
        self._cameraObservable.notifyObservers()

    def setCameraAutoOrbit(self, autoOrbit):
        """Indicates whether the camera should automatically orbit or not"""
        self._glCanvas.setCameraAutoOrbit(autoOrbit)
            
    def doesCameraAutoOrbit(self):
        """Checks if the camera is currently automatically orbiting."""
        return self._glCanvas.doesCameraAutoOrbit()    
        
    def drawCollisionVolumes(self, draw):
        """Indicates whether the application should draw collision volumes"""
        if draw != self._drawCollisionVolumes:
            self._drawCollisionVolumes = draw
            self._optionsObservable.notifyObservers()
    
    def getDrawCollisionVolumes(self):
        """Does the application draw collision volumes?"""
        return self._drawCollisionVolumes
        
    def setKinematicMotion(self, kinematicMotion):
        """Indicates whether the application should animate only kinematic motion"""
        if kinematicMotion != self._kinematicMotion:
            self._kinematicMotion = kinematicMotion
            self._optionsObservable.notifyObservers()
    
    def getKinematicMotion(self):
        """Does the application animate only kinematic motion?"""
        return self._kinematicMotion
    
    def captureScreenShots(self, capture):
        """Indicates whether the application should capture a screenshot at every frame."""
        if capture != self._captureScreenShots:
            self._captureScreenShots = capture
            self._optionsObservable.notifyObservers()
    
    def getCaptureScreenShots(self):
        """Does the application capture a screenshot at every frame?"""
        return self._captureScreenShots
    
    #
    # Public methods
       
    def deleteAllObjects(self):
        """Delete all objects: characters, rigid bodies, snapshots, etc."""
        if self._followedCharacter is not None :           
            self._followedCharacter = None
            self._cameraFollowCharacter = False
            self._cameraObservable.notifyObservers()        
        self._characters = []
        self._controllerList.clear()
        import Physics
        Physics.world().destroyAllObjects()
        self.deleteAllSnapshots()        
       
    def addCharacter(self, character):
        """Adds a character to the application and the world"""
        import Physics
        if PyUtils.sameObjectInList(character, self._characters) :
            raise KeyError ('Cannot add the same character twice to application.')
        Physics.world().addArticulatedFigure( character )
        self._characters.append( character )
        if self._followedCharacter is None :
            self._followedCharacter = character
            self._cameraFollowCharacter = True
            self._cameraObservable.notifyObservers()
        self._characterObservable.notifyObservers()

    def deleteCharacter(self, character):
        """Removes a character from the application. Specify either a name, an index, or an instance of a character object."""        
        character = self.getCharacter(character)
        if self._followedCharacter is character :           
            self._followedCharacter = None
            self._cameraFollowCharacter = False
            self._cameraObservable.notifyObservers()
        self._characters.remove(character)
        self._characterObservable.notifyObservers()
        
    def getCharacter(self, description ):
        """Returns a character. Specify either a name, an index. Anything else will be returned unmodified."""
        if isinstance(description,basestring) :
            try: 
                description = [char.getName() for char in self._characters].index(description)
            except ValueError: raise ValueError( "No character found with the specified name." )
        if isinstance(description,int) :
            return self._characters[description]
        return description
    
    def getCharacterCount(self):
        """Returns the number of characters."""
        return len( self._character )
    
    def recenterCharacter(self, character):
        """Reposition the character at the center of the world in X,Z. Specify either a name, an index, or an instance of a character object."""
        character = self.getCharacter(character)
        character.recenter()
        
    
    def addController(self, controller):
        """Adds a controller to the application"""
        return self._controllerList.add(controller)

    def deleteController(self, controller):
        """Removes a controller from the application. Specify either a name, an index, or an instance of a controller object."""        
        return self._controllerList.delete(controller)

    def getController(self, description):        
        """Returns a controller. Specify either a name, an index. Anything else will be returned unmodified."""
        return self._controllerList.get(description)

    def getControllerCount(self):
        """Returns the number of controllers."""
        return self._controllerList.getCount()
        
    def getControllerList(self):
        """Returns the controller list object. Useful for observation."""
        return self._controllerList
    
    
    def addCurve(self, name, trajectory1d, phiPtr = None):
        """Adds a curve to the application"""
        return self._curveList.add( Curve(name, trajectory1d, phiPtr) )

    def deleteCurve(self, curve):
        """Removes a curve from the application. Specify either a name, an index, or an instance of a controller object."""        
        return self._curveList.delete(curve)

    def clearCurves(self):
        """Remove all the curves from the application."""
        self._curveList.clear();
    
    def getCurve(self, description):
        """Returns a curve. Specify either a name, an index. Anything else will be returned unmodified."""
        return self._curveList.get(description)

    def getCurveCount(self):
        """Returns the number of curves."""
        return self._curveList.getCount()
        
    def getCurveList(self):
        """Returns the curve list object. Useful for observation."""
        return self._curveList
    
    def getSnapshotTree(self):
        """Returns the top-level SnapshotBranch that can be observed.""" 
        return self._snapshotTree
    
    def takeSnapshot(self):
        """Take a snapshot of the world.
        The snapshot will be returned and added to the snapshot tree."""
        return self._snapshotTree.takeSnapshot()
    
    def restoreActiveSnapshot(self, restoreControllerParams = True):
        """Restores the current snapshot. Return it."""
        return self._snapshotTree.restoreActive(restoreControllerParams)

    def previousSnapshot(self, restoreControllerParams = True):
        """Navigate to the previous snapshot. Return it, or None if failed."""
        return self._snapshotTree.previousSnapshot(restoreControllerParams)
    
    def nextSnapshot(self, restoreControllerParams = True):
        """Navigate to the next snapshot. Return it, or None if failed."""
        return self._snapshotTree.nextSnapshot(restoreControllerParams)

    def deleteAllSnapshots(self):
        """Delete all the snapshots of the world."""
        self._snapshotTree = SnapshotBranch()    
    #
    # For observers
    #    
    def addCharacterObserver(self, observer):
        self._characterObservable.addObserver(observer)
    
    def deleteCharacterObserver(self, observer):
        self._characterObservable.deleteObserver(observer)
    
    def addControllerObserver(self, observer):
        self._controllerList.addObserver(observer)
    
    def deleteControllerObserver(self, observer):
        self._controllerList.deleteObserver(observer)
    
    def addAnimationObserver(self, observer):
        self._animationObservable.addObserver(observer)
    
    def deleteAnimationObserver(self, observer):
        self._animationObservable.deleteObserver(observer)        
    
    def addCameraObserver(self, observer):
        self._cameraObservable.addObserver(observer)
    
    def deleteCameraObserver(self, observer):
        self._cameraObservable.deleteObserver(observer)
        
    def addOptionsObserver(self, observer):
        self._optionsObservable.addObserver(observer)
    
    def deleteOptionsObserver(self, observer):
        self._optionsObservable.deleteObserver(observer)
        
    