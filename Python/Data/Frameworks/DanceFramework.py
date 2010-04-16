'''
Created on 2009-11-26

@author: beaudoin
'''

import PyUtils
import Core, wx
import Controllers

PyUtils.load( "RigidBodies.FiniteFlatGround" )
PyUtils.loadMany( "RigidBodies.DodgeBall", 5 )
character = PyUtils.load( "Characters.BipV3" )
character.computeMass();

controller = Controllers.DanceController(character)
controller.loadFromFile("../data/controllers/bipV3/fWalk_4.sbc")
#controller.loadFromFile("../data/controllers/bipV3/fWalk_3.sbc")

Core.cvar.SimGlobals_stanceKnee = 0.00
Core.cvar.SimGlobals_rootSagittal = 0.3
Core.cvar.SimGlobals_stepHeight = 0.25
Core.cvar.SimGlobals_duckWalk = 0.2
Core.cvar.SimGlobals_VDelSagittal = 2.0
Core.cvar.SimGlobals_stepTime = 0.2
#Core.cvar.SimGlobals_upperBodyTwist = 0.2

controller.initialSetup()



