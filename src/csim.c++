#include "csim.h" 
#include "simio.h"

bool warnings = true;
int nthreads;
int debug = 0;
int nbodies;
unsigned long long cputime = 0;
unsigned long long polltime = 0;
unsigned long long maxTime = 0;
unsigned long long simulationtime = 0;
#if SIMTIME_TYPE == INT
std::atomic<unsigned long long> simTime;
#elif SIMTIME_TYPE == DOUBLE
std::atomic<double> simTime;
#endif
std::atomic<int> tocalc;
std::atomic<int> joinable;

std::mutex mtx;

CSim::CSim() {
	init();	
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
CSim::CSim(int n) {
	init();
	nbodies = n;
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
CSim::CSim(int n, double max, double step) {
	init();
	nadded = 0;
	nbodies = n;
	integrator->h = step;
	maxTime = max;
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
CSim::~CSim() {
	if (_pyinit) Py_Finalize();
}

void CSim::setDebug(int Debug) { debug = Debug;} 
void CSim::Type(simType t) { type = t; }
simType CSim::Type() { return type; }

void CSim::addBody(CBody* body) {
	integrator->one.push_back(body);
	// Force copy construction to create separate copy of 
	// body at different memory location - one set of bodies
	// is for reading from, the other is for writing to 
	// and they toggle every step
	CBody* wbody = new CBody(*body);
	integrator->two.push_back(wbody);
	nadded++;
}
void CSim::addPlanet(CBody* body) {
	body->Type(bodyType::planet);
	addBody(body);
}
void CSim::addGhost(CGhost* ghost) {
	ghost->Type(bodyType::ghost);
	integrator->one.push_back(ghost);
	// Force copy construction, see comment in CSim::addBody
	// for details.
	CGhost* wghost = new CGhost(*ghost);
	integrator->two.push_back(wghost);
	nadded++;
}
CBody* CSim::at(int ii) { return integrator->write[ii]; }
CBody CSim::copy(int ii) { return *integrator->write[ii]; }
double CSim::H() { return integrator->h; }
int CSim::count() { return nreal; }
void CSim::sort() {
	std::array<std::vector<CBody*>, 2> planets;
	std::array<std::vector<CBody*>, 2> ghosts;
	std::array<std::vector<CBody*>, 2> defs;
	for (int ii = 0; ii < nadded; ii++) {
		if (integrator->one[ii]->Type() == bodyType::planet) {
			planets[0].push_back(integrator->one[ii]);
			planets[1].push_back(integrator->two[ii]);
		}
		else if (integrator->one[ii]->Type() == bodyType::ghost) {
			ghosts[0].push_back(integrator->one[ii]);
			ghosts[1].push_back(integrator->two[ii]);
		}
		else {
			defs[0].push_back(integrator->one[ii]);
			defs[1].push_back(integrator->two[ii]);
		}
	}
	nreal = planets[0].size();
	nghosts = ghosts[0].size();
	ndefs = defs[0].size();
	std::array<std::valarray<CBody*>, 2> bodies;
	bodies[0].resize(nreal + nghosts + ndefs);
	bodies[1].resize(nreal + nghosts + ndefs);
	for (int ii = 0; ii < nreal; ii ++) {
		bodies[0][ii] = planets[0][ii];
		bodies[1][ii] = planets[1][ii];
	}
	for (int ii = nreal; ii < nghosts + nreal; ii ++) {
		bodies[0][ii] = ghosts[0][ii - nreal];
		bodies[1][ii] = ghosts[1][ii - nreal];
	}
	for (int ii = nreal + nghosts; ii < nghosts + nreal + ndefs; ii ++) {
		bodies[0][ii] = defs[0][ii - nreal - nghosts];
		bodies[1][ii] = defs[1][ii - nreal - nghosts];
	}
	for (int ii = 0; ii < bodies[0].size(); ii ++) {
		if (ii < nreal + nghosts) {
			bodies[0][ii]->idx = ii;
			bodies[1][ii]->idx = ii;
		}
		integrator->one[ii] = bodies[0][ii];
		integrator->two[ii] = bodies[1][ii];
	}
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
int CSim::NVirtual() {
	int size = integrator->read.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (integrator->read[ii]->Type() == bodyType::def) {
			ret++;
		}
	}
	return ret;
}
int CSim::NReal() {
	const int size = integrator->read.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (integrator->read[ii] != NULL && integrator->read[ii]->Type() != bodyType::def) {
			ret++;
		}
	}
	return ret;
}
void CSim::toggle(bool t) { _toggle = t; }

void CSim::ofile(const std::string& filename)     { binaryofile = filename; }
void CSim::outputInterval(const double& interval) { write_interval = interval; }
void CSim::binarywrite() {
	binaryout.open(binaryofile, std::ios::binary | std::ios::app);
	for (auto l : output) {
		binaryout.write((char*)&nbodies, 8);
		for (int ii = 0; ii < nbodies; ii ++) {
			for (int jj = 0; jj < 3; jj ++) {
				binaryout.write((char*)&(l[ii].first[jj]), 8);
			}
			for (int jj = 0; jj < 3; jj ++) {
				binaryout.write((char*)&(l[ii].second[jj]), 8);
			}
		}
	}
	binaryout.close();
	write_fix = simTime;
}
void CSim::csvwrite() {
    std::ofstream fp;
    fp.open("out.csv");
    for (int ii = 0; ii < energies.size(); ii ++) {
        fp << outtimes[ii] << "," << energies[ii] << "\n";
    }
    fp.close();
}

double CSim::calculate_energy() {
// Low optimization method to retrieve total energy of a system
	double energy = 0.0;
	for (int ii = 0; ii < nreal; ii ++) {
		for (int jj = ii; jj < nreal; jj ++) {
			if (ii != jj) energy += G * (integrator->write[jj]->m * integrator->write[ii]->m) / (integrator->write[jj]->r - integrator->write[ii]->r).norm();
		}
		energy += 0.5 * integrator->write[ii]->m * integrator->write[ii]->v.squared();
	}
	return energy;
}

void CSim::sim() {
	auto start = std::chrono::high_resolution_clock::now();
	binaryout = std::fstream(binaryofile, std::ios::out | std::ios::binary);
	binaryout.close();

	integrator = new Integrator();
	integrator->set(nadded, nghosts);
	bulirschStoer = new BulirschStoer();
	miller = new Miller();
	mercury = new Mercury(integrator->h);

	if (type == simType::miller) toggle(false);

	if (do_main) {
		if (type == simType::basic) {
			calcs.push_back(std::bind(&Integrator::main, this->integrator, std::placeholders::_1, std::placeholders::_2));
		}
		else if (type == simType::miller) {
			calcs.push_back(std::bind(&Miller::basic, this->miller, std::placeholders::_1, std::placeholders::_2));
		}
		else if (type == simType::bulirschStoer) {
			calcs.push_back(std::bind(&BulirschStoer::bulirschStoer, this->bulirschStoer, std::placeholders::_1, std::placeholders::_2));
		}
		else if (type == simType::mercury) {
			integrator = mercury;
			calcs.push_back(std::bind(&Mercury::bulirschStoer, this->mercury, std::placeholders::_1, std::placeholders::_2));
			mainCalcs.push_back(std::bind(&Mercury::resizeH, this->mercury));
		}
	}
	ncalcs = calcs.size();
	sort();
	simTime = 0;
	tocalc = 0;
	joinable = 0;
	if (nthreads < 1) {
		nthreads = 1;
	}
	if (nthreads > std::thread::hardware_concurrency() - 1) {		// If the specified number of threads is greater than the maximum 
		nthreads = std::thread::hardware_concurrency() - 1;			// of the current machine adjust accordingly.
	}
	if (nadded < nthreads) {
		nthreads = nadded;
	}
	else {
		while (nadded % nthreads != 0) {
			CBody* body = new CBody(0, 0, 0, 0, 0, 0, 1);
			addBody(body);
		}
	}
	sort();
	nbodies = nadded;
	nreal = NReal();
	positions.resize(nreal);
	for (int jj = 0; jj < nreal; jj ++) {
		positions[jj][0].push_back(integrator->write[jj]->r[0]);
		positions[jj][1].push_back(integrator->write[jj]->r[1]);
	}

	int block = nbodies / nthreads;
//			cpu_set_t set;
//			CPU_ZERO(&set);
	std::thread threads[nthreads];
	for (int ii = 0; ii < nthreads - 1; ii ++) {
		threads[ii] = std::thread(&CSim::integrate, this, block * ii, block * (ii + 1));
//				CPU_SET(ii, &set);
//				pthread_setaffinity_np(threads[ii].native_handle(), sizeof(cpu_set_t), &set);
	}
	threads[nthreads - 1] = std::thread(&CSim::integrate, this, block * (nthreads - 1), nbodies);
//			CPU_SET(nthreads - 1, &set);
//			pthread_setaffinity_np(threads[nthreads - 1].native_handle(), sizeof(cpu_set_t), &set);
	integrator->nbodies = nbodies;
#ifdef profiling
	start = std::chrono::high_resolution_clock::now();
#endif

	// Only attempt to integrate if step size is positive
	if (integrator->h > 0.0) {
		vec a;
		vec v;
		double previous = simTime;
		
		// Calculate energy for initial configuration
		energies.push_back(calculate_energy());
		
		// Main integration loop
		while (simTime < maxTime) {
			if (write_interval > 0.0 && simTime - write_fix >= write_interval) {
				std::valarray<std::pair<Pos, Vel>> line(nreal);
				for (int jj = 0; jj < nreal; jj ++) {
					line[jj] = std::make_pair(integrator->write[jj]->r, integrator->write[jj]->v);
					positions[jj][0].push_back(integrator->write[jj]->r[0] - integrator->write[0]->r[0]);
					positions[jj][1].push_back(integrator->write[jj]->r[1] - integrator->write[0]->r[1]);
				}
				outtimes.push_back(simTime);
//				energies.push_back(energies.back() - calculate_energy());
                energies.push_back(calculate_energy());
				output.push_back(line);
				write_fix = simTime;
			}
			tocalc = nthreads;

			// Atomic += operation (would be equivalent to simTime += integrator->h, but std::atomic<double> does not presently support)
            fetch_add(&simTime, integrator->h);
			if (!debug && simTime != previous) {
				print_percent(simTime, maxTime);
				previous = simTime;
			}

			println(in("CSim","sim")+"         Sim time - "+std::to_string(simTime),1);
			while (tocalc > 0) {}
			for (auto calc : mainCalcs) calc();

			if (_toggle) {
				// Toggling between read and write array is more efficient, but not every integration scheme supports it
				if (integrator->read == integrator->one) {
					integrator->read = integrator->two;
					integrator->write = integrator->one;
				}
				else {
					integrator->read = integrator->one;
					integrator->write = integrator->two;
				}
			}
		}
	}
	else {
		error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);
		exit(0);
	}
#ifdef profiling
	simulationtime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
	while (joinable < nthreads - 1) {}
	for (int ii = 0; ii < nthreads; ii ++) {
		threads[ii].join();
	}
	if (write_interval > 0.0) {
		binarywrite();
		csvwrite();
		if (_pyinit) {
			// Plot positions
			for (auto p : positions) {
				PyObject* x = PyList_New(0);
				PyObject* y = PyList_New(0);
				for (auto i : p[0]) {
					PyList_Append(x, Py_BuildValue("d", i));
				}
				for (auto j : p[1]) {
					PyList_Append(y, Py_BuildValue("d", j));
				}
				PyObject_CallFunction(pltPlot, "(OO)", x, y);
				PyObject_CallFunction(pltScatter, "([d],[d])", p[0][0], p[1][0]);
			}
			PyObject* fig = PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("gcf")), "()");
			PyObject_CallMethod(PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("gca")), "()"), "set_aspect", "s", (char*)("equal"));
			PyObject_CallFunction(pltSavefig, "(s)", (char*)("positions.png"));
			PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("close")), "(O)", fig);

			// Plot system energy over time
			PyObject* t = PyList_New(0);
			PyObject* e = PyList_New(0);
			for (int ii = 0; ii < outtimes.size(); ii ++) {
				PyList_Append(t, Py_BuildValue("d", outtimes[ii]));
				PyList_Append(e, Py_BuildValue("d", (energies[ii] - energies[0]) / energies[0]));
			}
			PyObject* av = PyList_New(0);
			PyObject* tv = PyList_New(0);
			int block = energies.size() / 100;
			block = block > 0.0 ? block : 1.0;
			for (int ii = 0; ii < energies.size() / block; ii ++) {
				double sum = 0.0;
				for (int jj = 0; jj < block; jj ++) {
					sum += energies[ii*block + jj];
				}
				PyList_Append(av, Py_BuildValue("d", sum / float(block)));
				PyList_Append(tv, Py_BuildValue("d", outtimes[ii*block]));
			}
			fig = PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("gcf")), "()");
			PyObject_CallFunction(pltScatter, "(OO)", tv, av);
			PyObject_CallFunction(pltSavefig, "(s)", (char*)("energies.png"));
			PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("close")), "(O)", fig);
			PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("close")), "(O)", fig);
		}
	}
}

