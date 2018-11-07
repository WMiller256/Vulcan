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
#include <colors.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <limits>
#include <omp.h>

#define G 6.67408e-11

extern int nbodies;
extern bool warnings;
extern double minradius;
extern double maxradius;
extern int blockwidth;		// The width of each hash block

class Pos;				// Forward declared for use in CBody constructor
class CBody;			// Forward declared for use in CSim
class Hash;				// Forward declared for use in CSim

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
	CBody(double Mass, double Radius, double Velocity, bool initialize = true);
	CBody(double Mass, double Radius, double Velocity, double X, double Y, double Z);
	CBody(double Mass, double Radius, double Velocity, Pos pos);
	~CBody();

	void setParent(CBody* Parent);
	CBody* getParent();

	Pos pos();
	int originDist();

	std::string writeFormat(format f); 		// Defined in simio.c++

private:
	CBody* parent;				// Parent, i.e. the body that is being orbited
	double radius;				// Radius of the body
	double x;					// x pos
	double y;					// y pos
	double z;					// z pos
	double mass;				// Mass of the body
	double speed;				// Magnitude of linear velocity

	void init();
};

class Pos
{
public:
	Pos();
	Pos(double X, double Y, double Z);

	double X();
	double Y();
	double Z();
	void setX(double X);
	void setY(double Y);
	void setZ(double Z);

	int originDist();

	void print();

	bool operator==(Pos r) const;

private:
	double x;
	double y;
	double z;

	void init();
};

class Hash
{
private:
	typedef struct node {
		int depth;					// Number of bodies in this block
		int key;					// The hash key
		node* next;					// Pointer to the next node, for faster iterating
		std::vector<CBody*> bodies;	// Other bodies in the given hash block
	}* nodeptr; 
	nodeptr node_search(Pos pos);

public:
	Hash(int n = 0);				// Constructor

	nodeptr* table;					// Table of pointers to all the nodes
	int hash_f(Pos pos);			// Hash function
	void addNode(CBody* body);		// Adds a node to the 	
	CBody* find(Pos pos);

	void write(std::string filename);	// Defined in simio.c++
	
	int size;
};

#endif // HASH_H
