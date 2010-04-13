%module(directors="1") Core

%begin %{
// VC++ issues those warnings when compiling templates
#pragma warning (disable : 4275 4251)
%}

%include "std_string.i"

%{
#include "SimGlobals.h"
#include "Character.h"
#include "BalanceFeedback.h"
#include "SimBiConState.h"
#include "Controller.h"
#include "PoseController.h"
#include "SimBiController.h"
#include "IKVMCController.h"
#include "WorldOracle.h"
#include "BehaviourController.h"
#include "TurnController.h"
#include "DuckController.h"
#include "TwoLinkIK.h"
%}

// SWIG compiler does not support VC++ declspec
#define GLUTILS_DECLSPEC
#define GLUTILS_TEMPLATE(x)
#define UTILS_DECLSPEC
#define UTILS_TEMPLATE(x) 
#define MATHLIB_DECLSPEC
#define MATHLIB_TEMPLATE(x)
#define PHYSICS_DECLSPEC
#define PHYSICS_TEMPLATE(x)
#define CORE_DECLSPEC
#define CORE_TEMPLATE(x)


%feature("director") Controller::performPreTasks;
%feature("director") Controller::performPostTasks;
%apply SWIGTYPE *DISOWN { RigidBody* rigidBody_disown };
%apply SWIGTYPE *DISOWN { ArticulatedFigure* articulatedFigure_disown };
%apply SWIGTYPE *DISOWN { Character* character_disown };
%apply SWIGTYPE *DISOWN { SimBiController* controller_disown };
%apply SWIGTYPE *DISOWN { TrajectoryComponent* trajComp_disown };
%apply SWIGTYPE *DISOWN { ExternalForce* extForce_disown };
%apply SWIGTYPE *DISOWN { Trajectory* traj_disown };
%apply SWIGTYPE *DISOWN { SimBiConState* state_disown };
%apply SWIGTYPE *DISOWN { BalanceFeedback* feedback_disown };
%apply SWIGTYPE *DISOWN { Trajectory1d* traj1d_disown };
%apply SWIGTYPE *DISOWN { BehaviourController* behaviour_disown };
%import "../Utils/Utils.i"
%import "../Physics/Physics.i"
%import "../MathLib/MathLib.i"
%include "SimGlobals.h"
%include "Character.h"
%include "BalanceFeedback.h"
%include "SimBiConState.h"
%include "Controller.h"
%include "PoseController.h"
%include "SimBiController.h"
%include "IKVMCController.h"
%include "WorldOracle.h"
%include "BehaviourController.h"
%include "TurnController.h"
%include "DuckController.h"
%include "TwoLinkIK.h"

%inline %{
#include <Core/CoreDll.h>

#define CORE_CAST_TO( className ) CORE_DECLSPEC inline className* castTo##className(void* obj) { return (className*)obj; }

CORE_CAST_TO( Character )
CORE_CAST_TO( Controller )
CORE_CAST_TO( PoseController )
CORE_CAST_TO( SimBiController )
CORE_CAST_TO( IKVMCController )
CORE_CAST_TO( ControlParams )
CORE_CAST_TO( SimBiConState )
CORE_CAST_TO( ExternalForce )
CORE_CAST_TO( Trajectory )
CORE_CAST_TO( TrajectoryComponent )
CORE_CAST_TO( BalanceFeedback )
CORE_CAST_TO( LinearBalanceFeedback )
%}

