'''
Created on 2009-09-09

@author: beaudoin
'''

from App.Proxys import *

data = SimBiController( 

    name = 'Walking',

    startingState = 0,

    controlParamsList = [ 
        ControlParams( joint = 'root', kp = 3000, kd = 300, tauMax = 10000, scale = ( 1, 0.2, 1 ) ),
        ControlParams( joint = 'pelvis_torso', kp = 200, kd = 30, tauMax = 10000, scale = ( 1, 0.2, 1 ) ),
        ControlParams( joint = 'lHip', kp = 300, kd = 30, tauMax = 10000, scale = ( 1, 0.66, 1 ) ),
        ControlParams( joint = 'rHip', kp = 300, kd = 30, tauMax = 10000, scale = ( 1, 0.66, 1 ) ),
        ControlParams( joint = 'torso_head', kp = 200, kd = 20, tauMax = 10000, scale = ( 1, 0.2, 1 ) ),
        ControlParams( joint = 'lShoulder', kp = 20, kd = 5, tauMax = 10000, scale = ( 0.5, 1, 1 ) ),
        ControlParams( joint = 'rShoulder', kp = 20, kd = 5, tauMax = 10000, scale = ( 0.3, 1, 1 ) ),
        ControlParams( joint = 'lKnee', kp = 300, kd = 30, tauMax = 10000, scale = ( 1, 0.2, 1 ) ),
        ControlParams( joint = 'rKnee', kp = 300, kd = 30, tauMax = 10000, scale = ( 1, 0.2, 1 ) ),
        ControlParams( joint = 'lElbow', kp = 5, kd = 1, tauMax = 10000, scale = ( 0.2, 1, 1 ) ),
        ControlParams( joint = 'rElbow', kp = 5, kd = 1, tauMax = 10000, scale = ( 0.2, 1, 1 ) ),
        ControlParams( joint = 'lAnkle', kp = 75, kd = 10, tauMax = 10000, scale = ( 1, 0.2, 0.2 ) ),
        ControlParams( joint = 'rAnkle', kp = 75, kd = 10, tauMax = 10000, scale = ( 1, 0.2, 0.2 ) ),
        ControlParams( joint = 'lToeJoint', kp = 10, kd = 0.5, tauMax = 10000, scale = ( 1, 1, 1 ) ),
        ControlParams( joint = 'rToeJoint', kp = 10, kd = 0.5, tauMax = 10000, scale = ( 1, 1, 1 ) ) ],

    states = [ 

        SimBiConState( 

            name = 'State 0',

            nextStateIndex = 0,

            transitionOn = 'FOOT_CONTACT',

            stance = 'REVERSE',

            duration = 0.7,

            externalForces = [
                ExternalForce(
                    body = 'pelvis',
                    forceX = [(0,0)],
                    forceY = [(0,0)],
                    forceZ = [(0,0)] )
            ],

            trajectories = [ 

                Trajectory( 
                    joint = 'root',
                    strength = [ ( 0, 1 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            baseTrajectory = [ ( 0.020067, 0.0713 ), ( 0.538462, 0.0713 ), ( 0.966555, 0.0713 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 0, 1 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.006689, 0.037422 ), ( 0.491639, -0.030618 ), ( 0.993311, 0.034506 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Hip',
                    strength = [ ( 0, 1 ) ],
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 1, 0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.010033, -0.015813 ), ( 0.354515, 0.024849 ), ( 0.508361, 0.196533 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            feedback = LinearBalanceFeedback( axis = ( 0, 0, 1 ), cd = -0.3, cv = -0.3 ),
                            baseTrajectory = [ 
                                ( 0.023411, -0.091589 ),
                                ( 0.230769, -0.476365 ),
                                ( 0.521739, -0.218055 ),
                                ( 0.70903, -0.035635 ),
                                ( 0.842809, -0.002125 ),
                                ( 0.986622, -0.000708 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 0, 1 ),
                            reverseOnStance = 'LEFT',
                            feedback = LinearBalanceFeedback( axis = ( 1, 0, 0 ), cd = 0.55, cv = 0.3 ),
                            baseTrajectory = [ ( 0.73913, -0.058739 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Knee',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            baseTrajectory = [ 
                                ( 0.016722, 0.887089 ),
                                ( 0.250836, 0.800963 ),
                                ( 0.428094, 0.659216 ),
                                ( 0.555184, 0.47829 ),
                                ( 0.632107, 0.142159 ),
                                ( 0.77592, -0.027983 ),
                                ( 0.993311, -0.03162 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Knee',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            baseTrajectory = [ ( 0.046823, 0.39623 ), ( 0.207358, 0.134641 ), ( 0.662207, -0.011162 ), ( 1, 0 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Ankle',
                    strength = [ ( 0, 1 ) ],
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            baseTrajectory = [ 
                                ( 0, 1.5804 ),
                                ( 0.22408, 0.862791 ),
                                ( 0.431438, -0.048689 ),
                                ( 0.688963, -0.71086 ),
                                ( 0.986622, -0.782532 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Ankle',
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            feedback = LinearBalanceFeedback( axis = ( 0, 0, 1 ), cd = 0.2, cv = 0.2 ),
                            baseTrajectory = [ ( 0.016722, -0.203606 ), ( 0.551839, -0.217268 ), ( 1, 0.030252 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Shoulder',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 0, 1 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0, 1.57 ), ( 1, 1.57 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            baseTrajectory = [ ( 0.036789, 0.087916 ), ( 0.665552, -0.282452 ), ( 0.989967, -0.224412 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Shoulder',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 0, 1 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0, 1.57 ), ( 1, 1.57 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 1, 0, 0 ),
                            baseTrajectory = [ 
                                ( 0.003344, 0.03002 ),
                                ( 0.180602, 0.002729 ),
                                ( 0.494983, -0.011906 ),
                                ( 0.802676, 0.068758 ),
                                ( 0.986622, 0.078197 ) ] ) ] ),

                Trajectory( 
                    joint = 'STANCE_Elbow',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 1, 0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.033445, 0.310933 ), ( 0.521739, 0.419418 ), ( 0.939799, 0.310933 ) ] ) ] ),

                Trajectory( 
                    joint = 'SWING_Elbow',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 1, 0 ),
                            reverseOnStance = 'LEFT',
                            baseTrajectory = [ ( 0.003344, -0.036889 ), ( 0.608696, -0.391027 ), ( 1, -0.3 ) ] ) ] ),

                Trajectory( 
                    joint = 'pelvis_torso',
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 1, 0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.010033, 0.00034 ), ( 0.505017, -0.100323 ), ( 0.986622, -0.001158 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 1, 0, 0 ), baseTrajectory = [ ( 0.117057, -0.001063 ), ( 0.511706, 0.024454 ) ] ),
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 0, 1 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0, 0 ), ( 0.280602, 0.015874 ), ( 0.99, 0 ) ] ) ] ),

                Trajectory( 
                    joint = 'torso_head',
                    referenceFrame = 'CHARACTER_RELATIVE',
                    components = [ 
                        TrajectoryComponent( 
                            rotationAxis = ( 0, 1, 0 ),
                            reverseOnStance = 'RIGHT',
                            baseTrajectory = [ ( 0.010033, 0 ), ( 0.508361, 0.05 ), ( 0.986622, 0 ) ] ),
                        TrajectoryComponent( rotationAxis = ( 1, 0, 0 ) ) ] )
            ]
        )
    ]
)
