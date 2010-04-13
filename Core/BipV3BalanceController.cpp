#include "BipV3BalanceController.h"


BipV3BalanceController::BipV3BalanceController(World* w, Character* b){
	bip = b;
	world = w;
	steppingController = new SimBiController(bip);
	standingBalanceController = new SimBiController(bip);
	steppingController->loadFromFile("../data/controllers/bipV3/iWalk.sbc");
	standingBalanceController->loadFromFile("../data/controllers/bipV3/balance.sbc");

	con = steppingController;
}

BipV3BalanceController::~BipV3BalanceController(){
	delete steppingController;
	delete standingBalanceController;
}

/**
	this method is used to set the offsets for any and all components of trajectories..
*/
void BipV3BalanceController::applyTrajectoryOffsets(){
//	return;
	//huge hack for now...

	SimBiConState* curState = standingBalanceController->states[con->FSMStateIndex];


	Trajectory* tmpTraj;

	SimGlobals::rootSagittal = SimGlobals::stanceKnee / 10.0;

	tmpTraj = curState->getTrajectory("root");
	tmpTraj->components[2]->offset = SimGlobals::rootSagittal;
	tmpTraj = curState->getTrajectory("pelvis_lowerback");
	tmpTraj->components[2]->offset = SimGlobals::rootSagittal*1.5;
	tmpTraj = curState->getTrajectory("lowerback_torso");
	tmpTraj->components[2]->offset = SimGlobals::rootSagittal*2;
	tmpTraj = curState->getTrajectory("torso_head");
	tmpTraj->components[2]->offset = SimGlobals::rootSagittal*2.1;


/*
	tmpTraj = curState->getTrajectory("root");
	tmpTraj->components[0]->offset = SimGlobals::rootSagittal;
	tmpTraj = curState->getTrajectory("pelvis_lowerback");
	tmpTraj->components[0]->offset = SimGlobals::rootSagittal*1.5;
	tmpTraj = curState->getTrajectory("lowerback_torso");
	tmpTraj->components[0]->offset = SimGlobals::rootSagittal*2;
	tmpTraj = curState->getTrajectory("torso_head");
	tmpTraj->components[0]->offset = SimGlobals::rootSagittal*3;
*/

/*
	tmpTraj = curState->getTrajectory("root");
	tmpTraj->components[1]->offset = SimGlobals::rootSagittal;
	tmpTraj = curState->getTrajectory("pelvis_lowerback");
	tmpTraj->components[1]->offset = SimGlobals::rootSagittal*1.5;
	tmpTraj = curState->getTrajectory("lowerback_torso");
	tmpTraj->components[1]->offset = SimGlobals::rootSagittal*2;
	tmpTraj = curState->getTrajectory("torso_head");
	tmpTraj->components[1]->offset = SimGlobals::rootSagittal*3;
*/

	tmpTraj = curState->getTrajectory("STANCE_Knee");
	tmpTraj->components[0]->offset = SimGlobals::stanceKnee;
	tmpTraj = curState->getTrajectory("SWING_Knee");
	tmpTraj->components[0]->offset = SimGlobals::stanceKnee;



/*	
	tmpTraj = curState->getTrajectory("SWING_Hip");
	tmpTraj->components[0]->offset = SimGlobals::swingHipSagittal;

//	tmpTraj = curState->getTrajectory("root");
//	tmpTraj->components[0]->offset = SimGlobals::swingHipSagittal;

//	tmpTraj->components[1]->offset = SimGlobals::swingHipLateral;
//	tmpTraj = curState->getTrajectory("STANCE_Hip");
//	tmpTraj->components[0]->offset = 0;
//	tmpTraj->components[1]->offset = 0;

//	tmpTraj = curState->getTrajectory("STANCE_Ankle");
//	tmpTraj->components[0]->offset = SimGlobals::stanceAngleSagittal;
//	tmpTraj->components[1]->offset = SimGlobals::stanceAngleLateral;
//	tmpTraj = curState->getTrajectory("SWING_Ankle");
//	tmpTraj->components[0]->offset = 0;
//	tmpTraj->components[1]->offset = 0;

	tmpTraj = curState->getTrajectory("STANCE_Knee");
	tmpTraj->components[0]->offset = SimGlobals::stanceKnee;
	tmpTraj = curState->getTrajectory("SWING_Knee");
	tmpTraj->components[0]->offset = SimGlobals::stanceKnee;
*/
}


bool BipV3BalanceController::runASimulationStep(){
	applyTrajectoryOffsets();

	//compute the vectors used to measure the error from the balancing goal
	Vector3d comOffsetError = con->doubleStanceCOMError; comOffsetError.y = 0;
	Vector3d comVelError = con->v; comVelError.y = 0;

	double comError = comOffsetError.length();
	double comVError = comVelError.length();

	//the character relative position of the left and right feet are here...
	Vector3d leftFootPos = con->characterFrame.inverseRotate(Vector3d(con->root->state.position,con->lFoot->state.position));
	Vector3d rightFootPos = con->characterFrame.inverseRotate(Vector3d(con->root->state.position, con->rFoot->state.position));

	//these are the total forces on the stance and swing feet
	double f1 = /*con->forceStanceHeel.y + con->forceStanceToe.y*/0.5;
	double f2 = /*con->forceSwingHeel.y + con->forceSwingToe.y*/0.5;

	double tot = f1+f2;

	if (tot < 1){
		f1 = 0;
		tot = 1;
	}

	//and the ratio of vertical forces on the two feet
	double ratio = f1 / tot;

//	tprintf("(%lf %lf %lf)\n", ratio, comError, comVelError);


	//if we're walking, see if we can transition to the balance controller
	if (con != standingBalanceController){
		if (ratio >= 0.25 && ratio <= 0.75){
//			tprintf("can stand!!! (%lf %lf)\n", comError, comVelError);
			//both feet are on the ground, so make sure that the com position and velocity are not too far off...
			//compute the character-relative coordinates of the left and right foot
			tprintf("left: %lf, right: %lf, (%lf %lf)\n", leftFootPos.x, rightFootPos.x, comError, comVelError);
			double maxComError = 0, maxComVError = 0;
			if (comOffsetError.unit().dotProductWith(comVelError.unit())<-0.5){
				maxComError = 0.05;
				maxComVError = 0.4;
			}else{
				maxComError = 0.03;
				maxComVError = 0.2;
			}
			if (comError < maxComError && comVError < maxComVError && leftFootPos.x > 0.03 && leftFootPos.x < 0.1 && -rightFootPos.x > 0.03 && -rightFootPos.x < 0.1){
				con = standingBalanceController;
				tprintf("can stand!!! (%lf %lf)\n", comError, comVelError);
			}
		}
	}else{
		//see if we need to start walking...
		if (comError > 0.05 || comVError > 0.35 /*|| (!(con->stanceHeelInContact || con->stanceToeInContact) && (con->swingHeelInContact || con->swingToeInContact))*/){
			con = steppingController;
			con->phi = 0;
			if (comVelError.x < 0)
				con->setStance(LEFT_STANCE);
			else
				con->setStance(RIGHT_STANCE);
			tprintf("can't stand!!! (%lf %lf)\n", comError, comVelError);
		}
	}



	con->computeTorques(world->getContactForces());

//	tprintf("%lf\n", con->comPosition.y);

	con->applyTorques();
	world->advanceInTime(SimGlobals::dt);
	return (con->advanceInTime(SimGlobals::dt, world->getContactForces()) != -1);
}



