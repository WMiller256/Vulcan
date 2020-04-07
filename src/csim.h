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
#define SIMTIME_TYPE DOUBLE

#include <iostream>
#include <iomanip>
#include <experimental/filesystem>
#include <math.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <limits>
#include <omp.h>
#include <pthread.h>

#include "colors.h"
#include "cghost.h"
#include "externs.h"
#include "format.h"
#include "force.h"
#include "global.h"
#include "vel.h"
#include "vec.h"
#include "pos.h"

class CBody;

enum threadmode {
	single,
	omp,
	manual
};
enum simType {
	basic,
	bulirschStoer,
	miller,
	symplectic
};

class CSim
{
public:
	CSim();
	CSim(int n);
	CSim(int n, double max, double h);
	~CSim();

	void setDebug(int Debug);
	void Type(simType t);
	simType Type();

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
	simType type;		// The simulation type

	double tMax;		// The integration time
	double tCurr;		// Current time
	double h;			// The time step
	std::vector<CBody*> one;
	std::vector<CBody*> two;
	std::vector<CBody*> read;
	std::vector<CBody*> write;
	int nadded;
	int ndefs;
	int nreal;
	int nghosts;

	bool forces;

	std::vector<std::function<void(CBody*, CBody*)>> calcs;
	int ncalcs;

	void init();
};

class BulirschStoer : public CSim {

public:
	BulirschStoer(CSim* sim = NULL);
	int step();
	vec force(CBody* body);

	int NSteps();

private:
	CSim* sim;				// To access the owning (CSim) methods and members
	double threshold;
	static int attempts;
	static int nsteps;
	static int steps[];

	void init();
};


class Miller : public CSim {

public:
	Miller(CSim* sim = NULL);
	int step(CBody* body, CBody* wbody);
	void force(CBody* body, CBody* wbody);

private:
	CSim* sim;
};


#endif // HASH_H
