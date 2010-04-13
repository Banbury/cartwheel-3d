'''
Created on 2009-08-25

Basic controller editor

@author: beaudoin
'''

import wx, App, math

movieResolution = (1280,720)
movieSetup = False # True if we want a movie
glMovie = False    # True if we're only interested in recording the GL canvas
                   # False if we want a "screen cast"

glCanvasSize = wx.DefaultSize
size = wx.DefaultSize
showConsole = True
if movieSetup:
    if glMovie:
        glCanvasSize = movieResolution
    else:
        size = movieResolution
        showConsole = False


app = App.SNMApp("Style Editor", size = size, glCanvasSize=glCanvasSize, showConsole = showConsole)

import UI, Utils, GLUtils, Physics, Core, MathLib, PyUtils
from App import InstantChar, KeyframeEditor

# Create the desired tool sets
toolPanel = app.getToolPanel()
animationToolSet = UI.ToolSets.Animation(toolPanel)
if not movieSetup:
    optionsToolSet = UI.ToolSets.Options(toolPanel)
cameraToolSet = UI.ToolSets.Camera(toolPanel)
instantChar = InstantChar.Model()
if not movieSetup:
    controllerSnapshotToolSet = UI.ToolSets.SnapshotTree(toolPanel)
    controllerTreeToolSet = UI.ToolSets.ControllerTree(toolPanel)

glCanvas = app.getGLCanvas()
glCanvas.addGLUITool( UI.GLUITools.CurveEditorCollection )

#from Data.Frameworks import DefaultFramework
#from Data.Frameworks import DanceFramework
#from Data.Frameworks import WalkFramework


#####################
## BEGIN Custom for Instant Character

character = instantChar.create()
#controller = PyUtils.load( "Characters.BipV3.Controllers.CartoonySneak", character )
#controller = PyUtils.load( "Characters.BipV3.Controllers.ZeroWalk", character )
#controller = PyUtils.load( "Characters.BipV3.Controllers.VeryFastWalk_5-43_0-4", character )
#controller = PyUtils.load( "Characters.BipV3.Controllers.jog_8-76_0-33", character )
#controller = PyUtils.load( "Characters.BipV3.Controllers.run_21-57_0-38_0-10", character )
#controller = PyUtils.load( "Characters.BipV3.Controllers.FastWalk_3-7_0-53", character )
controller = PyUtils.load( "Characters.BipV3.Controllers.EditableWalking", character )
#controller = PyUtils.load( "Temp.Cartoony", character )
#controller = PyUtils.load( "Temp.TipToe", character )
controller.setStance( Core.LEFT_STANCE );
instantChar.connectController(False)

#character.loadReducedStateFromFile( "Data/Characters/BipV3/Controllers/runState.rs" )

keyframeEditor = KeyframeEditor.Model()

## END
#####################

######################
## BEGIN DUMMY STUFF

#staircase = App.Scenario.Staircase( position=(0,0,5), rightRampHeight = 1, stepCount = 22, leftRampHeight = 1, angle = math.pi/4.0 )
#staircase.load()


#PyUtils.convertController("../../scoros5/data/controllers/bipV3/fWalk_3.sbc")

#ctrl2 = PyUtils.copy( app.getController(0), char )

## END DUMMY STUFF
######################

# Initial snapshot
app.takeSnapshot()

app.MainLoop()
app.Destroy()
