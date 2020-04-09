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

#include "simio.h"		// For [scientific]

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
	s = bright+magenta+scientific(x, precision)+res+" "+bright+magenta;					// Exclude
	s += scientific(y, precision)+res+" "+bright+magenta+scientific(z, precision)+res;	// Exclude
	return s;
}
void vec::zero() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

double vec::squared() const { return this->to(2); }
double vec::to(int n) const { return (pow(x, n) + pow(y, n) + pow(z, n)); }
double vec::norm() const    { return sqrt(this->to(2)); }
vec vec::unit()        		{ return *this / norm(); }

bool vec::operator != (vec r) const   { return (r == *this); }
bool vec::operator == (vec r) const   { return (x == r.x && y == r.y && z == r.z); }
vec operator / (double s, vec const &v) { return vec(v.x / s, v.y / s, v.z / s); }
vec operator * (double s, vec const &v) { return vec(v.x * s, v.y * s, v.z * s); }
vec operator * (long s, vec const &v)   { return vec(v.x * s, v.y * s, v.z * s); }
vec operator / (vec const &v, double s) { return vec(v.x / s, v.y / s, v.z / s); }
vec operator * (vec const &v, double s) { return vec(v.x * s, v.y * s, v.z * s); }
vec operator * (vec const &v, long s)   { return vec(v.x * s, v.y * s, v.z * s); }
double vec::operator * (vec const &r) { return x * r.x + y * r.y + z * r.z; }
vec vec::operator + (vec const &r)    { return vec(x + r.x, y + r.y, z + r.z); }
vec vec::operator - (vec const &r)    { return vec(x - r.x, y - r.y, z - r.z); }
vec vec::operator += (vec const &r)   { return vec(x + r.x, y + r.y, z + r.z); }
void vec::operator = (vec const &r) { 
	x = r.x;
	y = r.y;
	z = r.z;
}
double vec::operator [] (int idx) {
	if (idx > 2) {
		exit(2);
	}
	switch (idx) {
		case 0:	return x;
		case 1:	return y;
		case 2:	return z;
	}
}

void vec::init() {
	this->zero();
}
