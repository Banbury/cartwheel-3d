from App.UtilFuncs import fancify

print fancify( 
"""Character(
                         
    root = ArticulatedRigidBody(
        name = "pelvis",
        meshes = [ (path.join(meshDir, "pelvis_2_b.obj"), colourDark),
                   (path.join(meshDir, "pelvis_2_s.obj"), colourLight) ],
        mass = 12.9,
        moi = (0.0705, 0.11, 0.13),
        cdps = [ SphereCDP((0,-0.075,0), 0.12) ],
        pos = (0, 1.035, 0.2),
        frictionCoeff = 0.8,
        restitutionCoeff = 0.35 ),
    
    arbs = [
        ArticulatedRigidBody(
            name = "torso",
            meshes = [ (path.join(meshDir, "torso_2_b.obj"), colourDark),
                       (path.join(meshDir, "torso_2_s_v2.obj"), colourLight) ],
            mass = 22.5,
            moi = (0.34, 0.21, 0.46),
            cdps = [ SphereCDP((0,0,0.01), 0.11) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "head",
            meshes = [ (path.join(meshDir, "head_b.obj"), colourDark),
                       (path.join(meshDir, "head_s.obj"), colourLight) ],
            mass = 5.2,
            moi = (0.04, 0.02, 0.042),
            cdps = [ SphereCDP((0,0.04,0), 0.11) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),
    
        ArticulatedRigidBody(
            name = "lUpperArm",
            meshes = [ (path.join(meshDir, "lUpperArm.obj"), colourDark) ],
            mass = 2.2,
            moi = (0.005, 0.02, 0.02),
            cdps = [ CapsuleCDP((-0.15,0,0), (0.15,0,0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),
            
        ArticulatedRigidBody(
            name = "lLowerArm",
            meshes = [ (path.join(meshDir, "lLowerArm.obj"), colourDark) ],
            mass = 1.7,
            moi = (0.0024, 0.025, 0.025),
            cdps = [ CapsuleCDP((-0.15,0,0), (0.15,0,0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "rUpperArm",
            meshes = [ (path.join(meshDir, "rUpperArm.obj"), colourDark) ],
            mass = 2.2,
            moi = (0.005, 0.02, 0.02),
            cdps = [ CapsuleCDP((-0.15,0,0), (0.15,0,0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "rLowerArm",
            meshes = [ (path.join(meshDir, "rLowerArm.obj"), colourDark) ],
            mass = 1.7,
            moi = (0.0024, 0.025, 0.025),
            cdps = [ CapsuleCDP((-0.15,0,0), (0.15,0,0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),
        
        ArticulatedRigidBody(
            name = "lUpperLeg",
            meshes = [ (path.join(meshDir, "lUpperLeg.obj"), colourDark) ],
            mass = 6.6,
            moi = (0.15, 0.022, 0.15),
            cdps = [ CapsuleCDP((0, 0.12, 0), (0, -0.26, 0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "lLowerLeg",
            meshes = [ (path.join(meshDir, "lLowerLeg.obj"), colourDark) ],
            mass = 3.2,
            moi = (0.055, 0.007, 0.055),
            cdps = [ CapsuleCDP((0, 0.12, 0), (0, -0.2, 0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "rUpperLeg",
            meshes = [ (path.join(meshDir, "rUpperLeg.obj"), colourDark) ],
            mass = 6.6,
            moi = (0.15, 0.022, 0.15),
            cdps = [ CapsuleCDP((0, 0.12, 0), (0, -0.26, 0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "rLowerLeg",
            meshes = [ (path.join(meshDir, "rLowerLeg.obj"), colourDark) ],
            mass = 3.2,
            moi = (0.055, 0.007, 0.055),
            cdps = [ CapsuleCDP((0, 0.12, 0), (0, -0.2, 0), 0.05) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35 ),

        ArticulatedRigidBody(
            name = "lFoot",
            meshes = [ (path.join(meshDir, "lFoot.obj"), colourDark) ],
            mass = 1.0,
            moi = (0.007, 0.008, 0.002),
            cdps = [ BoxCDP((-0.025, -0.033, -0.09), (0.025, 0.005, 0.055)) ],
#    CDP_Sphere 0.025 -0.025 -0.08 0.01
#    CDP_Sphere -0.025 -0.025 -0.08 0.01
#    CDP_Sphere 0.02 -0.025 0.045 0.01
#    CDP_Sphere -0.02 -0.025 0.045 0.01        
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35,
            groundCoeffs = (0.0002, 0.2) ),

        ArticulatedRigidBody(
            name = "rFoot",
            meshes = [ (path.join(meshDir, "rFoot.obj"), colourDark) ],
            mass = 1.0,
            moi = (0.007, 0.008, 0.002),
            cdps = [ BoxCDP((-0.025, -0.033, -0.09), (0.025, 0.005, 0.055)) ],
#    CDP_Sphere 0.025 -0.025 -0.08 0.01
#    CDP_Sphere -0.025 -0.025 -0.08 0.01
#    CDP_Sphere 0.02 -0.025 0.045 0.01
#    CDP_Sphere -0.02 -0.025 0.045 0.01        
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35,
            groundCoeffs = (0.0002, 0.2) ),

        ArticulatedRigidBody(
            name = "lToes",
            meshes = [ (path.join(meshDir, "lToes.obj"), colourDark) ],
            mass = 0.2,
            moi = (0.002, 0.002, 0.0005),
            cdps = [ SphereCDP((0.0, -0.005, 0.025), 0.01) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35,
            groundCoeffs = (0.0002, 0.2) ),
            
        ArticulatedRigidBody(
            name = "rToes",
            meshes = [ (path.join(meshDir, "rToes.obj"), colourDark) ],
            mass = 0.2,
            moi = (0.002, 0.002, 0.0005),
            cdps = [ SphereCDP((0.0, -0.005, 0.025), 0.01) ],
            frictionCoeff = 0.8,
            restitutionCoeff = 0.35,
            groundCoeffs = (0.0002, 0.2) )
    ],
    
    joints = [
                   
        BallInSocketJoint(
            name = "pelvis_torso",
            parent = "pelvis",
            child = "torso",
            posInParent = (0, 0.17, -0.035),
            posInChild = (0, -0.23, -0.01),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 1, 0),
            limits = (-0.6, 0.6, -0.6, 0.6, -0.6, 0.6) ),
        
        BallInSocketJoint(
            name = "torso_head",
            parent = "torso",
            child = "head",
            posInParent = (0, 0.1, -0.00),
            posInChild = (0, -0.16, -0.025),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 1, 0),
            limits = (-0.6, 0.6, -0.6, 0.6, -0.6, 0.6) ),
        
        BallInSocketJoint(
            name = "lShoulder",
            parent = "torso",
            child = "lUpperArm",
            posInParent = (0.20, 0.07, 0.02),
            posInChild = (-0.17, 0, 0),
            swingAxis1 = (0, 0, 1),
            twistAxis = ( 1, 0, 0),
            limits = (-1.7, 1.7, -1.5, 1.5, -1.5, 1.5) ),
        
        BallInSocketJoint(
            name = "rShoulder",
            parent = "torso",
            child = "rUpperArm",
            posInParent = (-0.20, 0.07, 0.02),
            posInChild = (0.17, 0, 0),
            swingAxis1 = (0, 0, 1),
            twistAxis = ( 1, 0, 0),
            limits = (-1.7, 1.7, -1.5, 1.5, -1.5, 1.5) ),
        
        HingeJoint(
            name = "lElbow",
            parent = "lUpperArm",
            child = "lLowerArm",
            posInParent = (0.175, 0, 0.006),
            posInChild = (-0.215, 0, 0),
            axis = ( 0, 1, 0 ),
            limits = (-2.7, 0) ),
        
        HingeJoint(
            name = "rElbow",
            parent = "rUpperArm",
            child = "rLowerArm",
            posInParent = (-0.175, 0, 0.006),
            posInChild = (0.215, 0, 0),
            axis = ( 0, -1, 0 ),
            limits = (-2.7, 0) ),
        
        BallInSocketJoint(
            name = "lHip",
            parent = "pelvis",
            child = "lUpperLeg",
            posInParent = (0.1, -0.05, 0.0),
            posInChild = (0, 0.21, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 1, 0),
            limits = (-1.3, 1.9, -1, 1, -0.25, 1) ),
        
        BallInSocketJoint(
            name = "rHip",
            parent = "pelvis",
            child = "rUpperLeg",
            posInParent = (-0.1, -0.05, 0.0),
            posInChild = (0, 0.21, 0),
            swingAxis1 = (1, 0, 0),
            twistAxis = ( 0, 1, 0),
            limits = (-1.3, 1.9, -1, 1, -1, 0.25) ),
            
        HingeJoint(
            name = "lKnee",
            parent = "lUpperLeg",
            child = "lLowerLeg",
            posInParent = (0, -0.26, 0),
            posInChild = (0, 0.21, 0),
            axis = ( 1, 0, 0 ),
            limits = (0, 2.5) ),
            
        HingeJoint(
            name = "rKnee",
            parent = "rUpperLeg",
            child = "rLowerLeg",
            posInParent = (0, -0.26, 0),
            posInChild = (0, 0.21, 0),
            axis = ( 1, 0, 0 ),
            limits = (0, 2.5) ),
        
        UniversalJoint(
            name = "lAnkle",
            parent = "lLowerLeg",
            child = "lFoot",
            posInParent = (0, -0.25, 0.01),
            posInChild = (0.0, 0.02, -0.04),
            parentAxis = (1, 0, 0),
            childAxis = (0, 0, 1),
            limits = (-0.75, 0.75, -0.75, 0.75) ),
        
        UniversalJoint(
            name = "rAnkle",
            parent = "rLowerLeg",
            child = "rFoot",
            posInParent = (0, -0.25, 0.01),
            posInChild = (0.0, 0.02, -0.04),
            parentAxis = (1, 0, 0),
            childAxis = (0, 0, -1),
            limits = (-0.75, 0.75, -0.75, 0.75) ),
            
        HingeJoint(
            name = "lToeJoint",
            parent = "lFoot",
            child = "lToes",
            posInParent = (0, -0.02, 0.05),
            posInChild = (0, 0, -0.025),
            axis = ( 1, 0, 0 ),
            limits = (-0.52, 0.02) ),
            
        HingeJoint(
            name = "rToeJoint",
            parent = "rFoot",
            child = "rToes",
            posInParent = (0, -0.02, 0.05),
            posInChild = (0, 0, -0.025),
            axis = ( 1, 0, 0 ),
            limits = (-0.52, 0.02) )
    ]
)""")
