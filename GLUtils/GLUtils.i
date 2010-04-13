%module(directors="1") GLUtils

%begin %{
// VC++ issues those warnings when compiling templates
#pragma warning (disable : 4275 4251)
%}

%{
#include "GLUtils.h"
#include "GLCamera.h"
#include "GLTexture.h"
#include "GLMesh.h"
#include "GLUI.h"
#include "GLUIWindow.h"
#include "GLUIContainer.h"
#include "GLUITopLevelWindow.h"
#include "GLUICurveEditor.h"
#include "GLUICheckBox.h"
#include "GLUISizer.h"
#include "GLUICallback.h"
%}

// SWIG compiler does not support VC++ declspec
#define UTILS_DECLSPEC
#define UTILS_TEMPLATE(x)
#define GLUTILS_DECLSPEC
#define GLUTILS_TEMPLATE(x)

// Ignore Warning(362): operator= ignored
#pragma SWIG nowarn=362

%apply SWIGTYPE *DISOWN { GLUISizer* sizer_disown };
%feature("director") GLUIWindow;
%feature("director") GLUIContainer;
%feature("director") GLUITopLevelWindow;
%feature("director") GLUICallback;
%feature("nodirector") GLUIWindow::onMouseEventHandler;
%feature("nodirector") GLUIWindow::refresh;
%feature("nodirector") GLUIContainer::onMouseEventHandler;
%feature("nodirector") GLUIContainer::refresh;
%feature("nodirector") GLUITopLevelWindow::onMouseEventHandler;
%feature("nodirector") GLUITopLevelWindow::refresh;
%import "../Utils/Utils.i"
%import "../MathLib/MathLib.i"
%include "GLUtils.h"
%include "GLCamera.h"
%include "GLTexture.h"
%include "GLMesh.h"
%include "GLUI.h"
%include "GLUIWindow.h"
%include "GLUIContainer.h"
%include "GLUITopLevelWindow.h"
%include "GLUICurveEditor.h"
%include "GLUICheckBox.h"
%include "GLUISizer.h"
%include "GLUICallback.h"
