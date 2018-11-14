/*
 * pos.h
 *
 * William Miller
 * Nov 8, 2018
 *
 * Class definition for Pos, a structure to hold a three dimensional position vector
 *
 *
 */

#ifndef POS_H
#define POS_H

#include "vec.h"

class Pos : public vec
{
public:
	Pos();
	Pos(double X, double Y, double Z);
	Pos(vec v);

	double originDist();
	vec direction(Pos* target);
	vec direction(Pos target);

private:
};

#endif // POS_H
