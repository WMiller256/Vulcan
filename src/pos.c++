#include "csim.h"
#include "simio.h"
#include "global.h"

Pos::Pos() : vec() {}
Pos::Pos(double X, double Y, double Z) : vec(X, Y, Z) {}
Pos::Pos(vec v) : vec(v) {}

double Pos::originDist() { return norm(); }
vec Pos::direction(Pos* target) {
	vec v(target->x - x, target->y - y, target->z - z);
	v = v / v.norm();
	printrln(in("Pos","direction")+"            {"+green+"target"+res+"} - "+target->info(2)+" to {"+green+"this"+res+"} - "+info(2), v.info(2), 6);
	return v;
}
vec Pos::direction(Pos target) {
	vec v(target.x - x, target.y - y, target.z - z);
	v = v / v.norm();
	printrln(in("Pos","direction")+"            {"+green+"target"+res+"} - "+target.info(2)+" to {"+green+"this"+res+"} - "+info(2), v.info(2), 6);
	return v;
}
