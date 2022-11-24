#define TEST
#include "vulcan.h"
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

	double day = h;
	CSim* tsim = new CSim(3, t, day);
	tsim->setDebug(0);
	tsim->Type(simType::bulirschStoer);
	tsim->pyinit();
	CBody* sun = new CBody(1.989e30, 6.95508e8, 0.0, 0.0, 0.0, 0.0, day);
	CBody* uranus = new CBody(8.681e25, 2.362e7, 6.8e3, 0.0, 3.008e12, 0.0, day);
	CBody* neptune = new CBody(1.024e26, 2.431e7, 5.43e3, 0.0, 4.54e12, 0.0, day);
	CBody* jupiter = new CBody(1.8982e27, 6.991e7, 1.307e4, 0.0, 8.1662e11, 0.0, day);
	CBody* saturn = new CBody(5.683e26, 5.8232e7, 9.68e3, 0.0, 1.5155e12, 0.0, day);

	sun->Name("Sun"); 
	uranus->Name("Uranus");
	neptune->Name("Neptune");
	jupiter->Name("Jupiter");
	saturn->Name("Saturn");

	tsim->addPlanet(sun);
	tsim->addPlanet(uranus);
	tsim->addPlanet(neptune);
	tsim->addPlanet(jupiter);
	tsim->addPlanet(saturn);
	tsim->outputInterval(day);

	std::cout << green << " Initialization complete. " << res << std::endl;

	double avg = 0;
	unsigned long long sum = 0;
	for (int ii = 0; ii < nsamples; ii ++) {
		auto start = std::chrono::high_resolution_clock::now();	
		tsim->sim();
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
