/*
 * csim.h
 *
 * William Miller
 * Nov 3, 2018
 *
 * Header for CSim, CBody, Pos and Hash classes. CSim contains
 * a (Hash) table of (CBody)s each of which contains the
 * orbital elements for each body in the simulation as well as
 * methods to simulate the given configuration of (CBody)s. 
 * Pos is a convenience class for storing a cartesian set of 
 * coordinates. 
 *
 */

#ifndef CSIM_H
#define CSIM_H
//#define profiling
#define multhreading

#include <iostream>
#include <iomanip>
#include <experimental/filesystem>
#include "colors.h"
#include <math.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <limits>
#include <omp.h>

#include "force.h"
#include "global.h"
#include "vel.h"
#include "vec.h"
#include "pos.h"
#include "rpos.h"
#include "hash.h"

#define G 6.67408e-11

extern int nbodies;
extern int nthreads;
extern bool warnings;
extern int debug;
extern double minradius;
extern double maxradius;
extern double blockwidth;		// The width of each hash block
extern long long cputime;
extern long long mtxtime;
extern long long waittime;
extern long long nCOMcalls;
extern double cofactor;
extern std::atomic<unsigned long long> simTime;
extern std::atomic<int> tocalc;
extern std::atomic<int> toput;
extern std::atomic<int> joinable;

class CBody;			// Forward declared for use in CSim

enum threadmode {
	single,
	omp,
	manual
};
enum format {
	text
};

class CSim
{
public:
	CSim();
	CSim(int n);
	CSim(int n, double max, double h);
	~CSim();

	void setDebug(int Debug);

	void addBody(CBody* body);
	CBody* at(int ii);
	CBody copy(int ii);
	void step();
	void force(CBody* body);
	Force force(CBody body);
	Force mforce(CBody body);

	void printForces();
	void writeConfiguration(const std::string& filename, bool overwrite = false);
	CSim* readConfiguration(const std::string& filename);

	double H();
	int count();
	void sim(threadmode t = threadmode::single);
	void man_simulate(int ii);
	void simulate(unsigned long end);

private:
	double tMax;		// The integration time
	double tCurr;		// Current time
	double h;			// The time step
#ifdef using_hash
	Hash* bodies;		// Hash table to hold the body objects
#else
	CBody** bodies;
	int nadded;
#endif

	void init();
};

class BulirschStoer : public CSim
{
public:
	BulirschStoer();
	double step(double (*f)(double, double), double y0, double x0, double x, int nsteps);
	int simulate(double (*f)(double, double), double y0, double* y1, double x, double h, double* h_new, 
		double epsilon, double yscale, int rational_extrapolate);
	static int rationalExtrapolation(double* fzero, double* tableau, double* x, double f, int n);
	static int polynomialExtrapolation(double* fzero, double* tableau, double* x, double f, int n);

	void setThreading(threadmode t);
	threadmode getThreading();

private:
	static int attempts;
	static int steps[];
	threadmode threading;
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

	bool operator != (CBody r) const;
	bool operator == (CBody r) const;

private:
	CBody* parent;				// Parent, i.e. the body that is being orbited
	std::string name;			// The (optional) name of the body

	double radius;				// Radius of the body
	double x;					// x pos
	double y;					// y pos
	double z;					// z pos
	double xv;					// velocity in x direction
	double yv;					// velocity in y direction
	double zv;					// velocity in z direction
	double mass;				// Mass of the body
	double speed;				// Magnitude of linear velocity

	void init();
};

void sim(Hash* bodies, double tMax, threadmode t = threadmode::single);
void simulate(Hash* h, CBody* body, double t);
#endif // HASH_H
