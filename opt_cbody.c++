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
//	print(info(), 2);//	print(info(), 2);
}
CBody::CBody(double Mass, double Radius, double Speed, double X, double Y, double Z) {
	init();
	x = X;
	y = Y;
	z = Z;
	pos = Pos(x, y, z);
	mass = Mass;
	radius = Radius;
	speed = Speed;
	xv = Speed;
//	print(info(), 2);//	print(info(), 2);
}
CBody::CBody(double Mass, double Radius, double Speed, Pos p) {
	init();
	x = p.x;
	y = p.y;
	z = p.z;
	pos = p;
	mass = Mass;
	radius = Radius;
	speed = Speed;
	xv = Speed;
//	print(info(), 2);//	print(info(), 2);
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

double CBody::Mass() {
	return mass;
}
double CBody::Radius() {
	return radius;
}
double CBody::Speed() {
	return magnitude(Vel(xv, yv, zv));
}
vec CBody::Velocity(vec v) {
	xv = v.x;
	yv = v.y;
	zv = v.z;
	return vec(xv, yv, zv);
}
Vel CBody::Velocity() {
	return Vel(xv, yv, zv);
}
vec CBody::accelerate(vec a) {		// Accelerate the body by the given vector
	xv += a.x;
	yv += a.y;
	zv += a.z;
	return vec(xv, yv, zv);
}
void CBody::Position(vec v) {
	x = v.x;
	y = v.y;
	z = v.z;
	pos = v;
}

double CBody::originDist() {
	double dist = pos.originDist();
//	printrln(in("CBody", "originDist")+pos.info(), scientific(dist), 4);//	printrln(in("CBody", "originDist")+pos.info(), scientific(dist), 4);
	return dist;
}
Pos CBody::COM(CBody* target) {
#ifdef profiling
	nCOMcalls++;
#endif
	if (this == target) {
//		error("{target} and {this} are the same, center of mass will be {this -> pos}.", __LINE__, __FILE__);//		error("{target} and {this} are the same, center of mass will be {this -> pos}.", __LINE__, __FILE__);
	}
	double m1 = Mass();
	double m2 = target -> Mass();
	double M = m1 + m2;
	if (M > 0.0) {
		return Pos((target -> pos * m1 + pos * m2) / M);		// Surprisingly, returning a (Pos) object instead of a pointer is
	}															// 1000x faster in this case, due to the cost of pointer construction
	else if (M == 0.0) {
//		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);//		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
	else {
//		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);//		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
}
Pos CBody::COM(CBody target) {
#ifdef profiling
	nCOMcalls++;
#endif
	if (*this == target) {
//		error("{target} and {this} are the same, center of mass will be {this -> pos}.", __LINE__, __FILE__);//		error("{target} and {this} are the same, center of mass will be {this -> pos}.", __LINE__, __FILE__);
	}
	double m1 = Mass();
	double m2 = target.Mass();
	double M = m1 + m2;
	if (M > 0.0) {
		return Pos((target.pos * m1 + pos * m2) / M);			// Surprisingly, returning a (Pos) object instead of a pointer is
	}															// 1000x faster in this case, due to the cost of pointer construction
	else if (M == 0.0) {
//		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);//		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
	else {
//		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);//		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
}
double CBody::distance(CBody* target) {
	Pos t = target -> pos;
	double dist = sqrt(pow(t.x - pos.x,2) + pow(t.y - pos.y, 2) + pow(t.z - pos.z, 2));
//	printrln(in("CBody", "distance")+"Distance from "+pos.info(2)+" to "+t.info(2)+" is ", scientific(dist, 3)+res, 4);//	printrln(in("CBody", "distance")+"Distance from "+pos.info(2)+" to "+t.info(2)+" is ", scientific(dist, 3)+res, 4);
	return dist;
}
double CBody::distance(CBody target) const {
	Pos t = target.pos;
	double dist = sqrt(pow(t.x - pos.x,2) + pow(t.y - pos.y, 2) + pow(t.z - pos.z, 2));
//	printrln(in("CBody", "distance")+"Distance from "+pos.info(2)+" to "+t.info(2)+" is ", scientific(dist, 3)+res, 4);//	printrln(in("CBody", "distance")+"Distance from "+pos.info(2)+" to "+t.info(2)+" is ", scientific(dist, 3)+res, 4);
	return dist;	
}
double CBody::distance(Pos* t) {
	double dist = sqrt(pow(t -> x - pos.x,2) + pow(t -> y - pos.y, 2) + pow(t -> z - pos.z, 2));
//	printrln(in("CBody", "distance")+"Distance from "+pos.info()+" to "+t -> info()+" is ", scientific(dist)+res, 4);//	printrln(in("CBody", "distance")+"Distance from "+pos.info()+" to "+t -> info()+" is ", scientific(dist)+res, 4);
	return dist;
}

void CBody::init() {
//	print("Initializing new "+cyan+bright+"CBody"+res+"...");//	print("Initializing new "+cyan+bright+"CBody"+res+"...");

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

	net = Force(0,0,0);
	pos = Pos(0,0,0);

//	print(green+"done\n"+res);//	print(green+"done\n"+res);
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

bool CBody::operator != (CBody r) const {
	if (r == *this) {
		return false;
	}
	else {
		return true;
	}
}
bool CBody::operator == (CBody r) const {
	if (r.pos == pos && r.Mass() == mass) {
		return true;
	}
	else {
		return false;
	}
}
