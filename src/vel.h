/*
 * vel.h
 * 
 * William Miller
 * Nov 8, 2018
 *
 * Class definition for Vel, a structure to hold a three dimensional velocity vector
 *
 *
 */
#ifndef VEL_H
#define VEL_H

#include <iostream>
#include <string>

#include "colors.h"
#include "vec.h"

class Vel : public vec
{
public:
	Vel();
	Vel(double X, double Y, double Z);

private:

};

#endif // VEL_H
