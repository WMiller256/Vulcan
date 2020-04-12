#define TEST
#include "csim.h"
#include <chrono>

int main(int argn, char** argv) {
	double t;
	double h;
	int nsamples = 1;
	std::string extension;

	if (argn > 1) {
		t = atof(argv[1]);
	}
	else {
		t = 1000.0;
	}
	if (argn > 2) {
		h = atof(argv[2]);
	}
	else {
		h = 1.0;
	}
	if (argn > 3) {
		nthreads = atoi(argv[3]);
	}
	else {
		nthreads = 1;
	}
	if (argn > 4) {
		extension = std::string(argv[4]);
	}
	else {
		extension = "";
	}
	std::cout << t << " " << h << " " << nthreads << std::endl;

	CSim* tsim = new CSim(8, t, h);
	tsim -> setDebug(0);
	tsim->Type(simType::bulirschStoer);
	int day = int(h);
	CBody* sun = new CBody(1.989e30, 6.95508e8, 0.0, 0.0, 0.0, 0.0, day);
	CBody* mercury = new CBody(3.3011e23, 2.439e6, 4.7362e4, 0.0, 6.98169e10, 0.0, day);
	CBody* venus = new CBody(4.8675e24, 6.0518e6, 3.502e4, 0.0, 1.08939e11, 0.0, day);
	CBody* earth = new CBody(5.97237e24, 6.371e6, 2.978e4, 0.0, 1.521e11, 0.0, day);
	CBody* mars = new CBody(6.4171e23, 3.3895e6, 2.4007e4, 0.0, 2.492e11, 0.0, day);
	CBody* jupiter = new CBody(1.8982e27, 6.991e7, 1.307e4, 0.0, 8.1662e11, 0.0, day);
	CBody* saturn = new CBody(5.683e26, 5.8232e7, 9.68e3, 0.0, 1.5155e12, 0.0, day);
	CBody* uranus = new CBody(8.681e25, 2.362e7, 6.8e3, 0.0, 3.008e12, 0.0, day);
	CBody* neptune = new CBody(1.024e26, 2.431e7, 5.43e3, 0.0, 4.54e12, 0.0, day);

	sun -> Name("Sun"); 
	mercury -> Name("Mercury");
	venus -> Name("Venus");
	earth -> Name("Earth");
	mars -> Name("Mars");
	jupiter -> Name("Jupiter");
	saturn -> Name("Saturn");
	uranus -> Name("Uranus");
	neptune -> Name("Neptune");

	tsim -> addPlanet(sun);
	tsim -> addPlanet(mercury);
	tsim -> addPlanet(venus);
	tsim -> addPlanet(earth);
	tsim -> addPlanet(mars);
	tsim -> addPlanet(jupiter);
	tsim -> addPlanet(saturn);
	tsim -> addPlanet(uranus);
	tsim -> addPlanet(neptune);

	for (int ii = 0; ii < 0; ii ++) {
		// Test particles in the Kuiper belt
		CBody* ghost = new CBody(1e3, 1e4, 1e3, 1000*ii, 7.48e12, 0, day);
		ghost -> Name(std::to_string(ii));
		tsim -> addPlanet(ghost);
	}

	std::cout << green << " Initialization complete. " << res << std::endl;

	double avg = 0;
	unsigned long long sum = 0;
	for (int ii = 0; ii < nsamples; ii ++) {
		auto start = std::chrono::high_resolution_clock::now();	
		tsim -> sim();
		auto end = std::chrono::high_resolution_clock::now();	
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		sum += microseconds;
#ifdef profiling
		cputime /= nthreads;
		polltime /= nthreads;
		std::cout << "Computation time per thread: " << bright+magenta << cputime << res << std::endl;
#endif
//		std::cout << "Poll time:                   " << bright+magenta << polltime << res << std::endl;
//		std::cout << "Simulation total:            " << bright+magenta << simulationtime << res << std::endl;
//		std::cout << "Total time:                  " << bright+magenta << microseconds << res << std::endl;
	}
	avg = float(sum) / float(nsamples);
	std::cout << "\nAverage time:                    " << bright+magenta << avg << res << std::endl;
	for (int ii = 0; ii < tsim->count(); ii ++) {
		std::cout << tsim->at(ii)->Name() << " - " << tsim->at(ii)->totSteps << std::endl; 
	}
}
