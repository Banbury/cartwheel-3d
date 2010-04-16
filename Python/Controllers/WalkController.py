'''
Created on 2009-11-27

@author: beaudoin
'''

import Core
import random, math
from MathLib import Point3d, Vector3d

class WalkController(Core.IKVMCController):
    
    def __init__(self, character):
        super(WalkController,self).__init__(character)
        
    def typeName(self):
        return self.__class__.__name__
        
    def initialSetup(self):
        """Call this method for the first setup of the dance controller"""
        self.desiredCoronalStepLength = 0.04
        
        self.defaultStepSize = 0.275

        self.legLength = 1

        self.setupParameters()
                
               
    def setupParameters(self):
        """Called whenever parameters are setup"""
        
        self.getState(0).setDuration( Core.cvar.SimGlobals_stepTime )
        #now prepare the step information for the following step:
        footStart = self.getStanceFootPos().z 
        sagittalPlaneFutureFootPos = footStart + self.defaultStepSize
        self.swingFootTrajectory.clear()
        self.setSagittalBalanceFootPlacement( 1 )
        self.swingFootTrajectory.addKnot(0, Point3d(0, 0.04, self.getSwingFootPos().z - footStart));
        self.swingFootTrajectory.addKnot(0.5, Point3d(0, 0.05 + 0.1 + Core.cvar.SimGlobals_stepHeight, 0.5 * self.getSwingFootPos().z + sagittalPlaneFutureFootPos * 0.5 - footStart));
        self.swingFootTrajectory.addKnot(1, Point3d(0, 0.05 + 0, sagittalPlaneFutureFootPos - footStart));
        
        
    def performPreTasks(self, contactForces):
        """Performs all the tasks that need to be done before the physics simulation step."""          
        v = self.getV()
        d = self.getD()
        
        self.velDSagittal = Core.cvar.SimGlobals_VDelSagittal
        curState = self.getCurrentState()

        fLean =  Core.cvar.SimGlobals_rootSagittal

        traj = curState.getTrajectory("root")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean;
        traj = curState.getTrajectory("pelvis_lowerback")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean*1.5;
        traj = curState.getTrajectory("lowerback_torso")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean*2.5;
        traj = curState.getTrajectory("torso_head")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean*3.0;
        traj = curState.getTrajectory("STANCE_Knee")
        component = traj.getTrajectoryComponent(0)
        component.offset = Core.cvar.SimGlobals_stanceKnee
                
        self.legOrientation = Core.cvar.SimGlobals_duckWalk
        
        super(WalkController,self).performPreTasks(contactForces)
               
    def performPostTasks(self, dt, contactForces):
        """Performs all the tasks that need to be done after the physics simulation step."""          

        step = Vector3d(self.getStanceFootPos(), self.getSwingFootPos())
        step = self.getCharacterFrame().inverseRotate(step)                    
        
        phi = self.getPhase()
        if step.z > 0.7 :
            self.setPhase( 1.0 )
        
        if super(WalkController,self).performPostTasks(dt, contactForces):
            v = self.getV()
            print "step: %3.5f %3.5f %3.5f. Vel: %3.5f %3.5f %3.5f. phi = %f\n" % (step.x, step.y, step.z, v.x, v.y, v.z, phi);
            self.setupParameters()
        
        
      