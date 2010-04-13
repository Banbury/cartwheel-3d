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

            duration = 0.84,

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
                            baseTrajectory = [ ( 0.0, 0.2 ), ( 0.25, 0.2 ), ( 0.5, 0.9875 ), ( 0.75, 0.2 ), ( 1.0, 0.2 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Ankle',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 1.19 ), ( 0.25, 0.8071875 ), ( 0.5, 0.315 ), ( 0.75, -0.15326171875 ), ( 1.0, -0.56 ) ],
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
                            baseTrajectory = [ ( 0.0, -0.56 ), ( 0.25, -0.1771875 ), ( 0.5, 0.315 ), ( 0.75, 0.78326171875 ), ( 1.0, 1.19 ) ],
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
                            baseTrajectory = [ ( 0.0, 0.4 ), ( 0.25, 0.4 ), ( 0.5, 0.4 ), ( 0.75, 0.4 ), ( 1.0, 0.4 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, -1.5 ), ( 0.25, -0.856849024564 ), ( 0.5, -1.5 ), ( 0.75, -1.5 ), ( 1.0, -1.5 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, -0.25 ), ( 0.25, -0.140625 ), ( 0.5, 0.0 ), ( 0.75, 0.1337890625 ), ( 1.0, 0.25 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Shoulder',
                    strength = [  ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.4 ), ( 0.25, 0.4 ), ( 0.5, 0.4 ), ( 0.75, 0.4 ), ( 1.0, 0.4 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, 1.5 ), ( 0.25, 1.5 ), ( 0.5, 1.5 ), ( 0.75, 0.841261506347 ), ( 1.0, 1.5 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.25 ), ( 0.25, 0.140625 ), ( 0.5, 0.0 ), ( 0.75, -0.1337890625 ), ( 1.0, -0.25 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Elbow',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, 0.1 ), ( 0.25, 0.1 ), ( 0.5, 0.1 ), ( 0.75, 0.1 ), ( 1.0, 0.1 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Elbow',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, -0.1 ), ( 0.25, -0.1 ), ( 0.5, -0.1 ), ( 0.75, -0.1 ), ( 1.0, -0.1 ) ],
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
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.25, -0.338245685756 ), ( 0.5, 0.0 ), ( 0.75, 0.392858393573 ), ( 1.0, -0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.0, 0.562673316863 ),
                                ( 0.25, -0.0266145330645 ),
                                ( 0.5, -0.387208283918 ),
                                ( 0.75, 0.00346308709372 ),
                                ( 1.0, 0.562673316863 ) ],
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
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.25, -0.339825553389 ), ( 0.5, 0.0 ), ( 0.75, 0.356935318054 ), ( 1.0, -0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.0, 0.61823324355 ),
                                ( 0.25, -0.0516524962421 ),
                                ( 0.5, -0.462717152977 ),
                                ( 0.75, -0.0178491409028 ),
                                ( 1.0, 0.61823324355 ) ],
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
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, -0.0 ) ],
                            dScaledTrajectory = [  ],
                            vScaledTrajectory = [  ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, 0.0 ) ],
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

    sagittalTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, 0.0 ) ],

    coronalTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, 0.0 ) ],

    heightTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, 0.0 ) ]
)