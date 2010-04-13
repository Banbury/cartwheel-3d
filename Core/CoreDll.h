#pragma once

#ifndef CORE_DECLSPEC
	#ifdef CORE_EXPORTS
		#define CORE_DECLSPEC    __declspec(dllexport) 
		#define CORE_TEMPLATE(x) template class __declspec(dllexport) x;
	#else
		#define CORE_DECLSPEC    __declspec(dllimport) 
		#define CORE_TEMPLATE(x) template class __declspec(dllimport) x; 
	#endif
#endif
