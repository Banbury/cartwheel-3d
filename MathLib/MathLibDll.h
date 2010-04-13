#pragma once

#ifndef MATHLIB_DECLSPEC
	#ifdef MATHLIB_EXPORTS
		#define MATHLIB_DECLSPEC    __declspec(dllexport) 
		#define MATHLIB_TEMPLATE(x) template class __declspec(dllexport) x;
	#else
		#define MATHLIB_DECLSPEC    __declspec(dllimport) 
		#define MATHLIB_TEMPLATE(x) template class __declspec(dllimport) x; 
	#endif
#endif