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

#define multhreading
#define G 6.67408e-11
//#define profiling
#define SIMTIME_TYPE 2

enum threadmode {
	single,
	omp,
	manual
};
enum format {
	text
};
enum bodyType {
	def,
	planet,
	ghost
};

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
#include <atomic>
#include <limits>
#include <omp.h>

#include "cbody.h"
#include "force.h"
#include "global.h"
#include "vel.h"
#include "vec.h"
#include "pos.h"
#include "rpos.h"


extern int nbodies;
extern int nthreads;
extern bool warnings;
extern int debug;
extern unsigned long long cputime;
extern unsigned long long waittime;
extern unsigned long long polltime;
extern unsigned long long simulationtime;

class CBody;		// Forward declaration here is mandatory
class CGhost;

class CSim
{
public:
	CSim();
	CSim(int n);
	CSim(int n, double max, double h);
	~CSim();

	void setDebug(int Debug);

	void addBody(CBody* body);
	void addGhost(CGhost* ghost);
	CBody* at(int ii);
	CBody copy(int ii);

	void printForces();
	int writeConfiguration(const std::string& filename, bool overwrite = false);
	CSim* readConfiguration(const std::string& filename);

	double H();
	int count();
	int NReal();
	int NGhost();

	void step();
	Force force(CBody* body);
	void fixedHForce(CBody* body, CBody* wbody);
	void sim(threadmode t = threadmode::single);
	void threadedFixedH(int min, int max);
	void unthreadedFixedH(unsigned long end);

private:
	double tMax;
	double tCurr;
	double h;			// The time step
	std::vector<CBody*> bOne;		// Vector of 'real' (non-ghost) bodies
	std::vector<CBody*> bTwo;		// Second vector of 'real' bodies to allow pointer-toggle for 
	std::vector<CBody*> bRead;		// fastest possible I/O.
	std::vector<CBody*> bWrite;		// The toggling pointers for the 'real' bodies
	std::vector<CGhost*> gOne;		// Vector of 'ghost' bodies (test particles)
	std::vector<CGhost*> gTwo;		// Second vector of 'ghosts' for pointer toggling
	std::vector<CGhost*> gRead;		// Pointer toggles
	std::vector<CGhost*> gWrite;

	int nadded;

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

#endif // HASH_H
