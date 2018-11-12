/*
 * William Miller
 * Nov 8, 2018
 *
 * Definitions for 'globally' available functions
 *
 */

#include "global.h"

double magnitude(vec v) {
	double ret = sqrt(pow(v.X(), 2) + pow(v.Y(), 2) + pow(v.Z(), 2));
//	println(in("","magnitude")+"      Vector "+v.info()+" magnitude is "+bright+magenta+scientific(ret), 4);//	println(in("","magnitude")+"      Vector "+v.info()+" magnitude is "+bright+magenta+scientific(ret), 4);
	return ret;
}

std::string in(std::string prefix, std::string function) {
	std::string ret;
	if (prefix != "") {
		ret = "  "+cyan+prefix+yellow+"::"+bright+white+function+res+" ";
	}
	else {
		ret = "  "+bright+white+function+res+" ";
	}
	return ret;
}
