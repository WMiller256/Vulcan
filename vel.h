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

class Vel
{
public:
	Vel();
	Vel(double X, double Y, double Z);

	double X();
	double Y();
	double Z();
	void setX(double X);
	void setY(double Y);
	void setZ(double Z);

	std::string info();

private:
	double x;
	double y;
	double z;

	void init();
};

#endif // VEL_H
