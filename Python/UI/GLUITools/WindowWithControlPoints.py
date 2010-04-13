'''
Created on 2009-12-02

@author: beaudoin
'''

import GLUtils
from OpenGL.GL import *

class ControlPoint(object):
    """Base class for any type of control point. 
    Derived classes must implement:
       getPos()  returns the x, y position of the control point."""
        
    def __init__( self ):
        super(ControlPoint,self).__init__()
        self._previousMousePos = None
    
    def draw(self):
        xPos, yPos = self.getPos()
        glVertex2d( xPos, yPos )

    def setPreviousMousePos(self, pos ):
        self._previousMousePos = pos

    def unsetPreviousMousePos(self):
        self._previousMousePos = None
        
    def moveToPos(self, pos):
        if self._previousMousePos is None :
            self._previousMousePos = pos
        self.setPos(pos)
        self._previousMousePos = pos
        
class WindowWithControlPoints(GLUtils.GLUIWindow):
    """
    Base class for a simple GL window that can contain various control points.
    Derived class should not perform their drawing in draw() but instead in 
        drawContent()
    The control points will be drawn on top of that.
    At that point, the drawing area will be configured to the specified values (minPosY, maxPosY 
    """
    
    def __init__( self, parent, x=0, y=0, width=0, height=0, minWidth=-1, minHeight=-1, boundsX = (-1,1), boundsY = (-1,1), forceAspectRatio = None ):
        """
        Initializes a window that can have control points.
           boundsX indicate the (min, max) X values to display
           boundsY indicate the (min, max) Y values to display
           forceAspectRatio can be 'x', 'y' or None.
              If 'x', the boundsX will be modified to enforce conservation of aspect ratio
              If 'y', the boundsY will be modified to enforce conservation of aspect ratio
              If None, aspect ratio is not adjusted by the window      
        See GLUIWindow for other parameters.
        """        
        
        super(WindowWithControlPoints,self).__init__(parent,x,y,width,height, minWidth, minHeight)
        
        self._controlPoints = []
        
        self._controlPointHeld = None
        self._holdPos = (-1,-1)
        
        self._boundsX = boundsX
        self._boundsY = boundsY

        self._width  = float(boundsX[1] - boundsX[0])
        self._height = float(boundsY[1] - boundsY[0])
        
        if forceAspectRatio is not None:
            if forceAspectRatio == 'x' :
                self._width = self._height * float(minWidth) / float(minHeight)
                midX = (boundsX[0] + boundsX[1])/2.0
                self._boundsX = (midX - self._width/2.0, midX + self._width/2.0) 
            elif forceAspectRatio == 'y' :
                self._height = width * float(minHeight) / float(minWidth)
                midY = (boundsY[0] + boundsY[1])/2.0
                self._boundsY = (midY - self._height/2.0, midY + self._height/2.0) 
            else :
                raise ValueError( "forceAspectRation must be 'x', 'y' or None" )

    def draw(self):
        """Draws the content of the window by calling self.drawContent(), then the control points."""

        # Save projection matrix and sets it to a simple orthogonal projection
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        glOrtho(self._boundsX[0], self._boundsX[1], self._boundsY[0], self._boundsY[1],-1,1)
        
        self.drawContent()
    
        # Draw control points
        try:
            glPointSize( 8.0 )
            glColor3d(1,1,0)
            glBegin( GL_POINTS );
            for controlPoint in self._controlPoints:
                controlPoint.draw()
            glEnd()
                   
        except Exception as e:
            glEnd()
            print "Exception while drawing WindowWithControlPoints interface: " + str(e)
            traceback.print_exc(file=sys.stdout)
            
        # Restore projection to pixel mode
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()

    def deleteAllControlPoints(self):
        """Delete all the control points."""
        self._controlPoints = []

    def addControlPoint(self, controlPoint):
        """Adds a control point to the window."""
        self._controlPoints.append( controlPoint )
        
    def onLeftDown(self, mouseEvent):
        """Left mouse button pressed."""
        
        x, y = mouseEvent.x, mouseEvent.y
        self._captureControlPoint( self._findClosestControlPoint(x,y), x, y ) 
        return True
        
    def onLeftUp( self, mouseEvent ):
        self._checkReleaseCapture(mouseEvent)
        return True;
        
    def _posToPixel(self, x, y):
        size = self.getSize()
        return int((x-self._boundsX[0])*size.width/self._width), \
               size.height-int((y-self._boundsY[0])*size.height/self._height)
        
    def _pixelToPos(self, x, y):
        size = self.getSize()
        return self._boundsX[0] + x*self._width/float(size.width), \
               self._boundsY[0] - (y - size.height)*self._height/float(size.height) 
               
        
    def _findClosestControlPoint(self, x,y):
        for controlPoint in self._controlPoints:
            distX, distY = self._posToPixel( *controlPoint.getPos() )
            distX -= x
            distY -= y
            if distX*distX + distY*distY < 25 : 
                return controlPoint;
        return None;

    def _captureControlPoint( self, controlPoint, x, y ):
        if self.hasCapture(): return
        self._controlPointHeld = controlPoint
        if controlPoint is None : return
        posX, posY = self._pixelToPos( x, y )
        controlPoint.setPreviousMousePos( (posX, posY) )
        self._holdPos = (x,y)
        self.captureMouse()
    
    def _checkReleaseCapture( self, mouseEvent ):
        if not self.hasCapture(): return
        if mouseEvent.leftDown or mouseEvent.rightDown : return

        self.releaseMouse()
        if self._controlPointHeld is not None :
            self._controlPointHeld.unsetPreviousMousePos()
        self._controlPointHeld = None
        self._holdPos = (-1,-1)
        
        
    def onMotion( self, mouseEvent ):

        if not mouseEvent.leftDown and not mouseEvent.rightDown: return True
        if not self.hasCapture(): return True
        if self._controlPointHeld is None : return True

        posX, posY = self._pixelToPos( mouseEvent.x, mouseEvent.y )
        self._controlPointHeld.moveToPos( (posX, posY) )

        return True

