/*
 * vel.c++
 *
 * William Miller
 * Nov 8, 2018
 * 
 * Method definitions for (Vel) class, a structure which holds a three dimensional velocity vector
 *
 */

#include "vel.h"

Vel::Vel() {
	init();
}
Vel::Vel(double X, double Y, double Z) {
	init();
	x = X;
	y = Y;
	z = Z;
}

double Vel::X() {
	return x;
}
double Vel::Y() {
	return y;
}
double Vel::Z() {
	return z;
}
void Vel::setX(double X) {
	x = X;
}
void Vel::setY(double Y) {
	y = Y;
}
void Vel::setZ(double Z) {
	z = Z;
}

std::string Vel::info() {
	std::string s = "("+bright+magenta+std::to_string(x)+res+", "+bright+magenta;
	s += std::to_string(y)+res+", "+bright+magenta+std::to_string(z)+res+")";
	return s;
}

void Vel::init() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
