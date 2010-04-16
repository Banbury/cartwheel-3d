#include "BoxCDP.h"
#include <GLUtils/GLUtils.h>

BoxCDP::~BoxCDP(void){

}

/**
	Draw an outline of the box
*/
void BoxCDP::draw(){
	GLUtils::drawBox(p1, p2);
}

