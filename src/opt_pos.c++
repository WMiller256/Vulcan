#include "csim.h"
#include "simio.h"
#include "global.h"

Pos::Pos() : vec() {}
Pos::Pos(double X, double Y, double Z) : vec(X, Y, Z) {}
Pos::Pos(vec v) : vec(v) {}

double Pos::originDist() { return norm(); }
vec Pos::direction(Pos* target) {
	vec v(target->x - x, target->y - y, target->z - z);
	v = v / magnitude(v);
	return v;
}
vec Pos::direction(Pos target) {
	vec v(target.x - x, target.y - y, target.z - z);
	v = v / magnitude(v);
	return v;
}
