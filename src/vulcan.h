/*
 * vulcan.h
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
#include <Python.h>
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
class Mercury;
#include "integrator.h"
#include "bulirschStoer.h"
#include "miller.h"
#include "mercury.h"


#if SIMTIME_TYPE == INT
extern std::atomic<unsigned long long> simTime;
#elif SIMTIME_TYPE == DOUBLE
extern std::atomic<double> simTime;
#endif

extern std::mutex mtx;

class CBody;
class Miller;
class BulirschStoer;

// TODO Refactor - enumerations should be all-caps 

// ISSUE - Padding {integrator->read} and {integrator->write} with virtual bodies
// to acheive optimum block width is broken, the virtual bodies are NULL for
// unknown reason.

// TODO Make plotting optional

// TODO Make pointer toggling between integrator->one and integrator->two optional

enum simType {
	basic,
	bulirschStoer,
	miller,
	mercury,
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

	void toggle(bool t);

	void printForces();
	void ofile(const std::string& filename);
	void outputInterval(const double& interval);
	void binarywrite();
    void csvwrite();
    
	double calculate_energy();
	void sim();
	void integrate(int min, int max);

	void pyinit();

protected:
	Integrator* integrator;
	Miller* miller;
	BulirschStoer* bulirschStoer;
	Mercury* mercury;

	std::string binaryofile = "out.vln";
	std::fstream binaryout;
	double write_interval = -1.0;
	double write_fix = 0.0;
	std::vector<std::valarray<std::pair<Pos, Vel> > > output;
	std::vector<std::function<void(CBody*, CBody*)>> calcs;
	std::vector<std::function<void()>> mainCalcs;

	simType type;		// The simulation type

	int nadded;
	int ndefs;
	int nreal;
	int nghosts;

	bool do_main;
	bool _toggle = true;	// Enable or disable pointer toggling between read and write vector
	int ncalcs;

	bool _pyinit = false;   // If python C-api has been initialized
	PyObject* vln;
	PyObject* plot_positions;
	PyObject* plot_energy;
	PyObject* finalize;

	std::valarray<std::array<std::vector<double>, 2>> positions;
	std::vector<double> outtimes;
	std::vector<double> energies;
	double initial_energy;

	void init();
};


#endif // HASH_H
