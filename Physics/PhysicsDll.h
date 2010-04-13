#pragma once

#ifndef PHYSICS_DECLSPEC
	#ifdef PHYSICS_EXPORTS
		#define PHYSICS_DECLSPEC    __declspec(dllexport) 
		#define PHYSICS_TEMPLATE(x) template class __declspec(dllexport) x;
	#else
		#define PHYSICS_DECLSPEC    __declspec(dllimport) 
		#define PHYSICS_TEMPLATE(x) template class __declspec(dllimport) x; 
	#endif
#endif
