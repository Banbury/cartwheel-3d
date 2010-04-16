%module MathLib

%begin %{
// VC++ issues those warnings when compiling templates
#pragma warning (disable : 4275 4251)
%}

%include "std_string.i"

%{
#include "Matrix.h"
#include "TransformationMatrix.h"
#include "Vector.h"
#include "ThreeTuple.h"
#include "Vector3d.h"
#include "Point3d.h"
#include "Quaternion.h"
#include "Trajectory.h"
%}

// SWIG compiler does not support VC++ declspec
#define UTILS_DECLSPEC
#define UTILS_TEMPLATE(x)
#define MATHLIB_DECLSPEC
#define MATHLIB_TEMPLATE(x)

// Ignore Warning(362): operator= ignored
#pragma SWIG nowarn=362
%import "../Utils/Utils.i"
%include "Matrix.h"
%include "TransformationMatrix.h"
%include "Vector.h"
%include "ThreeTuple.h"
%include "Vector3d.h"
%include "Point3d.h"
%include "Quaternion.h"
%include "Trajectory.h"

%template(Trajectory1d) GenericTrajectory<double>;
%template(Trajectory3d) GenericTrajectory<Point3d>;
%template(Trajectory3dv) GenericTrajectory<Vector3d>;