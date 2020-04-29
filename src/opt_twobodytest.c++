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
		nthreads = 0;
	}
	if (argn > 4) {
		extension = std::string(argv[4]);
	}
	else {
		extension = "";
	}

	double day = h;
	CSim* tsim = new CSim(2, t, day);
	tsim->pyinit();
	tsim->setDebug(1);
	tsim->Type(simType::mercury);
	CBody* sun = new CBody(1.989e30, 6.95508e8, 0.0, 0.0, 0.0, 0.0, day);
	CBody* earth = new CBody(5.97237e24, 6.371e6, 2.978e4, 0.0, 1.521e11, 0.0, day);

	sun->Name("Sun"); 
	earth->Name("Earth");

	tsim->addPlanet(sun);
	tsim->addPlanet(earth);
	tsim->outputInterval(h*100);

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
		std::cout << tsim->at(ii)->Name() << " - " << tsim->at(ii)->ncalcs << std::endl; 
	}
}
