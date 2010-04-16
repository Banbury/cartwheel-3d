from App.Proxys import *

data = IKVMCController( 

    name = '',

    controlParamsList = [ 
        ControlParams( joint = 'root', kp = 1000.0, kd = 200.0, tauMax = 200.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'pelvis_lowerback', kp = 75.0, kd = 17.0, tauMax = 100.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'lowerback_torso', kp = 75.0, kd = 17.0, tauMax = 100.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'torso_head', kp = 50.0, kd = 15.0, tauMax = 200.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'lShoulder', kp = 50.0, kd = 15.0, tauMax = 200.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'rShoulder', kp = 50.0, kd = 15.0, tauMax = 200.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'lElbow', kp = 50.0, kd = 15.0, tauMax = 200.0, scale = ( 0.2, 1.0, 1.0 ) ),
        ControlParams( joint = 'rElbow', kp = 50.0, kd = 15.0, tauMax = 200.0, scale = ( 0.2, 1.0, 1.0 ) ),
        ControlParams( joint = 'lHip', kp = 300.0, kd = 35.0, tauMax = 200.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'rHip', kp = 300.0, kd = 35.0, tauMax = 200.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'lKnee', kp = 300.0, kd = 35.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'rKnee', kp = 300.0, kd = 35.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'lAnkle', kp = 50.0, kd = 15.0, tauMax = 100.0, scale = ( 1.0, 0.2, 0.2 ) ),
        ControlParams( joint = 'rAnkle', kp = 50.0, kd = 15.0, tauMax = 100.0, scale = ( 1.0, 0.2, 0.2 ) ),
        ControlParams( joint = 'lToeJoint', kp = 2.0, kd = 0.2, tauMax = 100.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'rToeJoint', kp = 2.0, kd = 0.2, tauMax = 100.0, scale = ( 1.0, 1.0, 1.0 ) ) ],

    states = [ 

        SimBiConState( 

            name = 'State 0',

            nextStateIndex = 0,

            duration = 0.72,

            externalForces = [  ],

            trajectories = [ 

                Trajectory( 
                    joint = 'root',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 1.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 1.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( joint = 'SWING_Hip', strength = [  ], components = [  ] ),

                Trajectory( 
                    joint = 'SWING_Knee',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Knee',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.515 ), ( 0.5, 0.2 ), ( 1.0, 0.515 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Ankle',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 1.19 ), ( 1.0, -0.56 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Ankle',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, -0.56 ), ( 1.0, 1.19 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Shoulder',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.4 ), ( 0.5, 0.4 ), ( 1.0, 0.4 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, -0.221383627068 ), ( 0.5, -0.117036102433 ), ( 1.0, -0.0126885777983 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, -0.25 ), ( 0.5, 0.0 ), ( 1.0, 0.25 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Shoulder',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.4 ), ( 0.5, 0.4 ), ( 1.0, 0.4 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, 0.0126885777983 ), ( 0.5, 0.117036102433 ), ( 1.0, 0.221383627068 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.25 ), ( 0.5, 0.0 ), ( 1.0, -0.25 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Elbow',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, 0.1 ), ( 0.5, 0.1 ), ( 1.0, 0.1 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Elbow',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, -0.1 ), ( 0.5, -0.1 ), ( 1.0, -0.1 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'pelvis_lowerback',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, -0.322732761288 ), ( 0.5, -6.93889390391e-18 ), ( 1.0, 0.322732761288 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.171880490998 ), ( 0.5, 0.0 ), ( 1.0, 0.171880490998 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'lowerback_torso',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, -0.107003153544 ), ( 0.5, 0.0 ), ( 1.0, 0.107003153544 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.164284842328 ), ( 0.5, 0.0 ), ( 1.0, 0.164284842328 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'torso_head',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.5, 0.0 ), ( 1.0, -0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.5, 0.0 ), ( 1.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'SWING_ToeJoint',
                    strength = [ ( 0.3, 0.1 ), ( 0.5, 0.1 ), ( 0.6, 1.0 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_ToeJoint',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] )
            ]
        )
    ],

    sagittalTrajectory = [ ( 0.0, 0.0 ), ( 0.5, 0.0 ), ( 1.0, 0.0 ) ],

    coronalTrajectory = [ ( 0.0, 0.0 ), ( 0.5, -0.0625 ), ( 1.0, 0.0 ) ],

    heightTrajectory = [ ( 0.0, 0.0 ), ( 0.5, 0.04375 ), ( 1.0, 0.0 ) ]
)