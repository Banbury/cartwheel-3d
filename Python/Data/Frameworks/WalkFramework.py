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
character.loadReducedStateFromFile( "Data/Characters/BipV3/Controllers/WalkingState.rs" );
character.computeMass();
#controller = PyUtils.load( "Characters.BipV3.Controllers.StylizedWalking", character )
controller = PyUtils.load( "Characters.BipV3.Controllers.EditableWalking", character )
controller.setStance( Core.LEFT_STANCE );

app = wx.GetApp()
worldOracle = app.getWorldOracle()
behaviour = Core.TurnController(character, controller, worldOracle)
behaviour.initializeDefaultParameters()
controller.setBehaviour(behaviour)

behaviour.requestHeading(0);
behaviour.conTransitionPlan();
