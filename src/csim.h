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
#define INT 1
#define DOUBLE 2
#define SIMTIME_TYPE INT

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

#include "cghost.h"
#include "externs.h"
#include "format.h"
#include "force.h"
#include "global.h"
#include "vel.h"
#include "vec.h"
#include "pos.h"
#include "rpos.h"

class CBody;

enum threadmode {
	single,
	omp,
	manual
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
	void addPlanet(CBody* body);
	void addGhost(CGhost* body);
	CBody* at(int ii);
	CBody copy(int ii);
	double H();
	int count();
	void sort();
	int NVirtual();
	int NReal();

	void printForces();
	int writeConfiguration(const std::string& filename, bool overwrite = false);
	CSim* readConfiguration(const std::string& filename);

	void fixedHForce(CBody* body, CBody* wbody);
	void sim(threadmode t = threadmode::single);
	void threadedFixedH(int min, int max);
	void unthreadedFixedH(unsigned long end);

private:
	double tMax;		// The integration time
	double tCurr;		// Current time
	double h;			// The time step
	std::vector<CBody*> one;
	std::vector<CBody*> two;
	std::vector<CBody*> read;
	std::vector<CBody*> write;
	int nadded;
	int ndefs;
	int nplanets;
	int nghosts;

	bool forces;

	std::vector<void (CSim::*)(CBody*, CBody*)> calcs;
	int ncalcs;

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
