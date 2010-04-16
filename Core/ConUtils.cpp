#include "ConUtils.h"
#include <string.h>

typedef struct con_key_word{
	char keyWord[40];
	int retVal;
}ConKeyWord;

static ConKeyWord keywords[] = {
	{"#", CON_COMMENT},
	{"PDParams", CON_PD_GAINS_START},
	{"/PDParams", CON_PD_GAINS_END},
	{"ConState", CON_STATE_START},
	{"/ConState", CON_STATE_END},
	{"nextState", CON_NEXT_STATE},
	{"description", CON_STATE_DESCRIPTION},
	{"transitionOn", CON_TRANSITION_ON},
	{"stateStance", CON_STATE_STANCE},
	{"startingStance", CON_STARTING_STANCE},
	{"startAtState", CON_START_AT_STATE},
	{"loadCharacterState", CON_CHARACTER_STATE},
	{"time", CON_STATE_TIME},
	{"trajectory", CON_TRAJECTORY_START},
	{"/trajectory", CON_TRAJECTORY_END},
	{"baseTrajectory", CON_BASE_TRAJECTORY_START},
	{"/baseTrajectory", CON_BASE_TRAJECTORY_END},
	{"rotationAxis", CON_ROTATION_AXIS},
	{"reverseTargetAngleOnStance",CON_REVERSE_ANGLE_ON_STANCE},
	{"feedbackProjectionAxis",CON_FEEDBACK_PROJECTION_AXIS},
	{"feedback", CON_FEEDBACK_START},
	{"/feedback", CON_FEEDBACK_END},
	{"cdNeg", CON_CD_NEG},
	{"cdPos", CON_CD_POS},
	{"cd", CON_CD},
	{"cvNegNeg", CON_CV_NEG_NEG},
	{"cvNegPos", CON_CV_NEG_POS},
	{"cvPosNeg", CON_CV_POS_NEG},
	{"cvPosPos", CON_CV_POS_POS},
	{"cv", CON_CV},
	{"loadRBFile", LOAD_RB_FILE},
	{"loadController", LOAD_CON_FILE},
	{"component", CON_TRAJ_COMPONENT},
	{"/component", CON_TRAJ_COMPONENT_END},
	{"synchronizeControllers", CON_SYNCHRONIZE_CONTROLLERS},
	{"loadCompositeController", CON_LOAD_COMPOSITE_CONTROLLER},
	{"strengthTrajectory", CON_STRENGTH_TRAJECTORY_START},
	{"/strengthTrajectory", CON_STRENGTH_TRAJECTORY_END},
	{"characterFrameRelative", CON_CHAR_FRAME_RELATIVE},
	{"stanceHipDamping", CON_STANCE_HIP_DAMPING},
	{"stanceHipMaxVelocity", CON_STANCE_HIP_MAX_VELOCITY},
	{"dMin", CON_D_MIN},
	{"dMax", CON_D_MAX},
	{"vMin", CON_V_MIN},
	{"vMax", CON_V_MAX},
	{"dTrajX", CON_D_TRAJX_START},
	{"/dTrajX", CON_D_TRAJX_END},
	{"dTrajZ", CON_D_TRAJZ_START},
	{"/dTrajZ", CON_D_TRAJZ_END},
	{"vTrajX", CON_V_TRAJX_START},
	{"/vTrajX", CON_V_TRAJX_END},
	{"vTrajZ", CON_V_TRAJZ_START},
	{"/vTrajZ", CON_V_TRAJZ_END},
	{"rootPredictiveTorqueScale", CON_ROOT_PRED_TORQUE_SCALE},
	{"minFeedback", CON_MIN_FEEDBACK},
	{"maxFeedback", CON_MAX_FEEDBACK},
	{"dScaleTraj", CON_D_SCALE_TRAJECTORY_START},
	{"/dScaleTraj", CON_D_SCALE_TRAJECTORY_END},
	{"vScaleTraj", CON_V_SCALE_TRAJECTORY_START},
	{"/vScaleTraj", CON_V_SCALE_TRAJECTORY_END}
};

/**
	This method is used to determine the type of a line that was used in the input file for a rigid body.
	It is assumed that there are no white spaces at the beginning of the string that is passed in. the pointer buffer
	will be updated to point at the first character after the keyword.
*/
int getConLineType(char* &buffer){


	//declare a list of keywords
	int keyWordCount = sizeof(keywords)/sizeof(keywords[0]);

	if (buffer[0] == '\0')
		return CON_COMMENT;

	for (int i=0;i<keyWordCount;i++){
		if (strncmp(buffer, keywords[i].keyWord, strlen(keywords[i].keyWord)) == 0){
			buffer += strlen(keywords[i].keyWord);
			return keywords[i].retVal;
		}
	}

	return CON_NOT_IMPORTANT;
}

/**
	This method is used to determine the string corresponding to a specific line keyword
*/
const char* getConLineString(int lineType){

	//declare a list of keywords
	int keyWordCount = sizeof(keywords)/sizeof(keywords[0]);

	for (int i=0;i<keyWordCount;i++){
		if ( keywords[i].retVal == lineType ){
			return keywords[i].keyWord;
		}
	}

	return "ERROR! Unknown lineType";
}
