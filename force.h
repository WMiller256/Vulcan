/*
 * William Miller
 * Nov 8, 2018
 * 
 * Class definition for (Force) data structure that contains the 
 * three dimensional moment on a given body
 *
 */

#ifndef FORCE_H
#define FORCE_H

#include "vec.h"

class Force : public vec
{
public:
	Force();
	Force(double X, double Y, double Z);

private:

};

#endif // FORCE_H
