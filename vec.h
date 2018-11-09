/*
 * vec.h
 *
 * William Miller
 * Nov 8, 2018
 *
 * Class definition for (vec)
 *
 *
 */

#ifndef VEC_H
#define VEC_H

#include <string>

#include "colors.h"
#include "baseio.h"

class vec
{
public:
	vec();
	vec(double X, double Y, double Z);

	double X();
	double Y();
	double Z();
	void setX(double X);
	void setY(double Y);
	void setZ(double Z);

	std::string info(int precision = 5);

	bool operator == (vec r) const;
	vec operator / (double scalar);
	vec operator * (double scalar);
	vec operator += (vec r);

private:
	double x;
	double y;
	double z;
	
	void init();

};

#endif // VEC_H
