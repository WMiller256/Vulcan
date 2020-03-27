#ifndef CBODY_H
#define CBODY_H

#include "format.h"
#include "force.h"
#include "global.h"
#include "vel.h"
#include "vec.h"
#include "pos.h"
#include "cvector.h"

enum bodyType {
	def,
	planet,
	ghost
};

class CBody
{
public:
	CBody();
	CBody(double Mass, double Radius, double Velocity);
	CBody(double Mass, double Radius, double Velocity, double X, double Y, double Z, int H=1);
	CBody(double Mass, double Radius, double Velocity, Pos pos);
	~CBody();

	std::string Name();
	void Name(std::string newName);
	void setParent(CBody* Parent);
	CBody* getParent();
	void Type(bodyType t);
	bodyType Type();

	double Mass();
	double Radius();
	double Speed();
	vec Velocity(vec v);
	Vel Velocity();
	vec accelerate(vec a);					// Update the velocity to reflect a given acceleration over given time
	void Position(vec v);
	double originDist();
	Pos COM(CBody* target);
	Pos COM(CBody target);
	double distance(CBody* target);			// Calculate the distance to the target (CBody)
	double distance(CBody target) const;
	double distance(Pos* pos);				// Calculate the distance to the target (Pos*)
	double distance(Pos v) const;			// Calculate the distance to the target (Pos)

	std::string writeFormat(format f = text); 		// Defined in simio.c++
	std::string info();
	Force net;					// The net force acting on the body
	Pos pos;
	long h;
	long fix;					// The fix time for this body's position in simulation time
	unsigned long long ncalcs;
	unsigned long long totSteps;
	class BulirschStoer {
	public:
		BulirschStoer(CBody* sim = NULL);
		std::vector<Pos> positions;	// Table for positions for Bulirsch Stoer extrapolation
	private:
		CBody* body;
	};

	BulirschStoer* BS;

	bool operator != (CBody r) const;
	bool operator == (CBody r) const;

private:
	CBody* parent;				// Parent, i.e. the body that is being orbited
	std::string name;			// The (optional) name of the body
	bodyType type;

	double radius;				// Radius of the body
	double x;					// x pos
	double y;					// y pos
	double z;					// z pos
	double xv;					// velocity in x direction
	double yv;					// velocity in y direction
	double zv;					// velocity in z direction
	double mass;				// Mass of the body
	double speed;				// Magnitude of linear velocity
	double period;				// Orbital period in seconds

	void init();
};

#endif
