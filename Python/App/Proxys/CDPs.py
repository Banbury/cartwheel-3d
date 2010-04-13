'''
Created on 2009-09-02

@author: beaudoin
'''

import Proxy
import Member
import Physics

cls = Physics.SphereCDP
SphereCDP = Proxy.create( cls, verbose = False, caster = Physics.castToSphereCDP,
    members = [
        Member.Point3d( 'center', (0.0,0.0,0.0), cls.getCenter, cls.setCenter ),
        Member.Basic( float, 'radius', 1.0, cls.getRadius, cls.setRadius),
    ] )

cls = Physics.BoxCDP
BoxCDP = Proxy.create( cls, verbose = False, caster = Physics.castToBoxCDP,
    members = [
        Member.Point3d( 'point1', (-1.0,-1.0,-1.0), cls.getPoint1, cls.setPoint1 ),
        Member.Point3d( 'point2', (1.0,1.0,1.0), cls.getPoint2, cls.setPoint2),
    ] )

cls = Physics.PlaneCDP
PlaneCDP = Proxy.create( cls, verbose = False, caster = Physics.castToPlaneCDP,
    members = [
        Member.Vector3d( 'normal', (0.0,1.0,0.0), cls.getNormal, cls.setNormal ),
        Member.Point3d( 'origin', (0.0,0.0,0.0), cls.getOrigin, cls.setOrigin),
    ] )

cls = Physics.CapsuleCDP
CapsuleCDP = Proxy.create( cls, verbose = False, caster = Physics.castToCapsuleCDP,
    members = [
        Member.Point3d( 'point1', (-1.0,0.0,0.0), cls.getPoint1, cls.setPoint1 ),
        Member.Point3d( 'point2', (1.0,0.0,0.0), cls.getPoint2, cls.setPoint2 ),
        Member.Basic( float, 'radius', 1.0, cls.getRadius, cls.setRadius),
    ] )