void CSim::integrate(int min, int max) {
	int tot = 0;
	double prev = 0;
	double elapsed = 0;
	int ii = 0;
	if (max > NReal()) max = NReal();	// Neglect the virtual bodies (they exist only to create an optimal block width)
	while (simTime == 0) {}				// Wait for the simulation to start
	while (true) {
#ifdef profiling
		auto start = std::chrono::high_resolution_clock::now();
#endif
		elapsed = simTime - prev;
		if (elapsed > 0) {
#ifdef profiling
			polltime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
			start = std::chrono::high_resolution_clock::now();
#endif
			if (simTime >= maxTime) {
				tocalc--;
				break;
			}
			prev = simTime;
			for (int ii = min; ii < max; ii ++) {
				for (auto calc : calcs) {
					calc(integrator->read[ii], integrator->write[ii]);
				}
				print(in("CSim", "integrate")+"       Old Velocity of {"+cyan+integrator->read[ii]->Name()+res+"} is "+integrator->read[ii]->v.info(3)+"\n", 6);
				print(in("CSim", "integrate")+"       New Velocity of {"+cyan+integrator->write[ii]->Name()+res+"} is "+integrator->write[ii]->v.info(3)+"\n", 6);
				print(in("CSim", "integrate")+"       Old posiiton of {"+cyan+integrator->read[ii]->Name()+res+"} is "+integrator->read[ii]->r.info(3)+"\n", 6);
				print(in("CSim", "integrate")+"       New posiiton of {"+cyan+integrator->write[ii]->Name()+res+"} is "+integrator->write[ii]->r.info(3)+"\n", 6);
			}
			tocalc--;
#ifdef profiling
			cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
		}
	}
	joinable++;
}

void CSim::pyinit() {
	Py_Initialize();
	plt = PyImport_ImportModule("matplotlib.pyplot");
	if (plt == NULL) {
	    std::cout << "Could not load matplotlib.pyplot, check matplotlib installation or run with graphing disabled." << std::endl;
	    exit(1);
	}
	else {
    	pltScatter = PyObject_GetAttrString(plt, (char*)("scatter"));
    	pltPlot = PyObject_GetAttrString(plt, (char*)("plot"));
    	pltSavefig = PyObject_GetAttrString(plt, (char*)("savefig"));
    	pltShow = PyObject_GetAttrString(plt, (char*)("show"));
    }
	_pyinit = true;
}

void CSim::init() {
	print("Initializing new "+cyan+bright+"CSim"+res+"...");

	type = simType::basic;
	do_main = true;
	ncalcs = 0;
	
	print(green+" done\n"+res);
}

