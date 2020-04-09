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
	void main() override;			// Main integration function
	std::vector<Force> gravity();	// Calculates gravitational forces without updating

	int NSteps();

private:
	double threshold;
	static int attempts;
	static int nsteps;
	static int steps[];

	std::vector<double> rscale;
	std::vector<double> vscale;

	void init();
};

#endif // BULIRSCHSTOER_H
