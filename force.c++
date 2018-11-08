/*
 * William Miller
 * Nov 8, 2018
 * 
 * Implementation file for force vector data structure
 *
 *
 */

#include "force.h"

Force::Force() {
	init();
}
Force::Force(double X, double Y, double Z) {
	init();
	x = X;
	y = Y;
	z = Z;
}

void Force::init() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
