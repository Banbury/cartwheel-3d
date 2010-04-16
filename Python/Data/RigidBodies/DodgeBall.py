'''
Created on 2009-08-28

@author: beaudoin
'''

from App.Proxys import *

data = RigidBody(
    name = "dodgeBall",
    meshes = [ ("../data/models/sphere.obj",(0.8,0,0,1)) ],
    mass = 2.0,
    moi = ( 0.2,0.2,0.2 ),
    cdps = [ SphereCDP( (0,0,0), 0.1 ) ],
    pos = (1000, 1000, 0.2),
    frictionCoeff = 1.8,
    restitutionCoeff = 0.35 )