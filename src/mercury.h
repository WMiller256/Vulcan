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

	void bulirschStoer(CBody*, CBody*);

private:
	int const nsteps = 12;
	double h;

	double tolerance;
	static double const grow;
	static double const shrink;
	std::valarray<double> error;   // Error value for each body, used to check that the completion condition is met

	std::valarray<double> rscale;  // Scale factors for calculating position errors
	std::valarray<double> vscale;  // Scale factors for calculating velocity errors

	std::valarray<double> hc;      // Intermediate h-values for current step

	std::valarray<Pos> r;    // Array to store positions in modified midterm integration scheme
	std::valarray<Pos> rn;   // Second array to store positions for integration
	std::valarray<Vel> v;    // Array to store velocities in modified midterm integration scheme
	std::valarray<Vel> vn;   // Second array to store velocities for integration
	Matrix<Pos> dr;          // Position values for polynomial extrapolation
	Matrix<Vel> dv;          // Velocity values for polynomial extrapolation
};

#endif // MERCURY_H
