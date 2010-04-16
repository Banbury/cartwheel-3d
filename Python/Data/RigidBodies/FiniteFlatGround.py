'''
Created on 2009-08-28

@author: beaudoin
'''

from App.Proxys import *

data = RigidBody(
    name = "ground",
    locked = True,
    cdps = [ BoxCDP( (-50,-1,-50), (50,0,50) ),
             PlaneCDP( (0,1,0), (0,-1,0) ) ],
    frictionCoeff = 2.5,
    restitutionCoeff = 0.35 )