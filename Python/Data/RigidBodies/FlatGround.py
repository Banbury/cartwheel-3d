'''
Created on 2009-08-28

@author: beaudoin
'''

from App.Proxys import *

data = RigidBody(
    name = "ground",
    locked = True,
    cdps = [ PlaneCDP( (0,1,0), (0,0,0) ) ],
    frictionCoeff = 2.5,
    restitutionCoeff = 0.35 )
