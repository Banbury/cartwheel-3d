#pragma once

#define CON_NOT_IMPORTANT				1
#define CON_COMMENT						2
#define CON_PD_GAINS_START				3
#define CON_PD_GAINS_END				4
#define CON_STATE_START					5
#define CON_STATE_END					6
#define CON_NEXT_STATE					7
#define CON_STATE_DESCRIPTION			8
#define CON_TRANSITION_ON				9
#define CON_STATE_STANCE				10
#define CON_STARTING_STANCE				11
#define CON_START_AT_STATE				12
#define CON_CHARACTER_STATE				13
#define CON_STATE_TIME					14
#define CON_TRAJECTORY_START			15
#define CON_TRAJECTORY_END				16
#define CON_REVERSE_ANGLE_ON_STANCE		17
#define CON_ROTATION_AXIS				18
#define CON_BASE_TRAJECTORY_START		19
#define CON_BASE_TRAJECTORY_END			20
#define CON_FEEDBACK_START				21
#define CON_FEEDBACK_END				22
#define CON_CD							23
#define CON_CV							24
#define CON_FEEDBACK_PROJECTION_AXIS    25
#define LOAD_RB_FILE					26
#define LOAD_CON_FILE					27
#define CON_TRAJ_COMPONENT				28
#define CON_TRAJ_COMPONENT_END			29

#define CON_SYNCHRONIZE_CONTROLLERS		30
#define CON_LOAD_COMPOSITE_CONTROLLER	31

#define CON_STRENGTH_TRAJECTORY_START	32
#define CON_STRENGTH_TRAJECTORY_END		33
#define CON_CHAR_FRAME_RELATIVE         34
#define CON_STANCE_HIP_DAMPING			35
#define CON_STANCE_HIP_MAX_VELOCITY     36

#define CON_D_MIN						37
#define CON_D_MAX						38
#define CON_V_MIN						39
#define CON_V_MAX						40

#define CON_D_TRAJX_START				41
#define CON_D_TRAJX_END					42
#define CON_D_TRAJZ_START				43
#define CON_D_TRAJZ_END					44
#define CON_V_TRAJX_START				45
#define CON_V_TRAJX_END					46
#define CON_V_TRAJZ_START				47
#define CON_V_TRAJZ_END					48
#define CON_ROOT_PRED_TORQUE_SCALE		49

#define CON_CD_NEG						50
#define CON_CD_POS						51
#define CON_CV_NEG_POS					52
#define CON_CV_NEG_NEG					53
#define CON_CV_POS_NEG					54
#define CON_CV_POS_POS					55
#define CON_MAX_FEEDBACK				56
#define CON_MIN_FEEDBACK				57

#define CON_D_SCALE_TRAJECTORY_START	58
#define CON_D_SCALE_TRAJECTORY_END		59

#define CON_V_SCALE_TRAJECTORY_START	60
#define CON_V_SCALE_TRAJECTORY_END		61


/**
	This method is used to determine the type of a line that was used in the input file for a rigid body.
	It is assumed that there are no white spaces at the beginning of the string that is passed in. the pointer buffer
	will be updated to point at the first character after the keyword.
*/
int getConLineType(char* &buffer);

/**
	This method is used to determine the string corresponding to a specific line keyword
*/
const char* getConLineString(int lineType);


