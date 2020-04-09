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
#include "simio.h"

class vec
{
public:
	vec();
	vec(double X, double Y, double Z);
	vec( vec* v);

	double x;
	double y;
	double z;

	std::string info(int precision = 5) const;
	void zero();
	double squared() const;
	double to(int n) const;
	double norm() const;

	bool operator != (vec r) const;
	bool operator == (vec r) const;
	vec operator / (double scalar);
	vec operator * (double scalar);
	vec operator * (long scalar);
	double operator * (vec const &r);
	vec operator + (vec const &r);
	vec operator - (vec const &r);
	vec operator += (vec const &r);
	void operator = (vec const &r);
	double operator [] (const int idx);

private:
	
	void init();

};

#endif // VEC_H
