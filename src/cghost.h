/*
 * cghost.h
 *
 * William Miller
 * Dec 24, 2018 1:13 AM MST
 *
 * Class definition for (CBody) subclass (CGhost), 
 * calculation oriented bodies that whose influence
 * both on each other and on (CBody) objects is 
 * neglected, but who are affected by all (CBody)
 * objects (subject to interaction strength and user
 * specified interaction constraints).
 */

#ifndef CGHOST_H
#define CGHOST_H

#include "cbody.h"

class CGhost : public CBody {

public:
	CGhost();
	CGhost(double Radius, double Velocity, double X, double Y, double Z, int H=1);

};

#endif // CGHOST_H
