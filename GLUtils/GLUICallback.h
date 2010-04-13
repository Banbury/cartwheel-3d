#pragma once

#include <GLUtils/GLUtilsDll.h>

class GLUTILS_DECLSPEC GLUICallback {
public:
	GLUICallback() {}
	virtual ~GLUICallback() {}
	virtual void execute() {}

};

