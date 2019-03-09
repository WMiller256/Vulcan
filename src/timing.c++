#define TEST
#include "csim.h"
#include <chrono>
#include <fstream>
#include <sstream>

int main(int argn, char** argv) {
	double t;
	double h;
	int nsamples = 0;
	int nghosts = 0;
	std::string extension;
	threadmode thread = threadmode::manual;

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
		nsamples = atoi(argv[3]);
	}
	else {
		nsamples = 1;
	}
	if (argn > 4) {
		nghosts = atoi(argv[4]);
	}
	else {
		nghosts = 0;
	}
	if (argn > 5) {
		extension = std::string(argv[5]);
	}
	else {
		extension = "";
	}

	std::cout << green << " Initialization complete. " << res << std::endl;

	std::ofstream outfile;
	std::ostringstream name;
	name.precision(3);
	name << "../data/timing." << double(t) << "-" << std::scientific << double(h) << "." << nghosts << "." << extension << ".txt";
	outfile.open(name.str(), std::ios_base::app);

	double avg = 0;
	unsigned long long sum = 0;
	int n = 9 + nghosts;
	CSim* tsim = new CSim(8, t, h);
	tsim -> setDebug(2);
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

	for (int ii = 0; ii < nghosts; ii ++) {
		// Kuiper belt test particles
		CBody* ghost = new CBody(1e3, 1e4, 1e3, 1000*ii, 7.48e12, 0, day);
		ghost -> Name(std::to_string(ii));
		tsim -> addPlanet(ghost);
	}

	if (n > 11) n = 11;
	for (int jj = n; jj > 0; jj --) {
		nthreads = jj;
		sum = 0;
		avg = 0;
		for (int ii = 0; ii < nsamples; ii ++) {
			auto start = std::chrono::high_resolution_clock::now();	
			tsim -> sim(thread);
			auto end = std::chrono::high_resolution_clock::now();	
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			sum += microseconds;
		}
		avg = float(sum) / float(nsamples);
		std::cout << nthreads << " Average time:                    " << bright+magenta << avg << res << "\n" << std::endl;
		outfile << nthreads << " " << avg << std::endl;
	}
	delete tsim;
	outfile.close();
}
