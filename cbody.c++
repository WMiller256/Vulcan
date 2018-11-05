#include "csim.h"

CBody::CBody() {
	init();
}
CBody::CBody(double Mass, double Radius, double Velocity, bool initialize) {
	if (initialize) init();
	mass = Mass;
	radius = Radius;
	velocity = Velocity;
}
CBody::CBody(double Mass, double Radius, double Velocity, double X, double Y, double Z) {
	CBody(Mass, Radius, Velocity, Pos(X, Y, Z));
}
CBody::CBody(double Mass, double Radius, double Velocity, Pos pos) {
	init();
	x = pos.X();
	y = pos.Y();
	z = pos.Z();
	CBody(Mass, Radius, Velocity, false);
}
CBody::~CBody() {
	// Destructor
}

void CBody::setParent(CBody* Parent) {
	parent = Parent;
}
CBody* CBody::getParent() {
	return parent;
}

int CBody::originDist() {
	return pos().originDist();
}
Pos CBody::pos() {
	return Pos(x, y, z);
}

void CBody::init() {
	parent = NULL;
	radius = 0;
	x = 0;
	y = 0;
	z = 0;
	mass = 0;
	velocity = 0;
}
