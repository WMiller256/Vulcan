/* mercury.h
 *
 * William Miller
 * Apr 15, 2020
 *
 * Intended to be as close a port as possible
 * to the original mercury hybrid symplectic
 * integration scheme. Derived from the 
 * Integrator base class
 *
 */

#ifndef MERCURY_H
#define MERCURY_H

#include "integrator.h"

class Mercury : public Integrator {

public:
	Mercury(double &h);

	void init();
	vec acceleration(Pos &r, int &idx) const;
	void resizeH();
	void bulirschStoer(CBody*, CBody*);

private:
	double const hmax = 1e8;
	static int const nsteps;
	static int const steps[];

	double tolerance;
	static double const grow;
	static double const shrink;
	std::valarray<double> error;   // Error value for each body, used to check that the completion condition is met
	std::vector<int> s;		   	   // The actually number of steps which were used each time

	std::valarray<double> rscale;  // Scale factors for calculating position errors
	std::valarray<double> vscale;  // Scale factors for calculating velocity errors

	std::valarray<double> hc;      // Intermediate h-values for current step
	Matrix<double> hs;      	   // Squared h-values to reduce floating point operations needed
	std::valarray<double> h2;      // Double h-values to reduce floating point operations needed

	std::valarray<Pos> r;    // Array to store positions in modified midterm integration scheme
	std::valarray<Pos> rn;   // Second array to store positions for integration
	std::valarray<Vel> v;    // Array to store velocities in modified midterm integration scheme
	std::valarray<Vel> vn;   // Second array to store velocities for integration
	Matrix<Pos> dr;          // Position values for polynomial extrapolation
	Matrix<Vel> dv;          // Velocity values for polynomial extrapolation
};

#endif // MERCURY_H
