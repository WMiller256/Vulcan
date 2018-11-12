/*
 * rpos.h
 *
 * William Miller
 * Nov 11, 2018
 *
 * Class definition for the (vec) subclass (rpos), a container
 * to hold a position vector and an index corresponding to the
 * body whose position it is. 
 *
 *
 */

#ifndef RPOS_H
#define RPOS_H

#include "vec.h"		// Base class

class rpos : public vec
{
public:
	rpos();
	rpos(vec p, int index);

private:
	int idx;
};

#endif // RPOS_H
