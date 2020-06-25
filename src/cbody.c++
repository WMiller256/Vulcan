#include "cbody.h"
#include "simio.h"
#include "global.h"

CBody::CBody() {
	init();
}
CBody::CBody(double Mass, double Radius, double Speed) {
	init();
	m = Mass;
	radius = Radius;
	v.x = Speed;
	print(info(), 2);
}
CBody::CBody(double Mass, double Radius, double Speed, double X, double Y, double Z, double H) {
	init();
	h = H;
	r = Pos(X, Y, Z);
	m = Mass;
	radius = Radius;
	v.x = Speed;
	print(info(), 2);
}
CBody::CBody(double Mass, double Radius, double Speed, Pos p) {
	init();
	r = p;
	m = Mass;
	radius = Radius;
	v.x = Speed;
	print(info(), 2);
}
CBody::~CBody() {
	// Destructor
}

void CBody::Name(std::string newName) { name = newName; }
void CBody::Type(bodyType t) { type = t; }
void CBody::Parent(CBody* Parent) { parent = Parent;}

std::string CBody::Name() {	return name; }
bodyType CBody::Type() { return type; }
CBody* CBody::Parent() { return parent; }
double CBody::Mass()   { return m; }
double CBody::Radius() { return radius; }
double CBody::Speed()  { return v.norm(); }

vec CBody::accelerate(vec dv) {		// Accelerate the body by the given vector
	v += dv;
	return v;
}

double CBody::originDist() {
	double dist = r.norm();
	printrln(in("CBody", "originDist")+r.info(), scientific(dist), 4);
	return dist;
}
Pos CBody::COM(CBody* target) {
	if (this == target) {
		error("{target} and {this} are the same, center of mass will be {this->r}.", __LINE__, __FILE__);
	}
	thread_local double m1 = Mass();
	thread_local double m2 = target->Mass();
	thread_local double M = m1 + m2;
	if (M > 0.0) {
		return Pos((target->r * m1 + r * m2) / M);		// Surprisingly, returning a (Pos) object instead of a pointer is
	}													// 1000x faster in this case, due to the cost of pointer construction
	else if (M == 0.0) {
		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
	else {
		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
}
Pos CBody::COM(CBody target) {
	if (*this == target) {
		error("{target} and {this} are the same, center of mass will be {this->r}.", __LINE__, __FILE__);
	}
	thread_local double m1 = Mass();
	thread_local double m2 = target.Mass();
	thread_local double M = m1 + m2;
	if (M > 0.0) {
		return Pos((target.r * m1 + r * m2) / M);		// Surprisingly, returning a (Pos) object instead of a pointer is
	}													// 1000x faster in this case, due to the cost of pointer construction
	else if (M == 0.0) {
		warning("Sum of masses is zero - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
	else {
		error("Sum of masses is negative - {"+green+"m1"+res+"}: "+scientific(m1)+" {"+green+"m2"+res+"}. Returning (0,0,0).", __LINE__, __FILE__);
		return Pos();
	}
}

double CBody::distance(CBody* t)      { return (t->r - r).norm(); }
double CBody::distance(CBody t) const { return (t.r - r).norm(); }
double CBody::distance(Pos* t)        { return (*t - r).norm(); }
double CBody::distance(Pos t) const   { return (t - r).norm(); }

double CBody::squareDistance(CBody* t)      { return (t->r - r).squared(); }
double CBody::squareDistance(CBody t) const { return (t.r - r).squared(); }
double CBody::squareDistance(Pos* t)        { return (*t - r).squared(); }
double CBody::squareDistance(Pos t) const   { return (t - r).squared(); }

void CBody::init(bool quiet) {
	if (!quiet) print("Initializing new "+cyan+bright+"CBody"+res+"...");

	parent = NULL;
	name = "";
	type = bodyType::def;
	
	radius = 0;
	m = 0;

	net = Force(0,0,0);
	r = Pos(0,0,0);
	v = Vel(0,0,0);
	h = 1;
	fix = 0;
	idx = 0;

	ncalcs = 0;
	totSteps = 0;
	
	if (!quiet) print(green+"done\n"+res);
}

std::string CBody::info() {
	std::string s = "";
	s.append("Body "+name+"\n");
	s.append("\t Position "+r.info()+"\n");
	s.append("\t Radius   "+scientific(radius, 5)+"\n");
	s.append("\t Mass     "+scientific(m, 5)+"\n");
	return s;
}

std::string CBody::writeFormat(format f) {
	std::string formatted = "";
	switch(f) {
		case text:
			formatted.append("Body - "+Name()+"\n");															 	// Exclude
			formatted.append("   Position "+scientific(r.x,5)+" "+scientific(r.y,5)+" "+scientific(r.z,5)+"\n"); 	// Exclude
			formatted.append("   Radius   "+scientific(radius)+"\n");											 	// Exclude
			formatted.append("   Mass     "+scientific(m)+"\n");												 	// Exclude
			formatted.append("   Velocity "+scientific(v.x,5)+" "+scientific(v.y,5)+" "+scientific(v.z,5)+"\n\n");	// Exclude
			break;
	}
	return formatted;
}

bool CBody::operator != (CBody rbody) const { return (rbody == *this); }
bool CBody::operator == (CBody rbody) const { return (rbody.r == r && rbody.Mass() == m); }
