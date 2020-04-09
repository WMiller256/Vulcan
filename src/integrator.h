/* integrator.h
 *
 * William Miller
 * Apr 4, 2020
 *
 * Base class for different integrators (Bulirsch-Stoer, Miller, Symplectic, Hybrid, etc)
 *
 */

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <vector>

class CSim;
class CBody;

class Integrator {
public:
	Integrator();
	virtual void main();		// Main integration function

protected:
	friend class CSim;

	static double h;			// The time step
	static int nadded;
	static int ndefs;
	static int nghosts;
	static int nreal;
	
	static std::vector<CBody*> one;
	static std::vector<CBody*> two;
	static std::vector<CBody*> read;
	static std::vector<CBody*> write;
};

#include "csim.h"

#endif // INTEGRATOR_H
