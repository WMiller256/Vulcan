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

// TODO Consider refactoring derived classes to have integration methods within each
// derived class to allow different integration schemes. (i.e. Miller::BulirschStoer()
// can be different from (Mercury::BulirschStoer())

class CSim;
class CBody;

class Integrator {
public:
	Integrator();
	virtual void main(CBody* body, CBody* wbody);		// Main integration function
	void set(int b, int g);

protected:
	friend class CSim;

	static double h;			// The time step
	static int nghosts;
	static int nbodies;
	static int nreal;
	
	static std::vector<CBody*> one;
	static std::vector<CBody*> two;
	static std::vector<CBody*> read;
	static std::vector<CBody*> write;
};

#include "csim.h"

#endif // INTEGRATOR_H
