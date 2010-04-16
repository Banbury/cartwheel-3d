'''
Created on 2009-11-20

@author: beaudoin
'''

from OpenGL.GL import *
import wx, GLUtils, PyUtils, math, traceback, sys
import UI

class CharacterScaler(UI.GLUITools.WindowWithControlPoints):
    """Base class for a simple GL window that display a character with handles to scale its various elements."""
    
    def __init__( self, parent, characterDescription, x=0, y=0, width=0, height=0, minWidth=-1, minHeight=-1 ):
        
        super(CharacterScaler,self).__init__(parent,x,y,width,height, minWidth, minHeight, boundsY = (-0.1,2.1), forceAspectRatio = 'x')
        
        self._characterDescription = characterDescription
        
class CharacterScalerFront(CharacterScaler):
    """A simple GL window that display a character from the front view with handles to scale its various elements."""
    
    def __init__( self, parent, characterDescription, x=0, y=0, width=0, height=0, minWidth=-1, minHeight=-1 ):
        
        super(CharacterScalerFront,self).__init__(parent,characterDescription,x,y,width,height, minWidth, minHeight)        
        
        self.addControlPoint( FrontFootControlPoint(characterDescription,-1) )
        self.addControlPoint( FrontFootControlPoint(characterDescription,1) )
        self.addControlPoint( FrontKneeControlPoint(characterDescription,-1) )
        self.addControlPoint( FrontKneeControlPoint(characterDescription,1) )
        self.addControlPoint( FrontLegControlPoint(characterDescription,-1) )
        self.addControlPoint( FrontLegControlPoint(characterDescription,1) )
        self.addControlPoint( FrontLegAnchorControlPoint(characterDescription,-1) )
        self.addControlPoint( FrontLegAnchorControlPoint(characterDescription,1) )
        self.addControlPoint( FrontPelvisControlPoint(characterDescription) )
        self.addControlPoint( FrontChestControlPoint(characterDescription) )
        self.addControlPoint( FrontShoulderControlPoint(characterDescription) )
        self.addControlPoint( FrontNeckControlPoint(characterDescription) )
        self.addControlPoint( FrontHeadTopControlPoint(characterDescription) )
        self.addControlPoint( FrontHeadSideControlPoint(characterDescription) )
        self.addControlPoint( FrontElbowControlPoint(characterDescription,-1) )
        self.addControlPoint( FrontElbowControlPoint(characterDescription,1) )
        self.addControlPoint( FrontWristControlPoint(characterDescription,-1) )
        self.addControlPoint( FrontWristControlPoint(characterDescription,1) )

        
        
    def drawContent(self):
        """Draw the character from front view."""
        
        cd = self._characterDescription
        
        glColor3f(1,1,1)
        try:
            for side in (-1,1):
                
                glColor4f( 0.5, 0.5, 0.5, 0.84 )
                glBegin(GL_QUADS)
                halfSize = cd.getFootSizeX(side)/2.0
                glVertex2d( cd.getLegPosX(side) - halfSize, cd.getGroundPosY() )
                glVertex2d( cd.getLegPosX(side) - halfSize, cd.getAnklePosY(side) )
                glVertex2d( cd.getLegPosX(side) + halfSize, cd.getAnklePosY(side) )
                glVertex2d( cd.getLegPosX(side) + halfSize, cd.getGroundPosY() )
                glEnd()
                
                glColor4f( 0.5, 0.6, 0.8, 0.84 )
                glBegin(GL_QUADS)
                halfSize = cd.getLowerLegDiameter(side)/2.0
                glVertex2d( cd.getLegPosX(side) - halfSize, cd.getAnklePosY(side) )
                glVertex2d( cd.getLegPosX(side) - halfSize, cd.getKneePosY(side) )
                glVertex2d( cd.getLegPosX(side) + halfSize, cd.getKneePosY(side) )
                glVertex2d( cd.getLegPosX(side) + halfSize, cd.getAnklePosY(side) )
                glEnd()
                
                glColor4f( 0.5, 0.6, 0.8, 0.84 )
                glBegin(GL_QUADS)
                halfSize = cd.getUpperLegDiameter(side)/2.0
                glVertex2d( cd.getLegPosX(side) - halfSize, cd.getKneePosY(side) )
                glVertex2d( cd.getLegPosX(side) - halfSize, cd.getHipPosY() )
                glVertex2d( cd.getLegPosX(side) + halfSize, cd.getHipPosY() )
                glVertex2d( cd.getLegPosX(side) + halfSize, cd.getKneePosY(side) )
                glEnd()
            
                glColor4f( 0.5, 0.6, 0.8, 0.84 )
                glBegin(GL_QUADS)                
                glVertex2d( 0, cd.getHipPosY() )
                glVertex2d( side*cd.getPelvisDiameter()/2.0, cd.getHipPosY() )
                glVertex2d( side*cd.getPelvisDiameter()/2.0, cd.getWaistPosY() )
                glVertex2d( 0, cd.getWaistPosY() )
                glEnd()
            
                glColor4f( 0.5, 0.8, 0.6, 0.84 )
                glBegin(GL_POLYGON)                
                glVertex2d( 0, cd.getWaistPosY() )
                glVertex2d( side*cd.getTorsoDiameter()/4.0, cd.getWaistPosY() )
                glVertex2d( side*cd.getTorsoDiameter()/2.0, cd.getChestPosY() )
                glVertex2d( side*cd.getTorsoDiameter()/2.0, cd.getShoulderPosY() )
                glVertex2d( 0, cd.getShoulderPosY() )
                glEnd()
                
                glColor4f( 0.892, 0.716, 0.602, 0.84 )
                glBegin(GL_QUADS)
                glVertex2d( 0, cd.getShoulderPosY() )
                glVertex2d( side*0.1*cd.getHeadSizeX(), cd.getShoulderPosY() )
                glVertex2d( side*0.1*cd.getHeadSizeX(), cd.getNeckPosY() )
                glVertex2d( 0, cd.getNeckPosY() )
                glEnd()

                glColor4f( 0.892, 0.716, 0.602, 0.84 )
                glVertex2f( 0, cd.getNeckPosY() + cd.getHeadSizeY()/2.0 )                    
                glBegin(GL_TRIANGLE_FAN)
                for i in range(21):
                    theta = math.pi * i/20.0
                    x = side * math.sin(theta) * cd.getHeadSizeX()/2.0
                    y = math.cos(theta) * cd.getHeadSizeY()/2.0 + cd.getNeckPosY() + cd.getHeadSizeY()/2.0
                    glVertex2f( x, y )                    
                glEnd()

                glColor4f( 0.5, 0.8, 0.6, 0.84 )
                glBegin(GL_QUADS)
                halfSize = cd.getUpperArmDiameter(side)/2.0
                glVertex2d( cd.getArmPosX(side)-halfSize, cd.getShoulderPosY() )
                glVertex2d( cd.getArmPosX(side)-halfSize, cd.getElbowPosY(side) )
                glVertex2d( cd.getArmPosX(side)+halfSize, cd.getElbowPosY(side) )
                glVertex2d( cd.getArmPosX(side)+halfSize, cd.getShoulderPosY() )
                glEnd()

                glColor4f( 0, 0, 0, 1 )
                glBegin(GL_LINES)
                glVertex2d( cd.getArmPosX(side)-side*halfSize, cd.getShoulderPosY() )
                glVertex2d( cd.getArmPosX(side)-side*halfSize, cd.getChestPosY() )
                glEnd()

                glColor4f( 0.892, 0.716, 0.602, 0.84 )
                glBegin(GL_QUADS)
                halfSize = cd.getLowerArmDiameter(side)/2.0
                glVertex2d( cd.getArmPosX(side)-halfSize, cd.getElbowPosY(side) )
                glVertex2d( cd.getArmPosX(side)-halfSize, cd.getWristPosY(side) )
                glVertex2d( cd.getArmPosX(side)+halfSize, cd.getWristPosY(side) )
                glVertex2d( cd.getArmPosX(side)+halfSize, cd.getElbowPosY(side) )
                glEnd()

            # Draw control points
    
            glPointSize( 8.0 )
            glColor3d(1,1,0)
            glBegin( GL_POINTS );
            for controlPoint in self._controlPoints:
                controlPoint.draw()
            glEnd()
        except Exception as e:
            glEnd()
            print "Exception while drawing scaled character interface: " + str(e)
            traceback.print_exc(file=sys.stdout)
            
        
    

