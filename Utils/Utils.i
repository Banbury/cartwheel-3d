%module(directors="1") Utils

%begin %{
// VC++ issues those warnings when compiling templates
#pragma warning (disable : 4275 4251)
%}

%{
#include "Utils.h"
#include "Observer.h"
#include "Observable.h"
%}

#define UTILS_DECLSPEC
#define UTILS_TEMPLATE(x)

%feature("director") Observer; 
%include "std_vector.i"
%include "Utils.h"
%include "Observer.h"
%include "Observable.h"

namespace std {
	%template(DynamicArrayDouble) DynamicArray<double>;
};