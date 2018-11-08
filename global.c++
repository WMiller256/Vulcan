/*
 * William Miller
 * Nov 8, 2018
 *
 * Definitions for 'globally' available functions
 *
 */

#include "global.h"

double magnitude(Vel vel) {
	double ret = sqrt(pow(vel.X(), 2) + pow(vel.Y(), 2) + pow(vel.Z(), 2));
	println(in("","magnitude(Vel)")+"Vector "+vel.info()+" magnitude is "+bright+magenta+std::to_string(ret), 4);
	return ret;
}
double magnitude(Pos pos) {
	double ret = sqrt(pow(pos.X(), 2) + pow(pos.Y(), 2) + pow(pos.Z(), 2));
	println(in("","magnitude(Vel)")+"Vector "+pos.info()+" magnitude is "+bright+magenta+std::to_string(ret), 4);
	return ret;	
}

std::string in(std::string prefix, std::string function) {
	std::string ret;
	if (prefix != "") {
		ret = "In ["+cyan+prefix+yellow+"::"+bright+white+function+res+"()] ";
	}
	else {
		ret = "In ["+bright+white+function+res+"()] ";
	}
	return ret;
}

