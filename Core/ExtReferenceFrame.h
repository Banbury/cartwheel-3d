/*
 * A Reference frame that can be used to express the target orientation of an articulated
 * body driven by a controller.
 */
#pragma once

#include <Utils/Utils.h>
#include <MathLib/ReferenceFrame.h>

class  ExtReferenceFrame {

public:
	
	ReferenceFrame evaluate( const Quaternion& parentFrame, const Quaternion& characterFrame ) 


};

class ParentFrame : ExtReferenceFrame {

	ReferenceFrame evaluate( const Quaternion& parentFrame, const Quaternion& characterFrame ) 


};

class CharacterFrame : ExtReferenceFrame {
	
};