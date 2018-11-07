#include "csim.h"

CBody::CBody() {
	init();
}
CBody::CBody(double Mass, double Radius, double Speed, bool initialize) {
	if (initialize) init();
	mass = Mass;
	radius = Radius;
	speed = Speed;
}
CBody::CBody(double Mass, double Radius, double Speed, double X, double Y, double Z) {
	CBody(Mass, Radius, Speed, Pos(X, Y, Z));
}
CBody::CBody(double Mass, double Radius, double Speed, Pos pos) {
	init();
	x = pos.X();
	y = pos.Y();
	z = pos.Z();
	CBody(Mass, Radius, Speed, false);
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
	speed = 0;
}
