#include "csim.h"
#include "simio.h"

CBody::CBody() {
	init();
}
CBody::CBody(double Mass, double Radius, double Speed) {
	init();
	mass = Mass;
	radius = Radius;
	speed = Speed;
	xv = Speed;
	print(info(), 2);
}
CBody::CBody(double Mass, double Radius, double Speed, double X, double Y, double Z) {
	init();
	x = X;
	y = Y;
	z = Z;
	mass = Mass;
	radius = Radius;
	speed = Speed;
	xv = Speed;
	print(info(), 2);
}
CBody::CBody(double Mass, double Radius, double Speed, Pos pos) {
	init();
	x = pos.X();
	y = pos.Y();
	z = pos.Z();
	mass = Mass;
	radius = Radius;
	speed = Speed;
	xv = Speed;
	print(info(), 2);
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

double CBody::originDist() {
	double dist = pos().originDist();
	print("In ["+bright+blue+"CBody::originDist()"+res+"] ", 4);
	println(pos().info()+" "+std::to_string(dist), 4);
	return dist;
}
Pos CBody::pos() {
	return Pos(x, y, z);
}

void CBody::init() {
	print("Initializing new "+cyan+bright+"CBody"+res+"...");
	parent = NULL;
	radius = 0;
	x = 0;
	y = 0;
	z = 0;
	xv = 0;
	yv = 0;
	zv = 0;
	mass = 0;
	speed = 0;
	print(green+"done\n"+res);
}

std::string CBody::info() {
	std::string s = "";
	s.append("Body info:\n");
	s.append("\t Position ("+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z)+")\n");
	s.append("\t Radius   "+std::to_string(radius)+"\n");
	s.append("\t Mass     "+std::to_string(mass)+"\n");
	s.append("\t Speed    "+std::to_string(speed)+"\n\n");
	return s;
}
