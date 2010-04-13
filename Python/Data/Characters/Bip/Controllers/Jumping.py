from App.Proxys import *

data = SimBiController( 

    name = 'Jumping',

    controlParamsList = [ 
        ControlParams( joint = 'root', kp = 3000.0, kd = 300.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'pelvis_torso', kp = 1000.0, kd = 100.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'torso_head', kp = 200.0, kd = 20.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'lShoulder', kp = 100.0, kd = 10.0, tauMax = 10000.0, scale = ( 0.5, 1.0, 1.0 ) ),
        ControlParams( joint = 'rShoulder', kp = 100.0, kd = 10.0, tauMax = 10000.0, scale = ( 0.3, 1.0, 1.0 ) ),
        ControlParams( joint = 'lElbow', kp = 5.0, kd = 1.0, tauMax = 10000.0, scale = ( 0.2, 1.0, 1.0 ) ),
        ControlParams( joint = 'rElbow', kp = 5.0, kd = 1.0, tauMax = 10000.0, scale = ( 0.2, 1.0, 1.0 ) ),
        ControlParams( joint = 'lHip', kp = 300.0, kd = 30.0, tauMax = 10000.0, scale = ( 1.0, 0.66, 1.0 ) ),
        ControlParams( joint = 'rHip', kp = 300.0, kd = 30.0, tauMax = 10000.0, scale = ( 1.0, 0.66, 1.0 ) ),
        ControlParams( joint = 'lKnee', kp = 300.0, kd = 30.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'rKnee', kp = 300.0, kd = 30.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'lAnkle', kp = 100.0, kd = 10.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'rAnkle', kp = 100.0, kd = 10.0, tauMax = 10000.0, scale = ( 1.0, 0.2, 1.0 ) ),
        ControlParams( joint = 'lToeJoint', kp = 50.0, kd = 5.0, tauMax = 10000.0, scale = ( 1.0, 1.0, 1.0 ) ),
        ControlParams( joint = 'rToeJoint', kp = 50.0, kd = 5.0, tauMax = 10000.0, scale = ( 1.0, 1.0, 1.0 ) ) ],

    states = [ 

        SimBiConState( 

            name = 'State0',

            nextStateIndex = 1,

            transitionOn = 'TIME_UP',

            duration = 2.0,

            externalForces = [ 
                ExternalForce( 
                    body = 'pelvis',
                    forceX = [ ( 0.0, 0.0 ) ],
                    forceY = [ 
                        ( 0.220257139643, -0.297893823029 ),
                        ( 0.269866998824, 4009.37843155 ),
                        ( 0.323865606442, -0.297893823029 ) ],
                    forceZ = [ 
                        ( 0.22457639404, 0.230547728033 ),
                        ( 0.312787202786, 1969.12899193 ),
                        ( 0.340594719742, 0.281229318864 ) ],
                    torqueX = [ 
                        ( 0.336683417085, 0.026145829397 ),
                        ( 0.388500473607, -54.4526060901 ),
                        ( 0.448698407471, -3.77495360492 ),
                        ( 0.677040782713, -1.51348004883 ),
                        ( 0.878199541855, -0.759655530132 ),
                        ( 1.03586451524, -30.912636278 ) ],
                    torqueY = [ 
                        ( 0.361809045226, -0.0251256281407 ),
                        ( 0.429548422996, -8.20440959163 ),
                        ( 0.474930850007, -0.0223053848099 ) ],
                    torqueZ = [ 
                        ( 0.336683417085, 0.0753768844221 ),
                        ( 0.416682339801, -13.6502244593 ),
                        ( 0.462311557789, 0.0251256281407 ) ] ) ],

            trajectories = [ 

                Trajectory( 
                    joint = 'root',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( rotationAxis = ( 1.0, 0.0, 0.0 ), baseTrajectory = [ ( 0.488294, 0.113631 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, 0.0 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Hip',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.197367186158, -0.311997836151 ),
                                ( 0.29648241206, -2.0351758794 ),
                                ( 0.557788944724, -1.03015075377 ),
                                ( 0.663316582915, -0.929648241206 ),
                                ( 1.06507609055, 0.082112161635 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.0, -0.06 ), ( 0.5, -0.06 ), ( 1.0, -0.06 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Hip',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.195969899497, -1.08040201005 ),
                                ( 0.195979899497, 0.0251256281407 ),
                                ( 0.43216080402, 0.0753768844221 ),
                                ( 0.984924623116, -1.08040201005 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 0.0, 0.0, 1.0 ), reverseOnStance = 'LEFT', baseTrajectory = [ ( 0.0, 0.0 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Knee',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.336683417085, 1.4824120603 ), ( 0.552763819095, 0.175879396985 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Knee',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.16105827842, 0.873209081588 ),
                                ( 0.224457449363, 0.0138604616125 ),
                                ( 0.43216080402, 0.0753768844221 ),
                                ( 0.723618090452, 1.93467336683 ),
                                ( 0.989949748744, 0.879396984925 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Ankle',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.020067, 0.809573 ),
                                ( 0.197324, -0.510418 ),
                                ( 0.27135678392, -0.326633165829 ),
                                ( 0.658291457286, 0.376884422111 ),
                                ( 0.994974874372, 0.527638190955 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Ankle',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.211055276382, -0.326633165829 ), ( 0.261306532663, 0.577889447236 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Shoulder',
                    strength = [ ( 0.0, 1.0 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.327759, 1.733668 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0301, -0.005792 ), ( 0.41806, -0.277104 ), ( 0.973244, -0.017375 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.006689, -0.025126 ), ( 0.505017, -0.138526 ), ( 0.996656, -0.025126 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Shoulder',
                    strength = [ ( 0.0, 1.0 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.110368, 1.884422 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.023411, 0.003989 ), ( 0.471572, 0.243329 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.013378, 0.005066 ), ( 0.448161, 0.321392 ), ( 0.993311, 0.025126 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Elbow',
                    strength = [ ( 0.307692, 2.135678 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.307692, 2.573897 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Elbow',
                    strength = [ ( 0.364548, 2.98995 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.013378, -2.665055 ) ] ) ] ),

                Trajectory( 
                    joint = 'pelvis_torso',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.010033, 0.076817 ), ( 0.150502, -0.29923 ), ( 0.993311, -0.248525 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 1.0, 0.0, 0.0 ), baseTrajectory = [ ( 0.598662, 0.138959 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.685619, 0.026046 ) ] ) ] ),

                Trajectory( 
                    joint = 'torso_head',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.010033, 0.076817 ), ( 0.150502, -0.29923 ), ( 0.993311, -0.248525 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 1.0, 0.0, 0.0 ), baseTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_ToeJoint',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.692308, 0.025126 ), ( 0.856187, -1.532663 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_ToeJoint',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.38796, 0.014868 ), ( 0.826087, -0.431185 ) ] ) ] )
            ]
        ),

        SimBiConState( 

            name = 'State1',

            nextStateIndex = 1,

            transitionOn = 'TIME_UP',

            duration = 0.4,

            externalForces = [ 
                ExternalForce( 
                    body = 'pelvis',
                    forceX = [ ( 0.0, 0.0 ) ],
                    forceY = [ ( 0.0, 0.0 ) ],
                    forceZ = [ ( 0.0, 0.0 ) ],
                    torqueX = [  ],
                    torqueY = [  ],
                    torqueZ = [  ] ) ],

            trajectories = [ 

                Trajectory( 
                    joint = 'root',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( rotationAxis = ( 1.0, 0.0, 0.0 ), baseTrajectory = [ ( 0.488294, 0.113631 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0, 0.0 ), ( 0.25, 0.0 ), ( 0.5, 0.0 ), ( 0.75, 0.0 ), ( 1.0, 0.0 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Hip',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            feedback = LinearBalanceFeedback( axis = ( 0.0, 0.0, 1.0 ), cd = -0.55, cv = -0.3 ),
                            baseTrajectory = [ ( 0.541806, -0.438308 ), ( 0.692308, -0.362199 ), ( 0.859532, -0.160317 ), ( 0.996656, 0.200194 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            feedback = LinearBalanceFeedback( axis = ( 1.0, 0.0, 0.0 ), cd = 0.55, cv = 0.3 ),
                            baseTrajectory = [ ( 0.0, -0.06 ), ( 0.5, -0.06 ), ( 1.0, -0.06 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Knee',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.528428, 1.658482 ), ( 0.80602, 1.006429 ), ( 0.983278, 0.354748 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Knee',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.147157, 0.130628 ),
                                ( 0.394649, 0.318731 ),
                                ( 0.61204, 0.29114 ),
                                ( 0.832776, 0.236208 ),
                                ( 0.989967, 0.576787 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Ankle',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ 
                                ( 0.020067, 0.809573 ),
                                ( 0.197324, -0.510418 ),
                                ( 0.488294, -0.518456 ),
                                ( 0.75, -0.5 ),
                                ( 0.751, -0.15 ),
                                ( 1.0, -0.15 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Ankle',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.354515, -0.354772 ), ( 0.625418, 0.764028 ), ( 0.749164, 1.163781 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Shoulder',
                    strength = [ ( 0.0, 1.0 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.327759, 1.733668 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.0301, -0.005792 ), ( 0.41806, -0.277104 ), ( 0.973244, -0.017375 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.006689, -0.025126 ), ( 0.505017, -0.138526 ), ( 0.996656, -0.025126 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Shoulder',
                    strength = [ ( 0.0, 1.0 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.110368, 1.884422 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.023411, 0.003989 ), ( 0.471572, 0.243329 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.013378, 0.005066 ), ( 0.448161, 0.321392 ), ( 0.993311, 0.025126 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Elbow',
                    strength = [ ( 0.307692, 2.135678 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.307692, 2.573897 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Elbow',
                    strength = [ ( 0.364548, 2.98995 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.013378, -2.665055 ) ] ) ] ),

                Trajectory( 
                    joint = 'pelvis_torso',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.010033, 0.076817 ), ( 0.150502, -0.29923 ), ( 0.993311, -0.248525 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 1.0, 0.0, 0.0 ), baseTrajectory = [ ( 0.598662, 0.138959 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 0.0, 1.0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.685619, 0.026046 ) ] ) ] ),

                Trajectory( 
                    joint = 'torso_head',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0.0, 1.0, 0.0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.010033, 0.076817 ), ( 0.150502, -0.29923 ), ( 0.993311, -0.248525 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 1.0, 0.0, 0.0 ), baseTrajectory = [  ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_ToeJoint',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.692308, 0.025126 ), ( 0.856187, -1.532663 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_ToeJoint',
                    strength = [  ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1.0, 0.0, 0.0 ),
                            baseTrajectory = [ ( 0.38796, 0.014868 ), ( 0.826087, -0.431185 ) ] ) ] )
            ]
        )
    ]
)