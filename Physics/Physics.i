%module Physics

%begin %{
// VC++ issues those warnings when compiling templates
#pragma warning (disable : 4275 4251)
%}

%include "std_string.i"

%{
#include "CollisionDetectionPrimitive.h"
#include "BoxCDP.h"
#include "CapsuleCDP.h"
#include "PlaneCDP.h"
#include "SphereCDP.h"
#include "Joint.h"
#include "StiffJoint.h"
#include "HingeJoint.h"
#include "UniversalJoint.h"
#include "BallInSocketJoint.h"
#include "RigidBody.h"
#include "ArticulatedRigidBody.h"
#include "ArticulatedFigure.h"
#include "World.h"
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

%apply SWIGTYPE *DISOWN { RigidBody* rigidBody_disown };
%apply SWIGTYPE *DISOWN { ArticulatedRigidBody* articulatedRigidBody_disown };
%apply SWIGTYPE *DISOWN { ArticulatedFigure* articulatedFigure_disown };
%apply SWIGTYPE *DISOWN { CollisionDetectionPrimitive* cdp_disown };
%apply SWIGTYPE *DISOWN { Joint* joint_disown };
%apply SWIGTYPE *DISOWN { GLMesh* mesh_disown};
%import "../Utils/Utils.i"
%include "CollisionDetectionPrimitive.h"
%include "BoxCDP.h"
%include "CapsuleCDP.h"
%include "PlaneCDP.h"
%include "SphereCDP.h"
%include "Joint.h"
%include "StiffJoint.h"
%include "HingeJoint.h"
%include "UniversalJoint.h"
%include "BallInSocketJoint.h"
%include "RigidBody.h"
%include "ArticulatedRigidBody.h"
%include "ArticulatedFigure.h"
%include "World.h"

%pythoncode %{
def world():
	return World_instance();
%}

%inline %{
#include <Physics/PhysicsDll.h>

#define PHYSICS_CAST_TO( className ) PHYSICS_DECLSPEC inline className* castTo##className(void* obj) { return (className*)obj; }

PHYSICS_CAST_TO( CollisionDetectionPrimitive )
PHYSICS_CAST_TO( BoxCDP )
PHYSICS_CAST_TO( SphereCDP )
PHYSICS_CAST_TO( PlaneCDP )
PHYSICS_CAST_TO( CapsuleCDP )
PHYSICS_CAST_TO( Joint )
PHYSICS_CAST_TO( StiffJoint )
PHYSICS_CAST_TO( HingeJoint )
PHYSICS_CAST_TO( UniversalJoint )
PHYSICS_CAST_TO( BallInSocketJoint )
PHYSICS_CAST_TO( RigidBody )
PHYSICS_CAST_TO( ArticulatedRigidBody )
PHYSICS_CAST_TO( ArticulatedFigure )
%}


