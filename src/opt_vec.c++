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
	x = v -> x;
	y = v -> y;
	z = v -> z;
}

std::string vec::info(int precision) const {
	std::string s;
	s = bright+magenta+scientific(x, precision)+res+", "+bright+magenta;
	s += scientific(y, precision)+res+", "+bright+magenta+scientific(z, precision)+res;
	return s;
}

bool vec::operator != (vec r) const {
	if (r == *this) {
		return false;
	}
	else {
		return true;
	}
}
bool vec::operator == (vec r) const {
	if (x == r.x && y == r.y && z == r.z) {
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
vec vec::operator + (vec const &r) {
	return vec(x + r.x, y + r.y, z + r.z);
}
void vec::operator += (vec const &r) {
	x = x + r.x;
	y = y + r.y;
	z = z + r.z;
}

void vec::init() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
