#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/program_options.hpp>
#include <chrono>
#include <thread>
#include "csim.h"

double function(double x, double y) {
	return y;
}

void group(BulirschStoer* bs, int start, int niterations) {
	double* y1 = new double;
	double* h_new = new double;
	for (int ii = start; ii < niterations; ii ++) {
		bs->simulate(function, 0, y1, ii, 0.001, h_new, 0.001, 0.01, 1);
	}
}

long int n;
int nthreads;

int main(int argn, char** argv) {
	BulirschStoer* bs = new BulirschStoer();
	double* y1 = new double;
	double* h_new = new double;
	std::ofstream outfile;

	outfile.open("data.txt", std::ios::app);

	if (argn > 1) {
		if (strcmp(argv[1], "omp") == 0) {
			std::cout << "Using " << cyan << "Open MP" << res << std::endl;
			outfile << argv[1] << " ";
			bs -> setThreading(threadmode::omp);
		}
		else if (strcmp(argv[1], "single") == 0) {
			std::cout << "Using " << cyan << "Single" << res << std::endl;
			bs -> setThreading(threadmode::single);
			outfile << argv[1] << " ";
		}
		else if (strcmp(argv[1], "explicit") == 0) {
			std::cout << "Using " << cyan << "Explicit" << res << std::endl;
			bs -> setThreading(threadmode::manual);
			outfile << argv[1] << " ";
		}
		else {
			std::cout << red << "No threadmode " << argv[1] << " exists, options are "+cyan+"omp"+res+", ";
			std::cout << cyan+"single"+res+", "+cyan+"explicit"+res+"." << std::endl;
			return 1;
		}
	}
	else {
		outfile << "single ";
	}

	if (argn > 3) {
		nthreads = atoi(argv[3]);
	}
	else {
		nthreads = 6;
	}

	if (argn > 2) {
		n = atoi(argv[2]);
	}
	else {
		n = 100000;
	}
	outfile << n << " ";
	auto start = std::chrono::high_resolution_clock::now();	
	switch(bs -> getThreading()) {
		case omp:
			omp_set_num_threads(nthreads);
			#pragma omp parallel for
			for (int ii = 0; ii < n; ii ++) {
				bs->simulate(function, 0, y1, ii, 0.001, h_new, 0.001, 0.01, 1);
			}
			break;
	
		case single:
			for (int ii = 0; ii < n; ii ++) {
				bs->simulate(function, 0, y1, ii, 0.001, h_new, 0.001, 0.01, 1);
			}
			break;
		
		case manual:				
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii] = std::thread(group, bs, ii*n/nthreads, (ii+1)*n/nthreads);
			}
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii].join();
			}
			break;
	}

	auto end = std::chrono::high_resolution_clock::now();	
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	std::cout << "Computation time: " << bright+magenta << microseconds << res << std::endl;
	outfile << microseconds << " " << nthreads << "\n";
}

int BulirschStoer::attempts = 12;
int BulirschStoer::steps[] = { 2,4,6,8,12,16,24,32,48,64,96,128 };

BulirschStoer::BulirschStoer() : CSim() {
	threading = threadmode::single;
}

double BulirschStoer::step(double (*f)(double, double), double x0, double y0, double x, int nsteps) {
	double h = (x - x0) / (double) nsteps;
	double h2 = 2 * h;
	double y1 = y0 + h * (*f)(x0, y0);
	double y2;

	while (--nsteps) {
		x0 += h;
		y2 = y0 + h2 * (*f)(x0, y1);
		y0 = y1;
		y1 = y2;
	}
	return 0.5 * (y0 + y1 + h * (*f)(x, y1));
}
int BulirschStoer::simulate(double (*f)(double, double), double y0, double* y1, double x, double h, double* h_new, double epsilon, double yscale, int rational_extrapolate) {
	double step_size[attempts];
	double tableau[attempts+1];
	double estimate;			// Current estimate
	double oestimate;			// Previous estimate
	double dum;

	int (*extrapolate)(double*, double*, double*, double, int);
	int error;

	if (yscale == 0.0) {
		return -3;
	}
	if (rational_extrapolate) {
		extrapolate = rationalExtrapolation;
	}
	else {
		extrapolate = polynomialExtrapolation;
	}
	estimate = step(f, y0, x, x+h, steps[0]);
	step_size[0] = (dum = h / (double) steps[0], dum * dum);
	error = extrapolate(y1, tableau, step_size, estimate, 0);
	if (error < 0) {
		return error - 1;
	}

	for (int ii = 0; ii < attempts; ii ++) {
		oestimate = *y1;
		estimate = step(f, y0, x, x+h, steps[ii]);
		step_size[0] = (dum = h / (double) steps[ii], dum * dum);

		error = extrapolate(y1, tableau, step_size, estimate, ii);
		if (error < 0) {
			return error - 1;
		}
		if (fabs(*y1 / yscale - oestimate / yscale) < epsilon) {
			if (ii > 1) {
				*h_new = 8.0 * h / (double) steps[ii-1];
			}
			else {
				*h_new = h;
				return 0;
			}
		}
	}
	return -1;
}
int BulirschStoer::rationalExtrapolation(double* fzero, double* tableau, double* x, double f, int n) {
	double t, up, across, denominator, dum;

	if (n == 0) {
		*fzero = f;
		tableau[0] = f;
		return 0;
	}
	if (x[n] == 0.0) {
		*fzero = f;
		return -2;
	}
	across = 0.0;
	up = tableau[0];
	tableau[0] = f;

	for (int ii = 0; ii <= n; ii ++) {
		denominator = tableau[ii-1] - across;
		if (denominator == 0.0) {
			return -1;
		}
		dum = 1.0 - (tableau[ii-1] - up) / denominator;
		denominator = (x[n-ii] / x[n]) * dum - 1.0;
		if (denominator == 0.0) {
			return -1;
		}
		t = tableau[ii-1] + (tableau[ii-1] - up) / denominator;
		across = up;
		up = tableau[ii];
		tableau[ii] = t;
	}
	*fzero = t;
	return 0;	
}
int BulirschStoer::polynomialExtrapolation(double* fzero, double* tableau, double* x, double f, int n) {
	double backTwo;		// T[row,col-2]
	double old;			// T[row-1,col]
	double new_value;	// T[row,col]
	double vdiff;		// T[row,col] - T[row-1,col]
	double bdiff;		// T[row,col] - T[row,col-1]
	double fdiff;		// T{row,col] - T{row-1,col-1]
	double denominator;

	if (n == 0) {
		tableau[0] = f;
		return 0;
	}
	if (x[n] == 0.0) {
		*fzero = f;
		return -2;
	}
	
	backTwo = 0.0;
	old = tableau[0];
	tableau[0] = f;
	for (int ii = 0; ii < n; ii ++) {
		vdiff = tableau[ii] - old;
		bdiff = tableau[ii] - backTwo;
		fdiff = bdiff - vdiff;
		denominator = (x[n-ii-1] / x[n]) * fdiff - bdiff;
		if (denominator == 0.0) {
			return -1;
		}
		backTwo = old;
		old = tableau[ii+1];
		tableau[ii+1] = tableau[ii] + vdiff * bdiff / denominator;
	}
	*fzero = tableau[n];
	return 0;
}

void BulirschStoer::setThreading(threadmode t) {
	threading = t;
}
threadmode BulirschStoer::getThreading() {
	return threading;
}