class CharacterControlPoint(UI.GLUITools.ControlPoint):
    
    def __init__( self, characterDescription):
        """Pass the character description and a 4-tuple (minX, maxX, minY, maxY)."""
        super(CharacterControlPoint,self).__init__()
        self._characterDescription = characterDescription
        

class SymmetricCharacterControlPoint(CharacterControlPoint):
    
    def __init__( self, characterDescription, side ):
        super(SymmetricCharacterControlPoint,self).__init__(characterDescription)
        self._side = side
    
    def draw(self):
        if self._characterDescription.isSymmetric() and self._side == -1 : return
        super(SymmetricCharacterControlPoint,self).draw()
        
class FrontFootControlPoint(SymmetricCharacterControlPoint):
    def __init__(self, characterDescription, side):
        super(FrontFootControlPoint,self).__init__(characterDescription, side)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getLegPosX(self._side) + self._side*cd.getFootSizeX(self._side)/2.0, cd.getAnklePosY(self._side) )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setFootSizeX( self._side, (pos[0]-cd.getLegPosX(self._side))*2.0*self._side )        
        cd.setAnklePosY( self._side, pos[1] )
         
class FrontKneeControlPoint(SymmetricCharacterControlPoint):
    def __init__(self, characterDescription, side):
        super(FrontKneeControlPoint,self).__init__(characterDescription, side)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getLegPosX(self._side) + self._side*cd.getLowerLegDiameter(self._side)/2.0, cd.getKneePosY(self._side) )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setLowerLegDiameter( self._side, (pos[0]-cd.getLegPosX(self._side))*2.0*self._side )
        cd.setKneePosY( self._side, pos[1] )
         
