'''
Created on 2009-11-27

@author: beaudoin
'''

import Core
import random, math
from MathLib import Point3d, Vector3d

class DanceController(Core.IKVMCController):
    
    def __init__(self, character):
        super(DanceController,self).__init__(character)
        
    def typeName(self):
        return self.__class__.__name__
        
    def initialSetup(self):
        """Call this method for the first setup of the dance controller"""
        self.desiredCoronalStepLength = -0.02
        
        self.silly = 0;
        self.cnt = 0.0001;
        self.sillySign = 1;
        
        self.silly2 = 0;
        self.cnt2 = 0.0003;
        self.sillySign2 = 1;

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
#        self.swingFootTrajectory.addKnot(0, Point3d(0, 0.04, self.getSwingFootPos().z - footStart));
#        self.swingFootTrajectory.addKnot(0.5, Point3d(0, 0.05 + 0.1 + Core.cvar.SimGlobals_stepHeight, 0.5 * self.getSwingFootPos().z + sagittalPlaneFutureFootPos * 0.5 - footStart));
#        self.swingFootTrajectory.addKnot(1, Point3d(0, 0.05 + 0, sagittalPlaneFutureFootPos - footStart));
        self.swingFootTrajectory.addKnot(0, Point3d(0, 0.06 + 0.1, 0))
        self.swingFootTrajectory.addKnot(0.75, Point3d(0, 0.08 + 0.1 + Core.cvar.SimGlobals_stepHeight, 0.15))
        self.swingFootTrajectory.addKnot(1.0, Point3d(0, 0.08 + Core.cvar.SimGlobals_stepHeight/2, 0.15))
        # controller->swingFootTrajectory.addKnot(1, Point3d(0, 0.05, 0.27));
        
        takeAStep = False
        idleMotion = False
        if self.doubleStanceMode and idleMotion:
            if random.random() < 0.2:
                Core.cvar.SimGlobals_upperBodyTwist = (random.random() - 0.5)
            elif random.random() < 0.2:
                self.doubleStanceMode = False
                Core.cvar.SimGlobals_desiredHeading += Core.cvar.SimGlobals_upperBodyTwist + (random.random() - 0.5)
                Core.cvar.SimGlobals_upperBodyTwist = 0;
                takeAStep = True;
        
        v = self.getV()
        print "v.x: %f, v.z: %f" % (v.x, v.z)
        if math.fabs(v.x) < 0.1 and \
          math.fabs(v.z) < 0.05 and \
          math.fabs(Core.cvar.SimGlobals_VDelSagittal) <= 0.1 and \
          shouldComeToStop :
            if not self.doubleStanceMode :
                # check out the distance between the feet...
                fMidPoint = Vector3d(self.stanceFoot.getCMPosition(), self.swingFoot.getCMPosition())
                errV = self.characterFrame.inverseRotate(self.doubleStanceCOMError)            
                if errV.length() < 0.05 and fMidPoint.length() < 0.2 and fMidPoint.length() > 0.05 :
                    self.doubleStanceMode = True;
        
    def checkStanceState(self):
        """Checks the stance state"""
        if self.getDoubleStanceCOMError().length() > 0.06 :
            if self.doubleStanceMode :
                print "Should take a step...\n"
            self.doubleStanceMode = False
        
    def performPreTasks(self, contactForces):
        """Performs all the tasks that need to be done before the physics simulation step."""          
        v = self.getV()
        d = self.getD()
        
        self.checkStanceState()
        self.velDSagittal = Core.cvar.SimGlobals_VDelSagittal
        curState = self.getCurrentState()

