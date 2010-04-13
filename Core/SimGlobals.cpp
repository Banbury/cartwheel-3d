#include "SimGlobals.h"

//initialize all the parameters to some sensible values.

//give this a very high value so that we can use the scripted values in the rb specs for the value to use
int SimGlobals::forceHeadingControl = false;
double SimGlobals::desiredHeading = 0;
double SimGlobals::dt = 1.0/(2000.0);
World* SimGlobals::activeRbEngine = NULL;

double SimGlobals::conInterpolationValue;
double SimGlobals::bipDesiredVelocity;



double SimGlobals::targetPos = 0;


double SimGlobals::targetPosX = 0;
double SimGlobals::targetPosZ = 0;

int SimGlobals::constraintSoftness = 1;
int SimGlobals::CGIterCount = 0;
int SimGlobals::linearizationCount = 1;

double SimGlobals::rootSagittal = 0;
double SimGlobals::stanceKnee = 0;
/*

double SimGlobals::style = 0;

double SimGlobals::rootLateral = 0;
double SimGlobals::swingHipSagittal = 0;
double SimGlobals::swingHipLateral = 0;
double SimGlobals::stanceAngleSagittal = 0;
double SimGlobals::stanceAngleLateral = 0;

double SimGlobals::VDelSagittal = 0;
double SimGlobals::stepHeight = 0;
double SimGlobals::stepTime = 0.6;
double SimGlobals::duckWalk = 0;
double SimGlobals::upperBodyTwist = 0;
double SimGlobals::coronalStepWidth = 0.1;


double SimGlobals::COMOffsetX = 0;
double SimGlobals::COMOffsetZ = 0;
*/

