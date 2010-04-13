'''
Created on 2009-08-28

@author: beaudoin
'''

import PyUtils
import Core, wx

File = "Jumping"
#File = "Running"
#File = "SideRunning"
#File = "JumpWalk"
#File = "Walking"
#File = "Turning"

#PyUtils.load( "RigidBodies.FlatGround" )
PyUtils.load( "RigidBodies.FiniteFlatGround" )
PyUtils.loadMany( "RigidBodies.DodgeBall", 5 )
character = PyUtils.load( "Characters.Bip" )
character.loadReducedStateFromFile( "Data/Characters/Bip/Controllers/%sState.rs" % File );
controller = PyUtils.load( "Characters.Bip.Controllers.%s" % File, character )
controller.setStance( Core.LEFT_STANCE );
