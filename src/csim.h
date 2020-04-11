/*
 * csim.h
 *
 * William Miller
 * Nov 3, 2018
 *
 * Header for CSim classe. CSim contains
 * a vector of (CBody)s each of which contains the
 * orbital elements for each body in the simulation as well as
 * methods to simulate the given configuration of (CBody)s. 
 *
 */

#ifndef CSIM_H
#define CSIM_H

#define multhreading
#define G 6.67408e-11
#define INT 1
#define DOUBLE 2
#define SIMTIME_TYPE DOUBLE

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <experimental/filesystem>
#include <functional>
#include <iostream>
#include <iomanip>
#include <limits>
#include <math.h>
#include <mutex>
#include <omp.h>
#include <pthread.h>
#include <string.h>
#include <sstream>
#include <thread>
#include <valarray>
#include <vector>

#include "colors.h"
#include "cghost.h"
#include "externs.h"
#include "format.h"
#include "force.h"
#include "global.h"
#include "matrix.h"
#include "pos.h"
#include "vel.h"
#include "vec.h"

// Include order matters for Integrator and derived class headers
#include "integrator.h"
#include "bulirschStoer.h"
#include "miller.h"


#if SIMTIME_TYPE == INT
extern std::atomic<unsigned long long> simTime;
#elif SIMTIME_TYPE == DOUBLE
extern std::atomic<double> simTime;
#endif

class CBody;
class Miller;
class BulirschStoer;

// TODO Refactor - enumerations should be all-caps 

// TODO Issue - Reusing {CSim} instance causes segmentation fault because cleanup and 
// re-instantiation of the integrators is not properly managed

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

	void sim();
	void integrate(int min, int max);

protected:
	Integrator* integrator;
	Miller* miller;
	BulirschStoer* bulirschStoer;

	simType type;		// The simulation type

	int nadded;
	int ndefs;
	int nreal;
	int nghosts;

	bool do_main;

	std::vector<std::function<void(CBody*, CBody*)>> calcs;
	int ncalcs;

	void init();
};


#endif // HASH_H
