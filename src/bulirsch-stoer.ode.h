#ifndef BULIRSCH_STOER_ODE_H
#define BULIRSCH_STOER_ODE_H

#include "csim.h"

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

#endif // BULIRSCH_STOER_ODE_H
