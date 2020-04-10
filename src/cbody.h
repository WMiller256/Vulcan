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

class CBody {
public:
	CBody();
	CBody(double Mass, double Radius, double Velocity);
	CBody(double Mass, double Radius, double Velocity, double X, double Y, double Z, int H=1);
	CBody(double Mass, double Radius, double Velocity, Pos pos);
	~CBody();

	void Name(std::string newName);
	void Parent(CBody* Parent);
	void Type(bodyType t);

	std::string Name();
	bodyType Type();
	CBody* Parent();
	double Mass();
	double Radius();
	double Speed();
	vec Velocity(vec v);
	vec accelerate(vec a);					// Update the velocity to reflect a given acceleration over given time
	double originDist();

	Pos COM(CBody* target);
	Pos COM(CBody target);

	double distance(CBody* target);			// Calculate the distance to the target (CBody)
	double distance(CBody target) const;
	double distance(Pos* pos);				// Calculate the distance to the target (Pos*)
	double distance(Pos v) const;			// Calculate the distance to the target (Pos)
	double squareDistance(CBody* t);		// Calculate the squared distance to the target 
	double squareDistance(CBody t) const;	// Calculate the squared distance to the target
	double squareDistance(Pos* t);			// Calculate the squared distance to the target
	double squareDistance(Pos t) const;		// Calculate the squared distance to the target

	std::string writeFormat(format f = text); 		// Defined in simio.c++
	std::string info();
	Force net;					// The net force acting on the body
	Pos r;						// Current posision in 3D space (cartesian coordinates)
	Vel v;						// Current velocity in 3D space (cartesian coordinates)
	vec a;						// Current acceleration in 3D space (cartesian coordinates)
	double m;					// Mass of the body
	double h;					// The step size for this body (used for variable-step integrators, i.e. Miller)
	double fix;					// The fix time for this body's position in simulation time
	unsigned long long ncalcs;	// Tracks the number of calculations performed, to ensure synchronization was maintained
	unsigned long long totSteps;
	long idx;					// The index of this body in [read] and [write]

	bool operator != (CBody r) const;
	bool operator == (CBody r) const;

private:
	CBody* parent;				// Parent, i.e. the body that is being orbited
	std::string name;			// The (optional) name of the body
	bodyType type;

	double radius;				// Radius of the body
	double period;				// Orbital period in seconds

	void init();
};

#endif
