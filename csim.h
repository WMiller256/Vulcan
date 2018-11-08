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

#include <iostream>
#include <iomanip>
#include <experimental/filesystem>
#include "colors.h"
#include <math.h>
#include <string.h>
#include <vector>
#include <limits>
#include <omp.h>

#include "force.h"
#include "global.h"
#include "vel.h"
#include "pos.h"
#include "hash.h"

#define G 6.67408e-11

extern int nbodies;
extern bool warnings;
extern int debug;
extern double minradius;
extern double maxradius;
extern double blockwidth;		// The width of each hash block

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
	CSim(int n, double max);
	~CSim();

	void addBody(CBody* body);
	void writeConfiguration(const std::string& filename);
	void readConfiguration(const std::string& filename);

private:
	double tMax;		// The integration time
	double tCurr;		// Current time
	Hash* bodies;		// Hash table to hold the body objects

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
	CBody(double Mass, double Radius, double Velocity, double X, double Y, double Z);
	CBody(double Mass, double Radius, double Velocity, Pos pos);
	~CBody();

	std::string Name();
	void Name(std::string newName);
	void setParent(CBody* Parent);
	CBody* getParent();

	Pos pos();
	double Mass();
	double Radius();
	double Speed();
	double originDist();
	double distance(CBody* target);			// Calculate the distance to the target (CBody)
	double distance(Pos pos);				// Calculate the distance to the target (Pos)

	std::string writeFormat(format f = text); 		// Defined in simio.c++
	std::string info();

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

#endif // HASH_H
