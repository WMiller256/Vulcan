#include "csim.h"
#include "simio.h"
#include "global.h"

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
	printrln(in("CBody", "originDist")+pos().info(), scientific(dist), 4);
	return dist;
}
Pos CBody::COM(CBody* target) {
	if (this == target) {
		error("{target} and {this} are the same, center of mass will be {this -> pos()}.", __LINE__, __FILE__);
	}
	Pos com(0,0,0);
	double m1 = Mass();
	double m2 = target -> Mass();
	double M = m1 + m2;
	Pos tpos = target -> pos();
	Pos lpos = pos();
	if (M > 0.0) {
		com.setX((m1*lpos.X() + m2*tpos.X()) / M);
		com.setY((m1*lpos.Y() + m2*tpos.Y()) / M);
		com.setZ((m1*lpos.Z() + m2*tpos.Z()) / M);
	}
	else if (M == 0.0) {
		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
	}
	else {
		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
	}
	printrln(in("CBody", "COM")+"     COM of "+Name()+" and "+target -> Name()+" is ", com.info(2), 4);
	return com;
}
double CBody::distance(CBody* target) {
	Pos t = target -> pos();
	double dist = sqrt(pow(t.X() - pos().X(),2) + pow(t.Y() - pos().Y(), 2) + pow(t.Z() - pos().Z(), 2));
	printrln(in("CBody", "distance")+"Distance from "+pos().info(2)+" to "+t.info(2)+" is ", scientific(dist, 3)+res, 4);
	return dist;
}
double CBody::distance(Pos t) {
	double dist = sqrt(pow(t.X() - pos().X(),2) + pow(t.Y() - pos().Y(), 2) + pow(t.Z() - pos().Z(), 2));
	printrln(in("CBody", "distance")+"Distance from "+pos().info()+" to "+t.info()+" is ", scientific(dist)+res, 4);
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
