'''
Created on 2009-08-30

@author: beaudoin
'''


try:
    import wx
    from wx import glcanvas
except ImportError:
    raise ImportError, "Required dependency wx.glcanvas not present"

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    raise ImportError, "Required dependency OpenGL not present"

import time
import math
import GLUtils
from MathLib import Vector3d, Point3d, TransformationMatrix

class GLUITopLevelWindow( GLUtils.GLUITopLevelWindow ):
    
    def __init__(self, width, height, window):
        super(GLUITopLevelWindow,self).__init__(width,height)
        self._window = window
        
    def startMouseCapture(self):
        self._window.CaptureMouse()

    def stopMouseCapture(self):
        self._window.ReleaseMouse()
        

class GLPanel(glcanvas.GLCanvas):

    def __init__(self, parent, id = wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0,
                 name='glpane', attribList=(), fps=30):
    
        super(GLPanel, self).__init__(parent, id, pos, size, style, name, attribList)
        
        self._glInitialized = False

        width, height = self.GetSizeTuple()
        self._gluiTopLevelWindow  = GLUITopLevelWindow( width, height, self )
        self._gluiSizer = GLUtils.GLUIBoxSizer( GLUtils.GLUI_VERTICAL )
        self._gluiTopLevelWindow.setSizer( self._gluiSizer )
        
        #
        # Set the event handlers.
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.processEraseBackgroundEvent)
        self.Bind(wx.EVT_SIZE, self.processSizeEvent)
        self.Bind(wx.EVT_PAINT, self.processPaintEvent)

        self.Bind(wx.EVT_LEFT_DOWN, self.processMouseDown)
        self.Bind(wx.EVT_LEFT_UP, self.processMouseUp)
        self.Bind(wx.EVT_MIDDLE_DOWN, self.processMouseDown)
        self.Bind(wx.EVT_MIDDLE_UP, self.processMouseUp)
        self.Bind(wx.EVT_RIGHT_DOWN, self.processMouseDown)
        self.Bind(wx.EVT_RIGHT_UP, self.processMouseUp)
        self.Bind(wx.EVT_MOTION, self.processMouseMotion)

        self.Bind(wx.EVT_LEFT_DOWN, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onLeftDown) )
        self.Bind(wx.EVT_LEFT_UP, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onLeftUp) )
        self.Bind(wx.EVT_LEFT_DCLICK, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onLeftDClick) )
        self.Bind(wx.EVT_MIDDLE_DOWN, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onMiddleDown) )
        self.Bind(wx.EVT_MIDDLE_UP, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onMiddleUp) )
        self.Bind(wx.EVT_MIDDLE_DCLICK, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onMiddleDClick) )
        self.Bind(wx.EVT_RIGHT_DOWN, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onRightDown) )
        self.Bind(wx.EVT_RIGHT_UP, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onRightUp) )
        self.Bind(wx.EVT_RIGHT_DCLICK, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onRightDClick) )
        self.Bind(wx.EVT_MOTION, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onMotion) )
        self.Bind(wx.EVT_MOUSEWHEEL, lambda event: self.processGLUIMouseEvent(event, self._gluiTopLevelWindow.onMouseWheel) )
        
        self.Bind(wx.EVT_IDLE, self.processIdle)

        # Set-up starting state
        self._drawAxes = False
        self._printLoad = False
        self._drawGround = True
        self._groundTexture = None
        self._cameraControl = True
        self._fps = fps
        self._load = 0
        self._loadWindow = []
        self._loadWindowSize = math.ceil(fps)
    
        self._oldX = -1
        self._oldY = -1
        
        self._drawCallbacks = []
        self._postDrawCallbacks = []
        self._oncePerFrameCallbacks = []
        
        self._timeOfNextFrame = 0

        # Set-up initial camera
        self._camera = GLUtils.GLCamera()
        self._camera.setTarget( Point3d(0,1,0) )
        self._camera.modifyRotations( Vector3d( -0.18, -3.141592/2.0, 0 ) )
        self._cameraTargetFunction = None
        
    #
    # wxPython Window Handlers

    def processEraseBackgroundEvent(self, event):
        """Process the erase background event."""
        pass # Do nothing, to avoid flashing on MSWin

    def processSizeEvent(self, event):
        """Process the resize event."""
        if self.IsShown() and self.GetContext():
            # Make sure the frame is shown before calling SetCurrent.
            self.Show()
            self.SetCurrent()

            size = self.GetClientSize()
            self.onReshape(size.width, size.height)
        event.Skip()

    def processPaintEvent(self, event):
        """Process the drawing event."""
        self.SetCurrent()

        # This is a 'perfect' time to initialize OpenGL ... only if we need to
        if not self._glInitialized:
            self.onInitGL()
            self._glInitialized = True

        if self._cameraTargetFunction is not None:
            newTarget = self._cameraTargetFunction( self._camera.getTarget() )
            try: 
                self._camera.setTarget( newTarget )
            except:
                pass

        self.onDraw()
        event.Skip()

    def processGLUIMouseEvent(self, mouseEvent, gluiMethod ):
        """Sends a mouse event to GLUI. Will skip if GLUI did not process the event."""
        
        # If the mouse is captured, but not by GLUI, skip
        if self.HasCapture() and not self._gluiTopLevelWindow.mouseIsCaptured() :
            mouseEvent.Skip()
            return
        
        gluiMouseEvent = _createGLUIMouseEvent(mouseEvent)
        gluiMethod( gluiMouseEvent )
        if gluiMouseEvent.skip : mouseEvent.Skip()

    def processMouseDown(self, event):
        """
        Process a mouse left down event on any button.
        Captures the mouse events
        """
        if self._cameraControl and not self.HasCapture():
            self.CaptureMouse()
        event.Skip()

    def processMouseUp(self, event):
        """Process a mouse up event on any button.
        Releases the mouse events if all button are released
        """        
        self.potentiallyReleaseCapture(event)
        event.Skip()

    def potentiallyReleaseCapture(self, event):
        """Releases mouse capture if no button is pressed"""
        if self.HasCapture() and not ( event.LeftIsDown() or event.MiddleIsDown() or event.RightIsDown() ):
            self.ReleaseMouse()

    def processMouseMotion(self, event):
        """Process mouse motion within the window"""
        x = event.GetX()
        y = event.GetY()
        if self._cameraControl and self.HasCapture() and (self._oldX >= 0 or self._oldY >= 0) :        
            if event.LeftIsDown():
                self._camera.modifyRotations( Vector3d( (self._oldY-y)/100.0, (self._oldX-x)/100.0, 0 ) )
            if event.MiddleIsDown():
                self._camera.translateCamDistance( (self._oldY - y)/200.0 )
            if event.RightIsDown():
                v = Vector3d( (self._oldX - x)/200.0, -(self._oldY - y)/200.0, 0 )
                camToWorld = TransformationMatrix()                
                camToWorld.setToInverseCoordFrameTransformationOf(self._camera.getWorldToCam());
                v = camToWorld * v;

                self._camera.translateTarget(v)

        self._oldX = x
        self._oldY = y
        event.Skip()  

    def processIdle(self, event):
        """Process the idle loop, calls every function that wants to be called once per frame."""
        
        currentTime = time.clock()
        timeLeft = self._timeOfNextFrame - currentTime
        secondPerFrame = 1.0/self._fps
        
        if timeLeft > 0 :
            # Not enough elapsed time
            time.sleep( min(timeLeft, 0.001) )
        else :
            # Enough time elapsed perform simulation loop and render
            self._timeOfNextFrame = currentTime + secondPerFrame
            
            # Call everything that should happen once per frame
            for callback in self._oncePerFrameCallbacks:
                callback()
            
            # Redraw the screen
            self.Refresh(False)
            
            # Compute the load, that is, the percentage of the allotted time spend doing this
            # load = (secondPerFrame - (self._timeOfNextFrame - time.clock())) / secondPerFrame
            load = 1 - (self._timeOfNextFrame - time.clock()) * self._fps
            self._loadWindow.append(load)
            if len(self._loadWindow) >= self._loadWindowSize :
                self._load = sum(self._loadWindow) / float(len(self._loadWindow))
                self._loadWindow = []
            
         
        event.RequestMore()

    #
    # GLFrame OpenGL Event Handlers

    def onInitGL(self):
        """Initialize OpenGL for use in the window."""

        glClearColor(0.0, 0.0, 0.0, 1)
        glShadeModel(GL_SMOOTH)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_TEXTURE_2D)
        glEnable(GL_DEPTH_TEST)
        glDepthFunc(GL_LEQUAL)
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
        glEnable(GL_BLEND)
        glEnable(GL_POINT_SMOOTH)
        glEnable(GL_LINE_SMOOTH)
        glEnable(GL_POLYGON_SMOOTH)
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST)

        glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA)
        glClearColor(0.0, 0.0, 0.0, 1)

        ambient = [0.3, 0.3, 0.3, 1.0]
        diffuse0 = [0.9, 0.9, 0.9, 1.0]
        diffuse = [0.6, 0.6, 0.6, 1.0]
        specular = [0.0, 0.0, 0.0, 0.0]
    
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient)
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0)
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular)
    
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient)
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse)
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular)
    
        glLightfv(GL_LIGHT2, GL_AMBIENT, ambient)
        glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse)
        glLightfv(GL_LIGHT2, GL_SPECULAR, specular)

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular)
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0)
    
        glEnable(GL_LIGHTING)
    
        light0_position = [ 50.0, 10.0, 200.0, 0.0 ]
        light1_position = [200.0, 10.0, -200.0, 0.0 ]
        light2_position = [-200.0, 10.0, -200.0, 0.0 ]
    
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position)
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position)
        glLightfv(GL_LIGHT2, GL_POSITION, light2_position)
    
        glEnable(GL_LIGHT0)
        glEnable(GL_LIGHT1)
        glEnable(GL_LIGHT2)

    def onReshape(self, width, height):
        """Reshape the OpenGL viewport based on the dimensions of the window."""
        glViewport(0, 0, width, height)
        self._gluiTopLevelWindow.setSize( width, height ) 

    def onDraw(self, *args, **kwargs):
        """Draw the window."""
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

        size = self.GetClientSize()

        # Setup the 3D projection matrix
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45.0, size.width/float(size.height),0.1,150.0);

        # Apply the camera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        self._camera.applyCameraTransformations();

        if self._drawGround:
            GLUtils.GLUtils_drawGround(50, 5, 98)

        # Call all 3D drawing callbacks
        for callback in self._drawCallbacks:
            callback()

        if self._drawAxes:
            self.drawAxes()
            
        if self._printLoad:
            self.printLoad()

        # Draw the GLUI
        self._gluiTopLevelWindow.refresh()

        self.SwapBuffers()

        # Call all 3D post drawing callbacks
        for callback in self._postDrawCallbacks:
            callback()

    #
    # Private methods

    def drawAxes(self):
        
        glPushMatrix()
        glLoadIdentity()
        glTranslated(-3,-2.0,-6.0)
        rot = self._camera.getRotations()
        glRotated(-180/math.pi * rot.x,1.0,0.0,0.0)
        glRotated(-180/math.pi * rot.y,0.0,1.0,0.0)
        glRotated(-180/math.pi * rot.z,0.0,0.0,1.0)
        glDisable(GL_DEPTH_TEST)
        GLUtils.GLUtils_drawAxes(0.5)
        glEnable(GL_DEPTH_TEST)
        glPopMatrix()
        
    def printLoad(self):

        loadString = "FPS: %d  Load: %6.2f%%" % (self._fps, (self._load*100))
        size = self.GetClientSize()

        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        glOrtho(size.width,0,size.height,0,-1,1)

        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        glDisable(GL_DEPTH_TEST)


        glColor4f(0,0,0,1)
        glRasterPos2f(200, 15)
        for c in loadString:
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c));        

        glEnable(GL_DEPTH_TEST)
        
        glPopMatrix()
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glMatrixMode(GL_MODELVIEW)

    #
    # Accessors
    
    def setDrawAxes(self, drawAxes):
        """Controls whether the axes should be drawn or not."""
        self._drawAxes = drawAxes
        
    def getDrawAxes(self):
        """Indicates whether the application draws the axes or not."""
        return self._drawAxes
    
    def setPrintLoad(self, printLoad):
        """Controls whether the load should be printed or not."""
        self._printLoad = printLoad
        
    def getPrintLoad(self):
        """Indicates whether the load should be printed or not."""
        return self._printLoad
    
    def setDrawGround(self, drawGround):
        """Controls whether the ground should be drawn or not."""
        self._drawGround = drawGround
        
    def getDrawGround(self):
        """Indicates whether the application draws the ground or not."""
        return self._drawGround
    
    def setCameraControl(self, cameraControl):
        """Controls whether the camera can be controlled with the mouse."""
        self._cameraControl = cameraControl
        
    def getCameraControl(self):
        """Indicates whether the camera can be controlled with the mouse or not."""
        return self._cameraControl
    
    def getFps(self):
        """Return the desired frame per second for this window."""
        return self._fps
    
    def getLoad(self):
        """Return the load during the previous frame (in % of desired FPS)."""
        return self._load
    
    def setCameraTargetFunction(self, cameraTargetFunction):
        """Specifies a function that can be called to identify which point the panel should be looking at.
        This function must take one Point3d parameter that indicates the current camera target.
        This function must return a Point3d or None if the camera shouldn't change its current target.
        You can set the CameraTargetFunction to None to disable the feature."""
        self._cameraTargetFunction = cameraTargetFunction

    def setCameraAutoOrbit(self, autoOrbit):
        """Indicates whether the camera should automatically orbit or not"""
        self._camera.setAutoOrbit(autoOrbit)
            
    def doesCameraAutoOrbit(self):
        """Checks if the camera is currently automatically orbiting."""
        return self._camera.doesAutoOrbit()    
    
    #
    # Public methods
    def addGLUITool(self, gluiToolClass, *args, **kwargs):
        """Adds a GLUI tool to the list of available tools. The parameter passed should be a non-instanciated 
        GLUIWindow derived class for which the constructor accept a parent window.
        Returns the newly created GLUIWindow of the tool.
        Any extra parameter provided are passed to the tool's constructor."""
        gluiTool = gluiToolClass(self._gluiTopLevelWindow, *args, **kwargs)
        self._gluiSizer.add(gluiTool)
        self._gluiSizer.layout()
        return gluiTool
        

    def addDrawCallback(self, callback):
        """Adds a method that should be called when the system is drawing 3D meshes
        The order in which the registered callbacks are called is not guaranteed
        """
        self._drawCallbacks.append( callback )
    
    def addPostDrawCallback(self, callback):
        """Adds a method that should be called whenever the system has finished drawing the OpenGL window
        The order in which the registered callbacks are called is not guaranteed
        """
        self._postDrawCallbacks.append( callback )
    
    def addOncePerFrameCallback(self, callback):
        """Adds a method that should be called exactly once per frame
        The order in which the registered callbacks are called is not guaranteed
        """
        self._oncePerFrameCallbacks.append( callback )

    def removeDrawCallback(self, callback):
        """Removes a previously added draw callback"""
        self._drawCallbacks.remove( callback )
    
    def removeDrawInterfaceCallback(self, callback):
        """Removes a previously added draw interface callback"""
        self._drawCallbacks.remove( callback )
    
    def removeOncePerFrameCallback(self, callback):
        """Removes a previously added once-per-frame callback"""
        self._oncePerFrameCallbacks.remove( callback )
    
    def beginShadows(self):
        """This method should be called before drawing shadows
        After calling this method, the application should draw any mesh for which
        it wants a shadow. Make sure you do not call glColor when drawing the meshes.
        Call endShadows() when complete
        """
        
        # Setup constants
        n = Vector3d(0,1,0)        # Ground normal
        d = Vector3d(-150,200,200) # Light direction

        # Draw a 1 on the stencil buffer everywhere on the ground
        glClear(GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        GLUtils.GLUtils_drawGround(50,5,98);

        # Setup the stencil to write only on the ground
        glStencilFunc(GL_LESS, 0, 0xffffffff);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

        # Offset the shadow polygons
        glPolygonOffset(-2.0, -2.0);
        glEnable(GL_POLYGON_OFFSET_FILL);

        # Draw semitransparent black shadows
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0, 0.0, 0.0, 0.5);

        # Setup the ModelView matrix to a skewed & flattening projection
        glPushMatrix();
        dot = n.dotProductWith(d);

        mat = [ dot - n.x*d.x,   -n.x * d.y,            -n.x * d.z,            0,
               -n.y * d.x,            dot - n.y * d.y,    -n.y * d.z,            0,
               -n.z * d.x,            - n.z * d.y,        dot - n.z * d.z,    0,
               0,                     0,                    0,               dot ]

        glMultMatrixd(mat);
    
    def endShadows(self):
        """This method should be called after shadows have been drawn.
        It should always be called to close a call to beginShadows()"""
        
        glPopMatrix();
    
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_STENCIL_TEST);

    def saveScreenshot(self, filename):

        size = self.GetClientSize()
        GLUtils.GLUtils_saveScreenShot( filename, 0, 0, size.width, size.height)
    
    
def _createGLUIMouseEvent( mouseEvent ):
    """Converts a wx mouse event to a GLUIMouseEvent."""
    result = GLUtils.GLUIMouseEvent()
    result.altDown = mouseEvent.m_altDown
    result.controlDown = mouseEvent.m_controlDown
    result.leftDown = mouseEvent.m_leftDown
    result.middleDown = mouseEvent.m_middleDown
    result.rightDown = mouseEvent.m_rightDown
    result.metaDown = mouseEvent.m_metaDown
    result.shiftDown = mouseEvent.m_shiftDown
    result.x = mouseEvent.m_x
    result.y = mouseEvent.m_y
    result.wheelRotation = mouseEvent.m_wheelRotation
    result.wheelDelta = mouseEvent.m_wheelDelta
    result.linesPerAction = mouseEvent.m_linesPerAction
    result.moving = mouseEvent.Moving()
    result.dragging = mouseEvent.Dragging()
    
    return result
    
    