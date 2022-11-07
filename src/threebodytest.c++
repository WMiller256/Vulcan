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

	double day = h;
	CSim* tsim = new CSim(3, t, day);
	tsim->setDebug(0);
	tsim->Type(simType::miller);
	tsim->pyinit();
	CBody* sun = new CBody(1.989e30, 6.95508e8, 0.0, 0.0, 0.0, 0.0, day);
	CBody* saturn = new CBody(5.683e26, 5.8232e7, 1.018e4, 0.0, 1.3526e12, 0.0, day);
	CBody* jupiter = new CBody(1.8982e27, 6.991e7, 1.372e4, 0.0, 7.4052e11, 0.0, day);

	sun->Name("Sun"); 
	saturn->Name("Saturn");
	jupiter->Name("Jupiter");

	tsim->addPlanet(sun);
	tsim->addPlanet(saturn);
	tsim->addPlanet(jupiter);
	tsim->outputInterval(day * 1);

	std::cout << green << "Initialization complete. " << res << std::endl;

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
