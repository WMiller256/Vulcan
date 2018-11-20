/*
 * William Miller
 * Nov 8, 2018
 *
 * Definitions for 'globally' available functions
 *
 */

#include "global.h"

double magnitude(vec v) {
	double ret = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	printrln(in("","magnitude")+"      Vector "+v.info()+" magnitude is ", scientific(ret), 4);
	return ret;
}
