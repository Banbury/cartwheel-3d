'''
Created on 2009-09-03

@author: beaudoin
'''

import Proxy
import Member

import Physics, Core, wx

cls = Physics.ArticulatedFigure
ArticulatedFigure = Proxy.create( cls, loader = Physics.world().addArticulatedFigure,
    members = [
        Member.Basic( str, 'name', 'UnnamedArticulatedFigure', cls.getName, cls.setName ),
        Member.Object( 'root', None, cls.getRoot, cls.setRoot ),
        Member.ObjectList( 'arbs', None, cls.getArticulatedRigidBody, cls.getArticulatedRigidBodyCount, cls.addArticulatedRigidBody ),
        Member.ObjectList( 'joints', None, cls.getJoint, cls.getJointCount, cls.addJoint )
    ] )

cls = Core.Character
Character = Proxy.create( cls, parent = ArticulatedFigure, loader = wx.GetApp().addCharacter )
