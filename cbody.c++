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

std::string CBody::Name() {
	return name;
}
void CBody::Name(std::string newName) {
	name = newName;
}
void CBody::setParent(CBody* Parent) {
	parent = Parent;
}
CBody* CBody::getParent() {
	return parent;
}

Pos CBody::pos() {
	return Pos(x, y, z);
}
double CBody::Mass() {
	return mass;
}
double CBody::Radius() {
	return radius;
}
double CBody::Speed() {
	return magnitude(Vel(xv, yv, zv));
}

double CBody::originDist() {
	double dist = pos().originDist();
	print(in("CBody", "originDist"), 4);
	println(pos().info()+" "+std::to_string(dist), 4);
	return dist;
}
double CBody::distance(CBody* target) {
	Pos pos = target -> pos();
	double dist = sqrt(pow(pos.X(),2) + pow(pos.Y(), 2) + pow(pos.Z(), 2));
	println(in("CBody", "distance")+"Distance to "+pos.info()+" is "+bright+magenta+std::to_string(dist)+res, 4);
	return dist;
}
double CBody::distance(Pos pos) {
	double dist = sqrt(pow(pos.X(),2) + pow(pos.Y(), 2) + pow(pos.Z(), 2));
	println(in("CBody", "distance")+"Distance to "+pos.info()+" is "+bright+magenta+std::to_string(dist)+res, 4);
	return dist;
}

void CBody::init() {
	print("Initializing new "+cyan+bright+"CBody"+res+"...");

	parent = NULL;
	name = "";
	
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