#        fLean = 0
#        errM = v.z - self.velDSagittal
#        sign = math.copysign( 1, errM )
#        errM = math.fabs(errM)
#        if errM > 0.3 :
#            fLean += -(errM*sign - 0.3*sign) / 5.0
#        if fLean > 0.15 :
#            fLean = 0.15
#        if fLean < -0.15 :
#            fLean = -0.15
            
        fLean =  Core.cvar.SimGlobals_rootSagittal
                
        sign = 1
        if self.getStance() == Core.LEFT_STANCE :
            sign = -1

        self.silly += self.sillySign * self.cnt
        
        if self.silly > 0.15 : self.sillySign = -1
        if self.silly < -0.15 : self.sillySign = 1
        
        traj = curState.getTrajectory("root")
        component = traj.getTrajectoryComponent(0)
        component.offset = Core.cvar.SimGlobals_upperBodyTwist/2.0 * sign

        traj = curState.getTrajectory("pelvis_lowerback")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean*1.5
        component = traj.getTrajectoryComponent(1)
        component.offset = self.silly *1.5* sign
        component = traj.getTrajectoryComponent(0)
        component.offset =  Core.cvar.SimGlobals_upperBodyTwist * sign

        traj = curState.getTrajectory("lowerback_torso")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean*2.5
        component = traj.getTrajectoryComponent(1)
        component.offset = self.silly *2.5* sign
        component = traj.getTrajectoryComponent(0)
        component.offset =  Core.cvar.SimGlobals_upperBodyTwist*2.0 * sign
               
        traj = curState.getTrajectory("torso_head")
        component = traj.getTrajectoryComponent(2)
        component.offset = fLean*3.0
        component = traj.getTrajectoryComponent(1)
        component.offset = self.silly * 1 * sign
        component = traj.getTrajectoryComponent(0)
        component.offset =  Core.cvar.SimGlobals_upperBodyTwist*3.0 * sign


        self.silly2 += self.sillySign2 * self.cnt2

        if self.silly2 > 0.15 : self.sillySign2 = -1
        if self.silly2 < -0.2 : self.sillySign2 = 1

        traj = curState.getTrajectory("lShoulder")
        component = traj.getTrajectoryComponent(2)
        component.offset = self.silly2*10.0
        component = traj.getTrajectoryComponent(0)
        component.offset = self.silly2*-2.0
        component = traj.getTrajectoryComponent(1)
        component.offset = self.silly2*self.silly2*25

        traj = curState.getTrajectory("rShoulder")
        component = traj.getTrajectoryComponent(2)
        component.offset = self.silly2*10.0
        component = traj.getTrajectoryComponent(0)
        component.offset = self.silly2*-2.0
        component = traj.getTrajectoryComponent(1)
        component.offset = self.silly2*self.silly2*-25

        self.velDLateral = 0.0
        self.coronalPlaneOffset = 0

#        tmp = (0.8 - self.getPhase()) / 0.6
#        if tmp < 0 : tmp = 0
#        if tmp > 1 : tmp = 1
#        if math.fabs(v.x) > 0.1 or math.fabs(d.x) > 0.05 :
#            tmp = 0
#        tmp = 0
#        controller.coronalBalanceFootPlacement = 1-tmp
        self.setCoronalBalanceFootPlacement( 1 )

        if self.doubleStanceMode :
            self.setPhase( 0.3 )


        traj = curState.getTrajectory("STANCE_Knee")
        component = traj.getTrajectoryComponent(0)
        component.offset = Core.cvar.SimGlobals_stanceKnee

        traj = curState.getTrajectory("SWING_Knee")
        component = traj.getTrajectoryComponent(0)
        component.offset = Core.cvar.SimGlobals_stanceKnee

        self.legOrientation = Core.cvar.SimGlobals_duckWalk
        
        super(DanceController,self).performPreTasks(contactForces)
               
    def performPostTasks(self, dt, contactForces):
        """Performs all the tasks that need to be done after the physics simulation step."""          

        step = Vector3d(self.getStanceFootPos(), self.getSwingFootPos())
        step = self.getCharacterFrame().inverseRotate(step)                    
        
        phi = self.getPhase()
        if step.z > 0.7 or (math.fabs(step.x) > 0.45 and phi > 0.5) :
            self.setPhase( 1.0 )
        
        if super(DanceController,self).performPostTasks(dt, contactForces):
            v = self.getV()
            print "step: %3.5f %3.5f %3.5f. Vel: %3.5f %3.5f %3.5f. phi = %f\n" % (step.x, step.y, step.z, v.x, v.y, v.z, phi);
            self.setupParameters()
        
        
      