class FrontLegControlPoint(SymmetricCharacterControlPoint):
    def __init__(self, characterDescription, side):
        super(FrontLegControlPoint,self).__init__(characterDescription, side)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getLegPosX(self._side) + self._side*cd.getUpperLegDiameter(self._side)/2.0, cd.getHipPosY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setUpperLegDiameter( self._side, (pos[0]-cd.getLegPosX(self._side))*2.0*self._side )
        cd.setHipPosY( pos[1] )
         
class FrontLegAnchorControlPoint(SymmetricCharacterControlPoint):
    def __init__(self, characterDescription, side):
        super(FrontLegAnchorControlPoint,self).__init__(characterDescription, side)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getLegPosX(self._side), cd.getHipPosY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setLegPosX( self._side, pos[0] )
    
class FrontPelvisControlPoint(CharacterControlPoint):
    def __init__(self, characterDescription):
        super(FrontPelvisControlPoint,self).__init__(characterDescription)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getPelvisDiameter()/2.0, cd.getWaistPosY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setPelvisDiameter( pos[0]*2.0 )
        cd.setWaistPosY( pos[1] )
    
class FrontChestControlPoint(CharacterControlPoint):
    def __init__(self, characterDescription):
        super(FrontChestControlPoint,self).__init__(characterDescription)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getTorsoDiameter()/2.0, cd.getChestPosY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setTorsoDiameter( pos[0]*2.0 )
        cd.setChestPosY( pos[1] )
                
class FrontShoulderControlPoint(CharacterControlPoint):
    def __init__(self, characterDescription):
        super(FrontShoulderControlPoint,self).__init__(characterDescription)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getTorsoDiameter()/2.0, cd.getShoulderPosY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setTorsoDiameter( pos[0]*2.0 )
        cd.setShoulderPosY( pos[1] )
        
class FrontNeckControlPoint(CharacterControlPoint):
    def __init__(self, characterDescription):
        super(FrontNeckControlPoint,self).__init__(characterDescription)
    def getPos(self):
        cd = self._characterDescription
        return ( 0, cd.getNeckPosY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setNeckPosY( pos[1] )
        
class FrontHeadTopControlPoint(CharacterControlPoint):
    def __init__(self, characterDescription):
        super(FrontHeadTopControlPoint,self).__init__(characterDescription)
    def getPos(self):
        cd = self._characterDescription
        return ( 0, cd.getNeckPosY() + cd.getHeadSizeY() )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setHeadSizeY( pos[1]-cd.getNeckPosY() )
        
class FrontHeadSideControlPoint(CharacterControlPoint):
    def __init__(self, characterDescription):
        super(FrontHeadSideControlPoint,self).__init__(characterDescription)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getHeadSizeX()/2.0, cd.getNeckPosY() + cd.getHeadSizeY()/2.0 )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setHeadSizeX( pos[0]*2.0 )
        
class FrontElbowControlPoint(SymmetricCharacterControlPoint):
    def __init__(self, characterDescription, side):
        super(FrontElbowControlPoint,self).__init__(characterDescription, side)
    def getPos(self):
        cd = self._characterDescription
        return ( self._side*(cd.getTorsoDiameter()/2.0 + cd.getUpperArmDiameter(self._side)), cd.getElbowPosY(self._side) )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setUpperArmDiameter( self._side, pos[0]*self._side - cd.getTorsoDiameter()/2.0 )
        cd.setElbowPosY( self._side, pos[1] )
        
class FrontWristControlPoint(SymmetricCharacterControlPoint):
    def __init__(self, characterDescription, side):
        super(FrontWristControlPoint,self).__init__(characterDescription, side)
    def getPos(self):
        cd = self._characterDescription
        return ( cd.getArmPosX(self._side) + self._side*cd.getLowerArmDiameter(self._side)/2.0, cd.getWristPosY(self._side) )
    def setPos(self, pos):
        cd = self._characterDescription
        cd.setLowerArmDiameter( self._side, (pos[0]-cd.getArmPosX(self._side))*2.0*self._side )
        cd.setWristPosY( self._side, pos[1] )    
    