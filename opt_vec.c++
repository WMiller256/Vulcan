/*
 * vec.c++
 *
 * William Miller
 * Nov 8, 2018
 *
 * Implementation for class (vec)
 *
 *
 */

#include "vec.h"

vec::vec() {
	init();
}
vec::vec(double X, double Y, double Z) {
	init();
	x = X;
	y = Y;
	z = Z;
}
vec::vec(vec* v) {
	x = v -> X();
	y = v -> Y();
	z = v -> Z();
}

double vec::X() {
	return x;
}
double vec::Y() {
	return y;
}
double vec::Z() {
	return z;
}
void vec::setX(double X) {
	x = X;
}
void vec::setY(double Y) {
	y = Y;
}
void vec::setZ(double Z) {
	z = Z;
}

std::string vec::info(int precision) {
	std::string s = bright+magenta+scientific(x, precision)+res+", "+bright+magenta;
	s += scientific(y, precision)+res+", "+bright+magenta+scientific(z, precision)+res;
	return s;
}

bool vec::operator == (vec r) const {
	if (x == r.X() && y == r.Y() && z == r.Z()) {
		return true;
	}
	else {
		return false;
	}
}
vec vec::operator / (double scalar) {
	return vec(x / scalar, y / scalar, z / scalar);
}
vec vec::operator * (double scalar) {
	return vec(x * scalar, y * scalar, z * scalar);
}
vec vec::operator += (vec r) {
	x += r.X();
	y += r.Y();
	z += r.Z();
}

void vec::init() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
