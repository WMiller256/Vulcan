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

#include <experimental/array>

#include "integrator.h"
#include "matrix.h"

class BulirschStoer : public Integrator {

public:
	BulirschStoer();
	void bulirschStoer(CBody*, CBody*);			// Main integration function
	vec acceleration(Pos &r, int &idx) const;	// Get the gravitational acceleration if body {read[idx]} were at position {r}

	int NSteps();

private:
	static int const nsteps;
	static int const steps[];

	// TODO Determine if false sharing is affecting BulirschStoer integration
	// if it is create two separate sets of vectors (or two arrays of vectors)
	// one to read from and one to write to with pointer toggling to swicth from 
	// one step to the next (body specific switching?)

	double tolerance;
	static double const grow;
	static double const shrink;
	std::valarray<double> error;	// error value for each body, used to check for step completion

	std::valarray<double> rscale; // Scale factors for calculating position error
	std::valarray<double> vscale; // Scale factors for calculating velocity error

	std::valarray<double> hc; // intermediate h-values for current step
	Matrix<double> hs;		  // intermediate h^2 values for all steps
	std::valarray<double> h2; // intermediate h*2 values for current step

	std::valarray<Pos> r;  // positions for modified midpoint integration
	std::valarray<Vel> v;  // velocities for modified midpoint integration
	std::valarray<Pos> rn; // next positions for modified midpoint integration
	std::valarray<Vel> vn; // next velocities for modified midpoint integration
	Matrix<Pos> dr; 	   // position values for polynomial extrapolation
	Matrix<Vel> dv; 	   // velocity values for polynomial extrapolation
	
	void init();
};

bool vecComp(vec const &l, vec const &r);

#endif // BULIRSCHSTOER_H
