/* bulirschStoer.h
 *
 * William Miller
 * Apr 7, 2020
 *
 * Bulirsch Stoer integrator class
 *
 */

#ifndef BULIRSCHSTOER_H
#define BULIRSCHSTOER_H

#include "integrator.h"

class BulirschStoer : public Integrator {

public:
	BulirschStoer();
	int step();
	void force() override;

	int NSteps();

private:
	double threshold;
	static int attempts;
	static int nsteps;
	static int steps[];

	void init();
};

#endif // BULIRSCHSTOER_H
